#include <user.hpp>
#include <algo.hpp>

user::~user()
{
	for (auto& item : _algos)
		delete item.second;
}

dbp::top::enhance_order user::new_order
(
		algo* algo,
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
	auto odr = _client->new_order(quantity, price, code, side, type, aon, ignore, broker_id);
	if (0 != odr.order_id)
		_odr_map[odr.order_id] = algo;
	return odr;
}

bool user::modify_order(unsigned long long order_id, unsigned long long new_quantity, unsigned long long new_price)
{
	return _client->modify_order(order_id, new_quantity, new_price);
}

bool user::cancel_order(unsigned long long order_id)
{
	return _client->cancel_order(order_id);
}

bool user::add_algo(const std::string& name, const std::string lib, json& cfg)
{
	auto al = algo::get_algo(*this, name, lib, cfg);
	if (nullptr == al)
		return false;
	auto it = _algos.find(name);
	if (_algos.end() != it)
		delete it->second;
	_algos[name] = al;
	return true;
}

void user::run()
{
	_client->run();
	Tradable msg;
	if(_md.try_dequeue(msg))
	{
		switch(msg.m_MsgType)
		{
		case MsgType::OMDC_BOOK:
			for (auto& item : _algos)
				item.second->on_omdc_book(msg);
			break;
		case MsgType::OMDD_BOOK:
			for (auto& item : _algos)
				item.second->on_omdd_book(msg);
			break;
		case MsgType::OMDC_TRADE:
			for (auto& item : _algos)
				item.second->on_omdc_trade(msg);
			break;
		case MsgType::OMDD_TRADE:
			for (auto& item : _algos)
				item.second->on_omdd_trade(msg);
			break;
		case MsgType::TCP_BOOK:
			for (auto& item : _algos)
				item.second->on_tcp_trade(msg);
			break;
		case MsgType::TCP_TRADE:
			for (auto& item : _algos)
				item.second->on_tcp_trade(msg);
			break;
		case MsgType::COMMAND:
			{
				auto ptr = msg.m_AlgoBase;
				auto& cmd = *ptr;
				if (cmd.id == _id)
				{
					auto it = _algos.find(cmd.algo_name);
					if (_algos.end() != it)
					{
						it->second->handle_command(cmd);
					}
				}
			}
			break;
		case MsgType::ORDER_LIST:
			{
				auto base = msg.m_AlgoBase;
				auto ptr = dynamic_cast<user::user_order_list*>(base);
				if (ptr->id == _id)
				{
					for (const auto& item : _odr_map)
					{
						auto name = item.second->get_name();
						if (ptr->algo_name != "")
						{
							if (ptr->algo_name != name)
							{
								continue;
							}
						}
						const auto& order_id = item.first;
						user::algo_order odr(*_client->get_order(order_id));
						odr.algo_name = name;
						ptr->orders.push_back(odr);
					}
					std::sort(ptr->orders.begin(), ptr->orders.end(),
							[](const algo_order& x, const algo_order& y) ->bool
							{
								return x.order_id < y.order_id;
							});
					base->on_command();
				}
			}
			break;
		default:
			break;
		}
	}
}

void user::set_buy_power(unsigned long long buy_power)
{
	_client->set_buy_power(buy_power);
}

unsigned long long user::get_buy_power()
{
	return _client->get_buy_power();
}

bool user::get_top_buy_power(const std::string& ref, const std::string& algo_name)
{
	return _client->get_top_buy_power(ref, algo_name);
}

unsigned long long user::get_id()
{
	return _id;
}

algo* user::get_algo(const std::string& name)
{
	auto it = _algos.find(name);
	if (_algos.end() != it)
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

std::unordered_map<std::string, std::string> user::get_algos()
{
	std::unordered_map<std::string, std::string> rt;
	for (const auto& item : _algos)
		rt[item.first] = item.second->get_name();
	return rt;
}

void user::handler_order(const dbp::top::enhance_order& odr)
{
	auto it = _odr_map.find(odr.order_id);
	if (_odr_map.end() != it)
	{
		it->second->handler_order(odr);
	}
}

void user::handler_buy_power(const std::string& ref, const std::string& algo_name, long long buy_power)
{
	user::user_buy_power* buy_power_msg = user::user_buy_power_pool.get_obj();
	buy_power_msg->al = nullptr;
	buy_power_msg->algo_name = algo_name;
	buy_power_msg->id = _id;
	buy_power_msg->ref = ref;
	buy_power_msg->buy_power = buy_power;
	buy_power_msg->on_command();
}

rapid_ring::spsc_ring_buffer_object_pool<user::user_order_list, 8192> user::user_order_list_pool;
rapid_ring::spmc_ring_buffer_object_pool<user::user_buy_power, 8192> user::user_buy_power_pool;



