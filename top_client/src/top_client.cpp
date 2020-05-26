#include <top_client.hpp>
#include <cstring>

unsigned long long top_client::_client_order_id = 0;

void top_client::handle_msg(const char* ptr, std::size_t size)
{
	const char* p = ptr;
	if (!_buffer.empty())
	{
		auto ori = _buffer.size();
		_buffer.resize(ori + size);
		std::memcpy(&_buffer[ori], ptr, size);
		p = &_buffer[0];
		size += ori;
	}
	while (size > sizeof(dbp::top::header))
	{
		const auto& h = *static_cast<const dbp::top::header*>(static_cast<const void*>(p));
		if (size >= h.len)
		{
			if (dbp::top::api_type_flag::error == h.flag)
			{
				auto it = _order_map.find(h.order_id);
				if (_order_map.end() != it)
				{
					auto& odr = it->second;
					odr.status = dbp::top::order_status::rejected;
					std::string reject_reason = "error_code:";
					const auto& e_reply = *static_cast<const dbp::top::error_reply*>(static_cast<const void*>(p));
					reject_reason += std::to_string(static_cast<uint64_t>(e_reply.code));
					std::size_t cp_size = reject_reason.length() + 1;
					if (cp_size < sizeof(odr.reject_reason))
						std::memcpy(odr.reject_reason, reject_reason.c_str(), reject_reason.length() + 1);
					else
					{
						std::memcpy(odr.reject_reason, reject_reason.c_str(), sizeof(odr.reject_reason) - 1);
						odr.reject_reason[sizeof(odr.reject_reason) - 1] = 0;
					}
					if (dbp::top::order_side::buy == odr.side)
					{
						auto turnover = odr.price * odr.quantity;
						_buy_power += turnover;
					}
					if (_on_order)
					{
						_on_order(odr);
					}
				}
			}
			else
			{
				switch (h.api_id)
				{
					case dbp::top::api_id_flag::login:
					{

						const auto& login_response = *static_cast<const dbp::top::login_response*>(static_cast<const void*>(p));
						if (dbp::top::logon_response::success == login_response.response)
						{
							std::fprintf(stderr, "Logon Success");
							std::memcpy(_session_id, login_response.session_id, sizeof(login_response.session_id));
							_ready = true;
						}
						else
						{
							std::fprintf(stderr, "Logon Fail");
							clean();
						}
						if (_on_login)
							_on_login(login_response);
						break;
					}
					case dbp::top::api_id_flag::new_order:
					case dbp::top::api_id_flag::modify_order:
					case dbp::top::api_id_flag::cancel_order:
					case dbp::top::api_id_flag::odr_push:
					{
						const auto& report = *static_cast<const dbp::top::order_report*>(static_cast<const void*>(p));
						auto it = _order_map.find(h.order_id);
						if (_order_map.end() != it)
						{
							if (dbp::top::order_side::buy == report.side)
							{
								if (dbp::top::report_type::order_cancel_approve == report.rep_type)
								{
									_buy_power += it->second.price * it->second.remain_quantity;
								}
								else if (dbp::top::report_type::order_modify_approve == report.rep_type)
								{
									_buy_power += it->second.ori_price * it->second.ori_quantity;
								}
								else if (dbp::top::report_type::order_modify_reject == report.rep_type)
								{
									_buy_power += it->second.price * it->second.quantity;
								}
								else if (dbp::top::report_type::order_fill == report.rep_type)
								{
									_buy_power += (it->second.price - it->second.match_price) * it->second.match_quantity;
								}
							}
							else
							{
								if (dbp::top::report_type::order_fill == report.rep_type)
								{
									_buy_power += it->second.match_price * it->second.match_quantity;
								}
							}
							it->second = report;
							if (_on_order)
							{
								_on_order(it->second);
							}
						}
						break;
					}
					case dbp::top::api_id_flag::change_pass:
					case dbp::top::api_id_flag::order_report:
					case dbp::top::api_id_flag::all_order_report:
					case dbp::top::api_id_flag::all_filled_order_report:
					case dbp::top::api_id_flag::all_open_order_report:
					case dbp::top::api_id_flag::cash_balance:
					case dbp::top::api_id_flag::instrument_balance:
					case dbp::top::api_id_flag::heart_beat:
					case dbp::top::api_id_flag::local_time:
					default:
					{
						break;
					}
				}
			}
			p += h.len;
			size -= h.len;
		}
		else
		{
			break;
		}
	}
	if (0 != size)
	{
		if (_buffer.empty())
		{
			_buffer.resize(size);
			std::memcpy(&_buffer[0], p, size);
		}
		else
		{
			std::memmove(&_buffer[0], &_buffer[_buffer.size() - size], size);
			_buffer.resize(size);
		}
	}
}

