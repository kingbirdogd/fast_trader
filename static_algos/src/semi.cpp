#include <semi.hpp>

#include <global_memory.hpp>


semi::semi(user& u, const std::string& name):
	algo(u, name),
	_o_map(),
	_u_map(),
	_w_map(),
	_p_map()
{
	logger = new ThreadLogger("log/" + name + DateUtil::getToday() + ".log");
	logger->start();


	Log("Start Semi " + name);
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
	/*
	else{
		auto it = _u_map.find(tradable.m_Code);
		if (_u_map.end() != it)
		{
			auto bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
			auto ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
			auto best_bid_vol = static_cast<unsigned long long>(tradable.m_Bid[0].m_uQuantity);
			auto best_ask_vol = static_cast<unsigned long long>(tradable.m_Ask[0].m_uQuantity);

			auto bull_buyratio = static_cast<unsigned long long>((best_ask_vol*100) / (best_bid_vol + best_ask_vol));
			auto bull_sellratio = static_cast<unsigned long long>((best_bid_vol*100) / (best_bid_vol + best_ask_vol));
			auto bear_buyratio = static_cast<unsigned long long>((best_bid_vol*100) / (best_bid_vol + best_ask_vol));
			auto bear_sellratio = static_cast<unsigned long long>((best_ask_vol*100) / (best_bid_vol + best_ask_vol));

			//fprintf(stderr, "info Code: %u : %llu - Bid:%llu | Ask:%llu - %llu \n", tradable.m_Code, best_bid_vol, bid_price, ask_price, best_ask_vol);
			//fprintf(stderr, "info Code: %u : Bull Ratio Buy : %llu  Ratio Sell: %llu \n", tradable.m_Code,bull_buyratio, bull_sellratio);
			//fprintf(stderr, "info Code: %u : Bear Ratio_buy : %llu  Ratio_sell: %llu \n",tradable.m_Code, bear_buyratio, bear_sellratio);



			for (const auto& p : it->second)
			{
				//fprintf(stderr, "info Code: %u : User Ratio_buy : %llu  Ratio_sell: %llu   User Trigger_buy : %llu  User Trigger_sell : %llu \n",tradable.m_Code, p->ratio_buy(), p->ratio_sell(), p->buy_trriger(),p->sell_trriger()  );

				if(!p->is_buying() && !p->is_selling()){
					if ( p->auto_buy() && p->ratio_buy()>0 )
					{
						if(p->is_bull()){
							if(bull_buyratio <= p->ratio_buy()  && p->buy_trriger() == ask_price){
								p->buy(p->buy_price(), false, 0);
							}
						}else{
							if(bear_buyratio <= p->ratio_buy() && p->buy_trriger() == bid_price){
								p->buy(p->buy_price(), false, 0);
							}
						}
					}

					if(p->auto_sell() && p->ratio_sell()>0  )
					{
						if(p->is_bull()){
							if(bull_sellratio <= p->ratio_sell() && p->sell_trriger() == bid_price){
								p->sell(p->sell_price(), false, 0);
							}
						}else{
							if(bear_sellratio <= p->ratio_sell() && p->sell_trriger() == ask_price){
								p->sell(p->sell_price(), false, 0);
							}
						}
					}
				}

			}
		}
	}*/
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
			if(!p.is_reset_position()){
				p.set_position(it->second.position());
			}
		}

		p.set_auto_buy_id(it->second.auto_buy_id());
		p.set_auto_sell_id(it->second.auto_sell_id());


		if((it->second.is_buying() || it->second.is_selling()) && !no_change){
			return "can't change action while Buying / Selling";
		}


		p.set_is_buying(it->second.is_buying());
		p.set_is_selling(it->second.is_selling());
		p.set_last_trigger_price(it->second.last_trigger_price());
		p.set_last_price(it->second.last_price());

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


std::string semi::force_buy(unsigned long long price, unsigned long long quantity, pair*& pref, const std::string& ref)
{
	pref = nullptr;
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	pref = &p;
	auto result = p.buy(price, false, quantity);
	if (pair::buy_result::SUCCESS == result)
	{
		return  "SUCCESS";
	}
	else if(pair::buy_result::BUYING == result)
	{
		return "fail new buy order not valid";
	}else if(pair::buy_result::NOT_READY == result)
	{
		return "Not Ready";
	}else if(pair::buy_result::EXCEED_BUY_POWER == result)
	{
		return "Exceed Buy Power";
	}

	return "Unknown";

}

