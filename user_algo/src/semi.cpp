#include <semi.hpp>


semi::semi(user& u, const std::string& name):
	algo(u, name),
	_o_map(),
	_u_map(),
	_w_map()
{
}

void semi::on_omdc_book(const Tradable& tradable)
{
	auto it = _w_map.find(tradable.m_Code);
	if (_w_map.end() != it)
	{
		for (const auto& p : it->second)
		{
			if (p->auto_sell())
			{
					p->on_book(tradable);
				}
			}
	}
}

void semi::on_omdd_book(const Tradable&)
{
}

void semi::on_omdc_trade(const Tradable& tradable)
{
	auto it = _u_map.find(tradable.m_Code);
	if (_u_map.end() != it)
	{
		for (const auto& p : it->second)
		{
			if (p->auto_buy() || p->auto_sell())
			{
				p->on_trade(tradable);
			}
		}
	}
}

void semi::on_omdd_trade(const Tradable& tradable)
{
	auto it = _u_map.find(tradable.m_Code);
	if (_u_map.end() != it)
	{
		for (const auto& p : it->second)
		{
			if (p->auto_buy() || p->auto_sell())
			{
				p->on_trade(tradable);
			}
		}
	}
}

void semi::handler_order(const dbp::top::enhance_order& odr)
{
	auto it = _o_map.find(odr.order_id);
	if(_o_map.end() != it)
	{
		it->second->on_order(odr);
	}
}

void semi::handle_command(const algo_msg_base&)
{
}

algo_msg_base* semi::json_to_msg(json&)
{
	return nullptr;
}

json semi::msg_to_json(algo_msg_base* msg)
{
	if (0 == msg->msg_type)
	{
		auto odr_msg = dynamic_cast<algo_odr_msg*>(msg);
		return odr_msg->to_json();
	}
	else
	{
		return msg->to_json();
	}
}