top_client::top_client
(
	const std::string& user,
	const std::string& pass,
	unsigned long long buy_power
):
	_buffer(),
	_session_id{0},
	_order_map(),
	_login(user.c_str(), pass.c_str()),
	_on_order(),
	_on_login(),
	_buy_power(buy_power),
	_ready(false)
{
}

top_client::top_client(top_client&& client):
	_buffer(std::move(client._buffer)),
	_session_id{},
	_login(std::move(client._login)),
	_on_order(std::move(client._on_order)),
	_on_login(std::move(client._on_login)),
	_buy_power(client._buy_power),
	_ready(client._ready)
{
	std::memcpy(_session_id, client._session_id, sizeof(_session_id));
	client._ready = false;
}

top_client& top_client::operator= (top_client&& client)
{
	_buffer = std::move(client._buffer);
	_login = std::move(client._login);
	_on_order = std::move(client._on_order);
	_on_login = std::move(client._on_login);
	_buy_power = client._buy_power;
	_ready = client._ready;
	std::memcpy(_session_id, client._session_id, sizeof(_session_id));
	client._ready = false;
	return *this;
}

dbp::top::enhance_order top_client::new_order
(
	unsigned long long quantity,
	unsigned long long price,
	unsigned int code,
	dbp::top::order_side side,
	dbp::top::order_type type,
	dbp::top::aon_type aon,
	dbp::top::ignore_price_type ignore,
	unsigned int broker_id
)
{
	dbp::top::order_report report;
	if (!_ready)
	{
		report.order_id = 0;
		return report;
	}
	else
	{
		if (dbp::top::order_side::buy == side)
		{
			auto turnover = quantity * price;
			if (turnover > _buy_power)
			{
				report.order_id = 0;
				string reason = "exist buy power";
				memcpy(report.reject_reason, reason.c_str(), reason.size());
				//"exist buy power                                                                                 "
				//"                                                                                                "
				return report;
			}
			else
			{
				_buy_power -= turnover;
			}
		}
		dbp::top::new_order_request request
		(
			&_session_id[0],
			++_client_order_id,
			quantity,
			price,
			code,
			side,
			type,
			aon,
			ignore,
			broker_id
		);
		dbp::top::header& h_report = report;
		dbp::top::header& h_request = request;
		h_report = h_request;
		report.order_ref = 0;
		report.quantity = quantity;
		report.price = price;
		report.code = code;
		report.side = side;
		report.type = type;
		report.aon = aon;
		std::string sbroker_id = std::to_string(broker_id);
		std::strncpy(report.broker_id, sbroker_id.c_str(), sizeof(report.broker_id));
		_order_map[report.order_id] = report;
		send(request);
		return report;
	}
}


bool top_client::modify_order(unsigned long long order_id, unsigned long long new_quantity, unsigned long long new_price)
{
	if (!_ready)
	{
		return false;
	}
	else
	{
		auto it = _order_map.find(order_id);
		if (_order_map.end() == it)
		{
			return false;
		}
		auto& report = it->second;
		if (report.is_on_fly())
		{
			return false;
		}
		if (dbp::top::order_side::buy == report.side)
		{
			auto turnover = new_quantity * report.price;
			if (turnover > _buy_power)
			{
				return false;
			}
			else
			{
				_buy_power -= turnover;
			}
		}
		report.ori_quantity = report.quantity;
		report.ori_price = report.price;
		report.quantity = new_quantity;
		report.price = new_price;
		dbp::top::modify_order_request request
		(
			&_session_id[0],
			report.order_id,
			report.order_ref,
			new_quantity,
			new_price
		);
		report.order_ref = 0;
		send(request);
		return true;
	}
}


bool top_client::cancel_order(unsigned long long order_id)
{
	if (!_ready)
	{
		return false;
	}
	else
	{
		auto it = _order_map.find(order_id);
		if (_order_map.end() == it)
		{
			return false;
		}
		auto& report = it->second;
		if (report.is_on_fly())
		{
			return false;
		}
		dbp::top::cancel_order_request request
		(
			&_session_id[0],
			report.order_id,
			report.order_ref
		);
		report.order_ref = 0;
		send(request);
		return true;
	}
}


void top_client::set_on_order(order_event&& on_order)
{
	_on_order = std::move(on_order);
}

void top_client::set_on_login(login_event&& on_login)
{
	_on_login = std::move(on_login);
}

const dbp::top::enhance_order* top_client::get_order(unsigned long long order_id)
{
	auto it = _order_map.find(order_id);
	if (_order_map.end() == it)
	{
		return nullptr;
	}
	else
	{
		return &it->second;
	}
}

void top_client::set_buy_power(unsigned long long buy_power)
{
	_buy_power = buy_power;
}

unsigned long long top_client::get_buy_power()
{
	return _buy_power;
}

void top_client::login()
{
	std::fprintf(stderr, "do login\n");
	send(_login);
	std::fprintf(stderr, "end do login\n");
}

void top_client::clean()
{
	close();
	_ready = false;
}