std::string semi::force_sell(unsigned long long price, unsigned long long quantity, pair*& pref, const std::string& ref)
{
	pref = nullptr;
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	pref = &p;
	auto result = p.sell(price, false, quantity);
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

std::string semi::limit_sell(unsigned long long price, unsigned long long quantity, pair*& pref, const std::string& ref)
{
	pref = nullptr;
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	pref = &p;
	auto result = p.sell_limit(price, false, quantity);
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

void semi::Log(string msg){
	logger->Log(string(DateUtil::getCurrentTime()) + " " + msg);
}

algo_msg_base* semi::json_to_msg(json& json)
{
	algo_set* pset = nullptr;
	algo_force_buy* pforce_buy = nullptr;
	algo_force_sell* pforce_sell = nullptr;
	algo_limit_sell* plimit_sell = nullptr;
	try
	{

//		fprintf(stdout, "semi %s\n", json.dump().c_str());

		auto cmd = json["cmd"].get<std::string>();
		auto ref = json["ref"].get<std::string>();
		if (cmd == "position")
		{
			auto msg = algo_odr_position_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			return msg;
		}
		else if (cmd == "set")
		{
			pset = algo_set_pool.get_obj();
			auto& p = pset->p;
			pset->al = this;
			pset->algo_name = _name;
			pset->id = _u.get_id();
			pset->ref = ref;
			auto type = json["type"].get<std::string>();
			if (type != "bull" && type != "bear")
			{
				auto msg = algo_err_msg_pool.get_obj();
				msg->al = this;
				msg->algo_name = _name;
				msg->id = _u.get_id();
				msg->ref = ref;
				msg->err = "set only support bull bear";
				msg->action = "set";
				pset->release();
				return msg;
			}
			p._algo = this;
			p._ref = ref;
			p._is_bull = true;
			if (type == "bear")
			{
				p._is_bull = false;
			}
			p._is_reset_position = false;
			auto it_reset_position = json.find("resetposition");
			if (json.end() != it_reset_position)
			{
				p._is_reset_position = json["resetposition"].get<bool>();
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
					auto msg = algo_err_msg_pool.get_obj();
					msg->al = this;
					msg->algo_name = _name;
					msg->id = _u.get_id();
					msg->ref = ref;
					msg->err = "fail command set underlying code omdd mapping not found";
					msg->action = "set";
					pset->release();
					return msg;
				}
				p._underlying_code = it_omdd->second;
				p._is_omdd = true;
			}
			if (stockWarrantomdcMap.end() == stockWarrantomdcMap.find(p._warrant_code))
			{
				auto msg = algo_err_msg_pool.get_obj();
				msg->al = this;
				msg->algo_name = _name;
				msg->id = _u.get_id();
				msg->ref = ref;
				msg->err = std::string("Warrant code ") + std::to_string(p._warrant_code) + " not found";
				msg->action = "set";
				pset->release();
				return msg;
			}
			if (p._is_omdd)
			{
				if (omddMap.end() == omddMap.find(p._underlying_code))
				{
					auto msg = algo_err_msg_pool.get_obj();
					msg->al = this;
					msg->algo_name = _name;
					msg->id = _u.get_id();
					msg->ref = ref;
					if (p._is_omdd){
					msg->err = std::string("Underlying symbol ") + p._underlying_symbol + " not found";
					msg->action = "set";
					}
					pset->release();
					return msg;
				}
			}
			else
			{
				if (stockWarrantomdcMap.end() == stockWarrantomdcMap.find(p._underlying_code))
				{
					auto msg = algo_err_msg_pool.get_obj();
					msg->al = this;
					msg->algo_name = _name;
					msg->id = _u.get_id();
					msg->ref = ref;
					if (p._is_omdd){
					msg->err = std::string("Underlying code ") + std::to_string(p._underlying_code) + " not found";
					msg->action = "set";
					}
					pset->release();
					return msg;
				}
			}
			p._buy_trriger = json["buy_trriger"].get<unsigned long long>();
			p._sell_trriger = json["sell_trriger"].get<unsigned long long>();
			p._buy_price = json["buy_price"].get<unsigned long long>();
			p._sell_price = json["sell_price"].get<unsigned long long>();
			p._bottom_price = json["bottom_price"].get<unsigned long long>();
			p._ceiling_price = json["ceiling_price"].get<unsigned long long>();
			p._auto_buy_quantity = json["auto_buy_quantity"].get<unsigned long long>();

			//Log("Set Code = " + to_string(p._warrant_code) + " Buy Quantity = " + to_string(p._auto_buy_quantity));

			auto itwdef = omdcAdditionDefinitionsMap.find(p._warrant_code);
			if(omdcAdditionDefinitionsMap.end() != itwdef){
				COmdcAdditionDefinitions def = itwdef->second;



				unsigned long long lotsize = static_cast<unsigned long long>(def.LotSize) * 100000;

				//Log("Set Code = " + to_string(p._warrant_code) + " Def Lotsize = " + to_string(lotsize));

				if((p._auto_buy_quantity % lotsize) > 0){
					auto msg = algo_err_msg_pool.get_obj();
						msg->al = this;
						msg->algo_name = _name;
						msg->id = _u.get_id();
						msg->ref = ref;
						msg->err = std::string("Warrant code ") + std::to_string(p._warrant_code) + " Invalid Lotsize";
						msg->action = "set";
						pset->release();
						return msg;
				}
			}

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
				auto msg = algo_err_msg_pool.get_obj();
				msg->al = this;
				msg->algo_name = _name;
				msg->id = _u.get_id();
				msg->ref = ref;
				msg->err = std::string("fail command set, action:") + action = " is not support";
				msg->action = "set";
				pset->release();
				return msg;
			}
			p._early_buy_qty = json["early_buy_qty"].get<unsigned long long>();
			p._early_sell_qty = json["early_sell_qty"].get<unsigned long long>();
			p._ratio_buy = json["ratio_buy"].get<unsigned long long>();
			p._ratio_sell = json["ratio_sell"].get<unsigned long long>();
			auto it_position = json.find("position");
			if (json.end() != it_position)
			{
				if (it_position.value().type() == json::value_t::number_unsigned)
				{
					p._position = it_position.value().get<unsigned long long>();
				}
				else
				{
					p._position = 0;
				}
			}

			Log(json.dump().c_str());

			return pset;
		}
		else if (cmd == "delete")
		{
			auto msg = algo_del_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			return msg;
		}
		else if (cmd == "get")
		{
			auto msg = algo_get_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			return msg;
		}
		else if (cmd == "getprofit")
		{
			auto msg = algo_getprofit_msg_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			return msg;
		}
		else if (cmd == "force_buy")
		{
			pforce_buy = algo_force_buy_pool.get_obj();
			pforce_buy->al = this;
			pforce_buy->algo_name = _name;
			pforce_buy->id = _u.get_id();
			pforce_buy->ref = ref;
			pforce_buy->price = json["price"].get<unsigned long long>();
			pforce_buy->quantity = json["quantity"].get<unsigned long long>();
			return pforce_buy;
		}
		else if (cmd == "force_sell")
		{
			pforce_sell = algo_force_sell_pool.get_obj();
			pforce_sell->al = this;
			pforce_sell->algo_name = _name;
			pforce_sell->id = _u.get_id();
			pforce_sell->ref = ref;
			pforce_sell->price = json["price"].get<unsigned long long>();
			pforce_sell->quantity = json["quantity"].get<unsigned long long>();
			return pforce_sell;
		}
		else if (cmd == "limit_sell")
		{
			plimit_sell = algo_limit_sell_pool.get_obj();
			plimit_sell->al = this;
			plimit_sell->algo_name = _name;
			plimit_sell->id = _u.get_id();
			plimit_sell->ref = ref;
			plimit_sell->price = json["price"].get<unsigned long long>();
			plimit_sell->quantity = json["quantity"].get<unsigned long long>();
			return plimit_sell;
		}
		else
		{
			auto msg = algo_err_msg_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			msg->err = std::string("unkonw command " + cmd);
			msg->action = "cmd";
			return msg;
		}
	}
	catch(const std::exception& e)
	{
		auto msg = algo_err_msg_pool.get_obj();
		msg->al = this;
		msg->algo_name = _name;
		msg->id = _u.get_id();
		msg->ref = "unknow";
		msg->action = "exception";
		msg->err = e.what();
		if (pset)
			pset->release();
		if (pforce_buy)
			pforce_buy->release();
		if (pforce_sell)
			pforce_sell->release();
		if(plimit_sell)
			plimit_sell->release();
		return msg;
	}
}

