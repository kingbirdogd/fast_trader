#include <semi.hpp>


semi::semi(user& u, const std::string& name):
	algo(u, name),
	_o_map(),
	_u_map(),
	_w_map(),
	_p_map()
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
	}else{
		auto it = _u_map.find(tradable.m_Code);
		if (_u_map.end() != it)
		{
			for (const auto& p : it->second)
			{
				auto best_bid_vol = static_cast<unsigned long long>(tradable.m_Bid[0].m_uQuantity);
				auto best_ask_vol = static_cast<unsigned long long>(tradable.m_Ask[0].m_uQuantity);

				auto buyratio = static_cast<unsigned long long>(best_ask_vol / (best_bid_vol + best_ask_vol) * 100);
				auto sellratio = static_cast<unsigned long long>(best_bid_vol / (best_bid_vol + best_ask_vol) * 100);

				if ( p->auto_buy() && p->ratio_buy()>0 )
				{
					if(buyratio <= p->ratio_buy()){
						p->buy(false, 0);
					}

				}
				if(p->auto_sell() && p->ratio_sell()>0){

					if(sellratio <= p->ratio_sell()){
						p->sell(false, 0);
					}
				}
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


std::string semi::set_pair(pair&& p, bool no_change)
{
	auto ref = p.ref();
	auto it = _p_map.find(ref);
	if (_p_map.end() != it)
	{
		if ((it->second.underlying_code() != p.underlying_code())
				|| (it->second.warrant_code() != p.warrant_code()))
		{
			if (it->second.is_buying())
			{
				return "can't change code while Buying";
			}
			else if (it->second.is_selling())
			{
				return "can't change code while Selling";
			}
			else if (0 != it->second.position())
			{
				return "can't change code while has position";
			}
			auto underlying_code = it->second.underlying_code();
			auto warrant_code = it->second.warrant_code();
			auto node = &(it->second);
			auto u_it = _u_map.find(underlying_code);
			if (_u_map.end() != u_it)
			{
				if (u_it->second.end() != u_it->second.find(node))
				{
					u_it->second.erase(node);
					if(u_it->second.empty())
					{
						_u_map.erase(u_it);
					}
				}
			}
			auto w_it = _w_map.find(warrant_code);
			if (_w_map.end() != w_it)
			{
				if (w_it->second.end() != w_it->second.find(node))
				{
					w_it->second.erase(node);
					if(w_it->second.empty())
					{
						_w_map.erase(w_it);
					}
				}
			}
		}
		if (0 == p.position())
		{
			p.set_position(it->second.position());
		}
		p.set_auto_buy_id(it->second.auto_buy_id());
		p.set_auto_sell_id(it->second.auto_sell_id());
		p.set_is_buying(it->second.is_buying());
		p.set_is_selling(it->second.is_selling());
		if (no_change)
		{
			p.set_auto_buy(it->second.auto_buy());
			p.set_auto_sell(it->second.auto_sell());
		}
		it->second = std::move(p);
	}
	else
	{
		_p_map.emplace(ref, std::move(p));
		it = _p_map.find(ref);
	}
	auto underlying_code = it->second.underlying_code();
	auto warrant_code = it->second.warrant_code();
	auto node = &(it->second);
	_u_map[underlying_code].insert(node);
	_w_map[warrant_code].insert(node);
	return "SUCCESS";
}


std::string semi::delete_pair(const std::string& ref, pair*& pref)
{
	pref = nullptr;
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	pref = &p;
	auto underlying_code = p.underlying_code();
	auto warrant_code = p.warrant_code();
	auto u_it = _u_map.find(underlying_code);
	if (_u_map.end() != u_it)
	{
		if (u_it->second.end() != u_it->second.find(pref))
		{
			u_it->second.erase(pref);
			if(u_it->second.empty())
			{
				_u_map.erase(u_it);
			}
		}
	}
	auto w_it = _w_map.find(warrant_code);
	if (_w_map.end() != w_it)
	{
		if (w_it->second.end() != w_it->second.find(pref))
		{
			w_it->second.erase(pref);
			if(w_it->second.empty())
			{
				_w_map.erase(w_it);
			}
		}
	}
	p.set_ref(ref);
	return "SUCCESS";
}

std::string semi::get_pair(const std::string& ref, pair*& pref)
{
	pref = nullptr;
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	pref = &p;
	return "SUCCESS";
}


std::string semi::force_buy(unsigned long long quantity, pair*& pref, const std::string& ref)
{
	pref = nullptr;
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	pref = &p;
	if (p.buy(false, quantity))
	{
		return  "SUCCESS";
	}
	else
	{
		return "fail new buy order not valid";
	}

}

std::string semi::force_sell(unsigned long long quantity, pair*& pref, const std::string& ref)
{
	pref = nullptr;
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	pref = &p;
	auto result = p.sell(0, false, quantity);
	if (pair::sell_result::SUCCESS == result)
	{
		return "SUCCESS";
	}
	else if (pair::sell_result::NOTHING_TO_SELL == result)
	{
		return "fail NOTHING_TO_SELL";
	}
	else if (pair::sell_result::SELLING == result)
	{
		return "fail SELLING";
	}
	else if (pair::sell_result::SHORT_SELL == result)
	{
		return "fail SHORT_SELL";
	}
	else if (pair::sell_result::NEW_SELL_ODR_FAIL == result)
	{
		return "fail NEW_SELL_ODR_FAIL";
	}
	else
	{
		return "fail UNKNOW";
	}
}


void semi::position(algo_odr_position& msg) const
{
	for (const auto& it : _p_map)
	{
		std::string key = std::to_string(it.second.warrant_code());
		msg.position[key] += it.second.position();
	}
}

void semi::handle_command(algo_msg_base& msg)
{
	msg.on_command();
}

algo_msg_base* semi::json_to_msg(json& json)
{
	algo_set* pset = nullptr;
	algo_force_buy* pforce_buy = nullptr;
	algo_force_sell* pforce_sell = nullptr;
	try
	{
		auto cmd = json["cmd"].get<std::string>();
		auto ref = json["ref"].get<std::string>();
		if (cmd == "position")
		{
			auto msg = new algo_odr_position();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			return msg;
		}
		else if (cmd == "set")
		{
			pset = new algo_set();
			auto& p = pset->p;
			pset->al = this;
			pset->algo_name = _name;
			pset->id = _u.get_id();
			pset->ref = ref;
			auto type = json["type"].get<std::string>();
			if (type != "bull" && type != "bear")
			{
				auto msg = new algo_err_msg();
				msg->al = this;
				msg->algo_name = _name;
				msg->id = _u.get_id();
				msg->ref = ref;
				msg->err = "set only support bull bear";
				delete pset;
				return msg;
			}
			p._algo = this;
			p._ref = ref;
			p._is_bull = true;
			if (type == "bear")
			{
				p._is_bull = false;
			}
			p._is_omdd = false;
			p._warrant_code = json["warrant_code"].get<unsigned int>();
			p._underlying_code = 0;
			try
			{
				p._underlying_code = json["underlying_code"].get<unsigned int>();
				p._underlying_symbol = "";
			}
			catch(...)
			{
				std::string str_underlying = json["underlying_code"].get<std::string>();
				p._underlying_symbol = str_underlying;
				auto it_omdd = nameToCode.find(str_underlying);
				if (nameToCode.end() == it_omdd)
				{
					auto msg = new algo_err_msg();
					msg->al = this;
					msg->algo_name = _name;
					msg->id = _u.get_id();
					msg->ref = ref;
					msg->err = "fail command set underlying code omdd mapping not found";
					delete pset;
					return msg;
				}
				p._underlying_code = it_omdd->second;
				p._is_omdd = true;
			}
			p._buy_trriger = json["buy_trriger"].get<unsigned long long>();
			p._sell_trriger = json["sell_trriger"].get<unsigned long long>();
			p._buy_price = json["buy_price"].get<unsigned long long>();
			p._sell_price = json["sell_price"].get<unsigned long long>();
			p._bottom_price = json["bottom_price"].get<unsigned long long>();
			p._ceiling_price = json["ceiling_price"].get<unsigned long long>();
			p._auto_buy_quantity = json["auto_buy_quantity"].get<unsigned long long>();
			auto action = json["action"].get<std::string>();
			if (action == "BUY")
			{
				p._auto_buy = true;
				p._auto_sell= false;
			}
			else if (action == "SELL")
			{
				p._auto_buy = false;
				p._auto_sell= true;
			}
			else if (action == "AUTO")
			{
				p._auto_buy = true;
				p._auto_sell= true;
			}
			else if (action == "STOP")
			{
				p._auto_buy = false;
				p._auto_sell= false;
			}
			else if (action == "NOCHANGE")
			{
				p._auto_buy = false;
				p._auto_sell= false;
				pset->no_change = true;
			}
			else
			{
				auto msg = new algo_err_msg();
				msg->al = this;
				msg->algo_name = _name;
				msg->id = _u.get_id();
				msg->ref = ref;
				msg->err = std::string("fail command set, action:") + action = " is not support";
				delete pset;
				return msg;
			}
			p._early_buy_qty = json["early_buy_qty"].get<unsigned long long>();
			p._early_sell_qty = json["early_sell_qty"].get<unsigned long long>();
			p._ratio_buy = json["ratio_buy"].get<unsigned long long>();
			p._ratio_sell = json["ratio_sell"].get<unsigned long long>();
			return pset;
		}
		else if (cmd == "delete")
		{
			auto msg = new algo_del();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			return msg;
		}
		else if (cmd == "get")
		{
			auto msg = new algo_get();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			return msg;
		}
		else if (cmd == "force_buy")
		{
			pforce_buy = new algo_force_buy();
			pforce_buy->al = this;
			pforce_buy->algo_name = _name;
			pforce_buy->id = _u.get_id();
			pforce_buy->ref = ref;
			pforce_buy->quantity = json["quantity"].get<unsigned long long>();
			return pforce_buy;
		}
		else if (cmd == "force_sell")
		{
			pforce_sell = new algo_force_sell();
			pforce_sell->al = this;
			pforce_sell->algo_name = _name;
			pforce_sell->id = _u.get_id();
			pforce_sell->ref = ref;
			pforce_sell->quantity = json["quantity"].get<unsigned long long>();
			return pforce_sell;
		}
		else
		{
			auto msg = new algo_err_msg();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			msg->err = std::string("unkonw command " + cmd);
			return msg;
		}
	}
	catch(const std::exception& e)
	{
		auto msg = new algo_err_msg();
		msg->al = this;
		msg->algo_name = _name;
		msg->id = _u.get_id();
		msg->ref = "unknow";
		msg->err = e.what();
		if (pset)
			delete pset;
		if (pforce_buy)
			delete pforce_buy;
		if (pforce_sell)
			delete pforce_sell;
		return msg;
	}
}

json semi::msg_to_json(algo_msg_base* msg)
{
	return msg->to_json();
}