std::string semi::get_lib_name()
{
	return "semi";
}

void semi::on_tcp_book(const Tradable&)
{
}

void semi::on_tcp_trade(const Tradable&)
{
}

rapid_ring::spmc_ring_buffer_object_pool<semi::algo_odr_msg, 8192> semi::algo_odr_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<semi::algo_err_msg, 8192> semi::algo_err_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<semi::algo_odr_position, 8192> semi::algo_odr_position_pool;
#ifndef NOT_MEASURE
rapid_ring::spmc_ring_buffer_object_pool<semi::algo_latency, 8192> semi::algo_latency_pool;
#endif
rapid_ring::spsc_ring_buffer_object_pool<semi::algo_set, 8192> semi::algo_set_pool;
rapid_ring::spsc_ring_buffer_object_pool<semi::algo_del, 8192> semi::algo_del_pool;
rapid_ring::spsc_ring_buffer_object_pool<semi::algo_get, 8192> semi::algo_get_pool;
rapid_ring::spsc_ring_buffer_object_pool<semi::algo_force_buy, 8192> semi::algo_force_buy_pool;
rapid_ring::spsc_ring_buffer_object_pool<semi::algo_force_sell, 8192> semi::algo_force_sell_pool;
rapid_ring::spsc_ring_buffer_object_pool<semi::algo_limit_sell, 8192> semi::algo_limit_sell_pool;
rapid_ring::spsc_ring_buffer_object_pool<semi::algo_getprofit_msg, 8192> semi::algo_getprofit_msg_pool;

