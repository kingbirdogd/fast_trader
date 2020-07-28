#include <vector>
#include <bear.hpp>

//std::unordered_map<unsigned int, priceinfo*> bear::uprice_map;

bear::bear(user& u, const std::string& name):
	algo(u, name),
	 _o_map(),
	 _u_map(),
	 _w_map(),
	 buyin_map(),
	 sellout_map(),
	 _p_map()
{
}

void bear::on_omdc_book(const Tradable& tradable)
{
	auto it = _w_map.find(tradable.m_Code);
	if (_w_map.end() != it)
	{
		//for (const auto& p : it->second)
		//{
			//unsigned int code = tradable.m_uCode;
			unsigned long long sellout = it->second->getSellOut();
			unsigned long long buyin = it->second->getBuyIn();

			//auto itp = _p_map.find(it->second->getRef());
			//auto node = &(itp->second);

			//this->log_info(std::string("CBBC CODE = ") + std::to_string(tradable.m_uCode) + " Previous buyin = " + std::to_string(buyin) + " Sellout = " +  std::to_string(sellout));

			if(it->second->action_status() == STAGE_START || it->second->action_status() == STAGE_PAUSE){
/*
				if(sellout != 0 && sellout != 99999999){
					auto aso = sellout_map.find(sellout);
					if(aso != sellout_map.end()){
						if (aso->second.end() != aso->second.find(node))
						{
							aso->second.erase(node);
						}
					}
				}
				if(buyin != 0 && buyin != 99999999){
					auto abi = buyin_map.find(buyin);
					if(abi != buyin_map.end()){
						//buyin_map[buyin]->erase(tradable.m_uCode);
						if (abi->second.end() != abi->second.find(node))
						{
							//auto a_it = abi->second.find(node);
							abi->second.erase(node);
						}
					}
				}
*/
				if(it->second->getWtype() == BULL){
					it->second->on_bull_book(tradable);
				}else{
					it->second->on_bear_book(tradable);
				}

				unsigned long long newsellout = it->second->getSellOut();
				unsigned long long newbuyin = it->second->getBuyIn();
				//unsigned long long bestbid = it->second->getPE()->Bestbid;
				//unsigned long long bestask = it->second->getPE()->Bestask;

				if(newsellout != sellout || newbuyin != buyin){
					sellout = newsellout;
					buyin = newbuyin;

					//it->second->Log(std::string("BEAR CODE = ") + std::to_string(tradable.m_Code) + " Bestask = " + std::to_string(bestask) + " Current buyin = " + std::to_string(buyin) + " Bestbid = " + std::to_string(bestbid) + " Sellout = " +  std::to_string(sellout));
				}
				/*
				if(sellout != 0 && sellout != 99999999 ){
					sellout_map[sellout].insert(node);
				}
				if(buyin != 0 && buyin != 99999999 ){
					buyin_map[buyin].insert(node);
				}
				*/

			}
		//}
	}
}

void bear::on_omdc_trade(const Tradable& )
{
	/*
	auto it = _w_map.find(tradable.m_Code);
	if (_w_map.end() != it)
	{
		auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;
		auto side = tradable.m_TradeSide;

		it->second->Log(std::to_string(tradable.m_Code) + " Side=" + std::to_string(side) + " Price=" + std::to_string(trade_price) + " Qty=" + std::to_string(tradable.m_LastTradeQuantity) + " Buyin=" + to_string(it->second->getBuyIn()) + " Sellout=" + std::to_string(it->second->getSellOut()) );

	}*/
}

void bear::on_omdd_book(const Tradable& tradable)
{
	auto uit = uprice_map.find(tradable.m_Code);
	if(uprice_map.end() != uit){

		//unsigned int code = tradable.m_Code;

		//auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
		auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;

		//auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
		auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;



		if(uit->second->FBestbid != best_bid_price){
			uit->second->PFBestbid = uit->second->FBestbid;
			uit->second->FBestbid = best_bid_price;
		}
		if(uit->second->FBestask != best_ask_price ){
			uit->second->PFBestask = uit->second->FBestask;
			uit->second->FBestask = best_ask_price;
		}

		auto it = _u_map.find(tradable.m_Code);
		if (_u_map.end() != it)
		{
			for (const auto& p : it->second)
			{
				if(p->action_status() == STAGE_START && p->has_position()){
					p->on_omdd_book(tradable);
				}
			}
		}

		uit->second->LFBestbid = best_bid_price;
		uit->second->LFBestask = best_ask_price;
	}

}

void bear::on_omdd_trade(const Tradable& tradable)
{
	auto it = _u_map.find(tradable.m_Code);
	if (_u_map.end() != it)
	{

		priceinfo* uprice = uprice_map[tradable.m_Code];


		auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
		auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
		auto best_bid_price1 = static_cast<unsigned long long>(tradable.m_Bid[1].m_iPrice) * 100000;
		auto best_ask_price1 = static_cast<unsigned long long>(tradable.m_Ask[1].m_iPrice) * 100000;

		//auto trade_quantity = tradable.m_uAccumulatedQuantity;
		auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;



		if(BUY_ORDER == tradable.m_TradeSide){
			auto trade_quantity = static_cast<unsigned long long>(tradable.m_AccumulateSellQuantity);

			if(uprice->TBestbid != trade_price){
				uprice->PTBestbid = uprice->TBestbid;
				uprice->TBestbid = trade_price;
			}


			if (trade_price == best_bid_price && trade_quantity >= tradable.m_Bid[0].m_uQuantity &&	0 != tradable.m_Bid[0].m_uQuantity )
			{
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						if((p->getSellOut() == trade_price) || (best_bid_price > p->getSellOut() && p->getSellOut() < best_bid_price1))
						{
							p->on_bull_trade(tradable);
						}
					}else{
						if(p->getBuyIn() == trade_price)
						{
							p->on_bear_trade(tradable);
						}
					}
/*
					if(p->getSellOut() == trade_price && p->getWtype() == BULL && p->has_position()   )
					{
						p->on_bull_trade(tradable);
					}

					if(p->getBuyIn() == trade_price && p->getWtype() == BEAR && !p->has_position() && (p->status() == STATUS_READY || p->status == STATUS_DONE) )
					{
						p->on_bear_trade(tradable);
					}
*/
				}
			}else{
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						if((p->getSellOut() == trade_price || (best_bid_price > p->getSellOut() && p->getSellOut() < best_bid_price1)) && uprice->PTBestbid > trade_price && p->has_position())
						{
							p->doSellLevel();
						}
					}

				}
			}

		}

		if(SELL_ORDER == tradable.m_TradeSide){
			auto trade_quantity = static_cast<unsigned long long>(tradable.m_AccumulateBuyQuantity);

			if(uprice->TBestask != trade_price ){
				uprice->PTBestask = uprice->TBestask;
				uprice->TBestask = trade_price;
			}

			if (trade_price == best_ask_price && trade_quantity >= tradable.m_Ask[0].m_uQuantity &&	0 != tradable.m_Ask[0].m_uQuantity )
			{
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						if(p->getBuyIn() == trade_price)
						{
							p->on_bull_trade(tradable);
						}
					}else{
						if(p->getSellOut() == trade_price || (best_ask_price < p->getSellOut() && p->getSellOut() < best_ask_price1))
						{
							p->on_bear_trade(tradable);
						}
					}
/*
					if(p->getBuyIn() == trade_price && p->getWtype() == BULL && !p->has_position() && (p->status() == STATUS_READY || p->status == STATUS_DONE) )
					{
						p->on_bull_trade(tradable);
					}

					if(p->getSellOut() == trade_price && p->getWtype() == BEAR && p->has_position() )
					{
						p->on_bear_trade(tradable);
					}
*/
				}
			}else{
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BEAR){
						if((p->getSellOut() == trade_price  || (best_ask_price < p->getSellOut() && p->getSellOut() < best_ask_price1) ) && uprice->PTBestask < trade_price && p->has_position())
						{
							p->doSellLevel();
						}
					}

				}
			}

		}

	}
}


void bear::handler_order(const dbp::top::enhance_order& odr)
{
	auto it = _o_map.find(odr.order_id);
	if(_o_map.end() != it)
	{
		it->second->on_order(odr);
	}
}

std::string bear::set_pair(pair&& p)
{
	auto ref = p.ref();
	auto it = _p_map.find(ref);


	auto itcr = _w_ref_map.find(p.warrant_code());
	if(itcr != _w_ref_map.end()){
		if(p.ref() != itcr->second){
			fprintf(stderr, "Duplication Warrant Code \n");
			return "Duplicate Warrant";
		}else{
			_w_ref_map.erase(p.warrant_code());
			fprintf(stderr, "remove Old entry Warrant Code \n");
		}
	}




	fprintf(stderr, "set_pair 1 %s \n", ref.c_str());

	if (_p_map.end() != it)
	{
		if ((it->second.commodity_symbol() != p.commodity_symbol())
				|| (it->second.warrant_code() != p.warrant_code()))
		{

			fprintf(stderr, "set_pair 1 Code Exist %u \n", p.warrant_code());
			auto symbol = it->second.commodity_symbol();
			auto warrant_code = it->second.warrant_code();
			auto underlying_code = it->second.underlying_code();


			fprintf(stderr, "OLD Pair 1 Symbol = %s   Warrant Code= %u Underlying Code= %u \n", symbol.c_str(), warrant_code, underlying_code);

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
						fprintf(stderr, "set_pair 1 Delete  Exist %u \n", p.warrant_code());
						//subscribe_omdd_trade(underlying_code, false);
						//subscribe_omdd_book(underlying_code, false);
					}
				}
			}
			auto w_it = _w_map.find(warrant_code);
			if (_w_map.end() != w_it)
			{
				_w_map.erase(warrant_code);
				//subscribe_omdc_book(warrant_code, false);
			}
		}
		p.init();
		it->second = std::move(p);
	}
	else
	{
		_p_map.emplace(ref, std::move(p));
		it = _p_map.find(ref);
	}

	fprintf(stderr, "%s \n", "set_pair 2");

	auto symbol = it->second.commodity_symbol();
	auto underlying_code = it->second.underlying_code();
	auto warrant_code = it->second.warrant_code();
	//json["result"] = "SUCCESS";
	//send_out(json.dump());
	auto node = &(it->second);
	_u_map[underlying_code].insert(node);
	_w_map[warrant_code] = node;

	_w_ref_map[warrant_code] = it->second.ref();



	fprintf(stderr, "NEW Pair 1 Symbol = %s   Warrant Code= %u Underlying Code= %u  \n", symbol.c_str(), warrant_code, underlying_code);

	auto itu = uprice_map.find(underlying_code);
	if(itu  == uprice_map.end()){
		uprice_map[underlying_code] = std::move(new priceinfo());
	}

	fprintf(stderr, "%s \n", "set_pair 3");

	//bool result = subscribe_omdc_book(warrant_code, true);
	//if(!result){
		//json["result"] = "FAIL";
		//json["reason"] = "Invalid code";
		//send_out(json.dump());
		//return;
		//return "Invalid Code";
	//}else{
		//json["recovery"] = true;
		//json["result"] = "SUCCESS";
		//send_out(json.dump());
	//}
	//subscribe_omdd_book(underlying_code, true);
	//subscribe_omdd_trade(underlying_code, true);
	return "SUCCESS";
}

bear::action_resp bear::set_start(unsigned int code, const std::string& ref){

	fprintf(stderr, "%s \n", "do set_start");

	action_resp a_resp;
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		a_resp.result = "FAIL";
		a_resp.reason = "fail pair not found";
		return a_resp;
	}
	auto& p = it->second;
	if(p.warrant_code() != code){
		a_resp.result = "FAIL";
		a_resp.reason = "fail invalid code";
		return a_resp;
	}

	a_resp.previous_action = p.action_status();
	p.set_ActionStatus(STAGE_START);
	a_resp.action = p.action_status();

	a_resp.result = "SUCCESS";
	a_resp.reason = "";
	a_resp.recovery = true;

	return a_resp;
}

bear::action_resp bear::set_pause(unsigned int code, const std::string& ref){

	fprintf(stderr, "%s \n", "do set_pause");
	action_resp a_resp;
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		a_resp.result = "FAIL";
		a_resp.reason = "fail pair not found";
		return a_resp;
	}
	auto& p = it->second;
	if(p.warrant_code() != code){
		a_resp.result = "FAIL";
		a_resp.reason = "fail invalid code";
		return a_resp;
	}

	a_resp.previous_action = p.action_status();
	p.set_ActionStatus(STAGE_PAUSE);
	a_resp.action = p.action_status();

	a_resp.result = "SUCCESS";
	a_resp.reason = "";
	a_resp.recovery = true;

	return a_resp;
}

bear::action_resp bear::set_stop(unsigned int code, const std::string& ref){

	fprintf(stderr, "%s \n", "do set_stop");

	action_resp a_resp;
	auto it = _p_map.find(ref);

	if (_p_map.end() == it)
	{
		a_resp.result = "FAIL";
		a_resp.reason = "fail pair not found";
		a_resp.recovery = false;
		return a_resp;
	}
	auto& p = it->second;
	if(p.warrant_code() != code){
		a_resp.result = "FAIL";
		a_resp.reason = "fail invalid code";
		a_resp.recovery = false;
		return a_resp;
	}
	if(p.has_position()){
		a_resp.result = "FAIL";
		a_resp.reason = "Position Exist";
		a_resp.recovery = false;
		return a_resp;
	}

	a_resp.previous_action = p.action_status();
	p.set_ActionStatus(STAGE_STOP);
	a_resp.action = p.action_status();

	a_resp.result = "SUCCESS";
	a_resp.reason = "";
	a_resp.recovery = true;


/*
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
		_w_map.erase(warrant_code);
	}
*/
	return a_resp;
}

std::string bear::load_pricetable(unsigned int code, const std::string& ref){

	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;

	if(p.warrant_code() != code){
		return "fail invalid code";
	}

	std::map<unsigned long long, unsigned long long> bidTable = p.getBidTable();

	std::map<unsigned long long, unsigned long long>::iterator it2;
	for ( it2 = bidTable.begin(); it2 != bidTable.end(); it2++ )
	{
		unsigned long long bkey = it2->first;
		unsigned long long bprice = it2->second;

		if(bkey > 0 && bprice>0){

			auto msg = algo_pricetable_msg_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			msg->warrant_code = p.warrant_code();
			msg->side = "BID";
			msg->wkey = bkey;
			msg->fprice = bprice;
			ouputQueue.enqueue(msg);
		}
	}

	std::map<unsigned long long, unsigned long long> askTable = p.getAskTable();
	for ( it2 = askTable.begin(); it2 != askTable.end(); it2++ )
	{
		unsigned long long bkey = it2->first;
		unsigned long long bprice = it2->second;

		if(bkey > 0 && bprice>0){
			auto msg = algo_pricetable_msg_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			msg->warrant_code = p.warrant_code();
			msg->side = "ASK";
			msg->wkey = bkey;
			msg->fprice = bprice;
			ouputQueue.enqueue(msg);
		}
	}

	return "SUCCESS";
}

std::string bear::set_param(unsigned int code, const std::string& type, const std::string& value, const std::string& ref){

	fprintf(stderr, "%s \n", "do set_param");

	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;

	if(p.warrant_code() != code){
		return "fail invalid code";
	}

	unsigned long long ivalue = std::stoull(value);

	//int ivalue = atoi(value.c_str());

	if(type == "INOUT"){
		int _ivalue = static_cast<int>(ivalue);
		p.set_InOut(_ivalue);
	}else if(type == "WINTICK"){
		int _ivalue = static_cast<int>(ivalue);
		p.set_WinTick(_ivalue);
	}else if(type == "QTY"){
		p.set_BuyQuantity(ivalue);
	}else if(type == "ISIZE"){
		p.set_IssuerSize(ivalue);
	}else if(type == "INLVL"){
		int _ivalue = static_cast<int>(ivalue);
		p.set_LvlRange(_ivalue);
	}else if(type == "STOPLOST"){
		int _ivalue = static_cast<int>(ivalue);
		p.set_StopLost(_ivalue);
	}else if(type == "PTRANGE"){
		int _ivalue = static_cast<int>(ivalue);
		p.set_PtRange(_ivalue);
	}else{
		return "INVALID TYPE";
	}
	return "SUCCESS";
}

std::string bear::force_buy(unsigned long long price, unsigned long long quantity, const std::string& ref)
{
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	//auto underlying_code = p.underlying_code();
	//auto warrant_code = p.warrant_code();


	warrant* newWarrant = new warrant;
	newWarrant->Date = DateUtil::getToday();
	newWarrant->Code = p.warrant_code();
	newWarrant->Status = STATUS_READY;
	newWarrant->BuyPrice = price;
	newWarrant->UCode = p.underlying_code();
	newWarrant->BuyQty = quantity;
	newWarrant->Quantity = 0;
	newWarrant->Name = std::to_string(p.warrant_code());
	newWarrant->BuyIn = 0;
	newWarrant->SellOut = 0;

	//this->log_info(to_string(p.warrant_code()) + std::string("Status = ") + to_string(p.status()));

	if(p.status() != STATUS_READY && p.status() != STATUS_AVAILABLE){
		return "Invalid order status";
	}

	OBSetting* obs = p.getOBS();
	if(!obs->isExist(p.warrant_code())){
		obs->addWarrantOrCbbc(newWarrant);
	}

	if (p.doBuy(newWarrant))
	{
		return "SUCCESS";
	}

	return "fail new buy order not valid";
}

std::string bear::force_sell(unsigned long long price, unsigned long long quantity, const std::string& ref)
{
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	//auto underlying_code = p.underlying_code();
	//auto warrant_code = p.warrant_code();

	warrant* newWarrant = new warrant;
	newWarrant->Date = DateUtil::getToday();
	newWarrant->Code = p.warrant_code();
	newWarrant->Status = STATUS_READY;
	newWarrant->UCode = p.underlying_code();
	newWarrant->SellPrice = price;
	newWarrant->SellQty = quantity;
	newWarrant->Name = std::to_string(p.warrant_code());


	if(p.status() != STATUS_AVAILABLE){
		return "Invalid order status";
	}

	if(p.has_position()){
		p.Log(to_string(p.warrant_code()) + std::string("Position = ") + to_string(p.getPosition()));
		if(p.getPosition()<quantity){
			return "Insufficient Position";
		}
		//auto result = p.sell(0, false, quantity);
		bool result = p.doSell(newWarrant);
		if(!result){
			return "Order Error";;
		}
	}else{
		return "No Position";
	}
	return "SUCCESS";
}

std::string bear::set_position(unsigned long long price, unsigned long long quantity, const std::string& ref)
{
	auto it = _p_map.find(ref);
	if (_p_map.end() == it)
	{
		return "fail pair not found";
	}
	auto& p = it->second;
	//auto underlying_code = p.underlying_code();
	//auto warrant_code = p.warrant_code();

	if(p.action_status() == STAGE_START){
		return "Invalid status";
	}

	if(p.action_status() == STAGE_PAUSE && quantity == 0){

	}else if(p.action_status() == STAGE_STOP && quantity > 0){

	}else{
		return "Invalid status";
	}


	if(!p.has_position()){
		bool result = p.set_Position(price, quantity);
		if(!result){
			return "warrant exist";
		}
	}else{
		if(quantity == 0){
			bool result = p.set_Position(price, quantity);
			if(!result){
				return "reset quantity not zero";
			}else{
				return "SUCCESS";
			}
		}

		return "reset quantity not zero";
	}

	return "SUCCESS";
}

void bear::handle_command(algo_msg_base& msg)
{
	msg.on_command();
}

algo_msg_base* bear::json_to_msg(json& json)
{
	algo_set_msg* pset = nullptr;
	algo_action_msg* pAction_msg = nullptr;
	algo_force_buy* pforce_buy = nullptr;
	algo_force_sell* pforce_sell = nullptr;
	algo_setposition* palgo_setposition = nullptr;
	algo_loadpricetable* ploadpricetable = nullptr;
	algo_param_msg* palgo_param_msg = nullptr;
	try
	{

		//fprintf(stdout, "bear %s\n", json.dump().c_str());

		auto cmd = json["cmd"].get<std::string>();
		auto ref = json["ref"].get<std::string>();
		if (cmd == "set"){
			pset = algo_set_msg_pool.get_obj();
			auto& p = pset->p;
			pset->al = this;
			pset->algo_name = _name;
			pset->id = _u.get_id();
			pset->ref = ref;

			auto wtype = json["wtype"].get<std::string>();
			if (wtype != "bull" && wtype != "bear")
			{
				auto msg = algo_err_msg_pool.get_obj();
				msg->al = this;
				msg->algo_name = _name;
				msg->id = _u.get_id();
				msg->ref = ref;
				msg->action = "cmd set";
				msg->reason = "set only support bull bear";
				msg->result = "FAIL";
				//delete pset;
				pset->release();
				return msg;
			}
			p._algo = this;
			p._Ref = ref;
			p._Wtype = 1;
			if(wtype == "bear"){
				p._Wtype = 2;
			}

			fprintf(stdout, "bear 1\n");

			//p._Symbol = json["symbol"].get<std::string>();
			p._IssuerSize = json["issuersize"].get<unsigned long long>();
			p._Quantity = json["quantity"].get<unsigned long long>();
			p._SPREAD = json["spread"].get<unsigned long long>();
			p._warrant_code = json["warrant_code"].get<unsigned int>();

			auto it = warrantToUnderlying.find(p._warrant_code);
			if (warrantToUnderlying.end() == it){
				auto msg = algo_err_msg_pool.get_obj();
				msg->al = this;
				msg->algo_name = _name;
				msg->id = _u.get_id();
				msg->ref = ref;
				msg->action = "cmd set";
				msg->reason = "Invalid warrant Code";
				msg->result = "FAIL";
				pset->release();
				return msg;
			}


			p._Underlying_code = 0;

			fprintf(stdout, "bear 2\n");

			std::string str_underlying = json["symbol"].get<std::string>();
			p._Symbol = str_underlying;
			auto it_omdd = nameToCode.find(str_underlying);
			if (nameToCode.end() == it_omdd)
			{
				auto msg = algo_err_msg_pool.get_obj();
				msg->al = this;
				msg->algo_name = _name;
				msg->id = _u.get_id();
				msg->ref = ref;
				msg->action = "cmd set";
				msg->result = "FAIL";
				msg->reason = "fail command set underlying code omdd mapping not found";
				//delete pset.release();
				pset->release();
				return msg;
			}
			p._Underlying_code = it_omdd->second;

			fprintf(stdout, "bear 3\n");
			p._OBSetting = new OBSetting();
			p._PriceInfo = new priceinfo();
			p._PriceInfoU = new priceinfo();
			p._CbbcPriceMark = new CbbcPriceMark(p._Wtype, p._SPREAD, 100000);


			fprintf(stdout, "bear 4\n");

			return pset;
		}
		else if(cmd == "pause" || cmd == "stop" || cmd == "start")
		{
			pAction_msg = algo_action_msg_pool.get_obj();
			pAction_msg->al = this;
			pAction_msg->algo_name = _name;
			pAction_msg->id = _u.get_id();
			pAction_msg->ref = ref;
			pAction_msg->action = cmd;
			pAction_msg->code = json["warrant_code"].get<unsigned int>();
			return pAction_msg;
		}
		else if(cmd == "loadpricetable")
		{
			ploadpricetable = algo_loadpricetable_pool.get_obj();
			ploadpricetable->al = this;
			ploadpricetable->algo_name = _name;
			ploadpricetable->id = _u.get_id();
			ploadpricetable->ref = ref;
			ploadpricetable->code = json["warrant_code"].get<unsigned int>();
			return ploadpricetable;
		}
		else if(cmd == "param")
		{
			palgo_param_msg = algo_param_msg_pool.get_obj();
			palgo_param_msg->al = this;
			palgo_param_msg->algo_name = _name;
			palgo_param_msg->id = _u.get_id();
			palgo_param_msg->ref = ref;
			palgo_param_msg->code = json["warrant_code"].get<unsigned int>();
			palgo_param_msg->type = json["type"].get<std::string>();
			palgo_param_msg->value = json["value"].get<std::string>();
			return palgo_param_msg;
		}
		else if(cmd == "force_buy")
		{
			pforce_buy = algo_force_buy_pool.get_obj();
			pforce_buy->al = this;
			pforce_buy->algo_name = _name;
			pforce_buy->id = _u.get_id();
			pforce_buy->ref = ref;
			pforce_buy->code = json["warrant_code"].get<unsigned int>();
			pforce_buy->price = json["price"].get<unsigned long long>();
			pforce_buy->quantity = json["quantity"].get<unsigned long long>();
			return pforce_buy;
		}
		else if(cmd == "force_sell")
		{
			pforce_sell = algo_force_sell_pool.get_obj();
			pforce_sell->al = this;
			pforce_sell->algo_name = _name;
			pforce_sell->id = _u.get_id();
			pforce_sell->ref = ref;
			pforce_sell->code = json["warrant_code"].get<unsigned int>();
			pforce_sell->price = json["price"].get<unsigned long long>();
			pforce_sell->quantity = json["quantity"].get<unsigned long long>();
			return pforce_sell;
		}
		else if(cmd == "set_position")
		{
			palgo_setposition = algo_setposition_pool.get_obj();
			palgo_setposition->al = this;
			palgo_setposition->algo_name = _name;
			palgo_setposition->id = _u.get_id();
			palgo_setposition->ref = ref;
			palgo_setposition->code = json["warrant_code"].get<unsigned int>();
			palgo_setposition->price = json["price"].get<unsigned long long>();
			palgo_setposition->quantity = json["quantity"].get<unsigned long long>();
			return palgo_setposition;
		}
		else
		{
			auto msg = algo_err_msg_pool.get_obj();
			msg->al = this;
			msg->algo_name = _name;
			msg->id = _u.get_id();
			msg->ref = ref;
			msg->action = "json_to_msg";
			msg->result = "FAIL";
			msg->reason = std::string("invalid command");
			return msg;
		}
	}
	catch(const std::exception& e)
	{
		auto msg = algo_err_msg_pool.get_obj();
		msg->al = this;
		msg->algo_name = _name;
		msg->id = _u.get_id();
		msg->ref = "unknown";
		msg->action = "json_to_msg";
		msg->result = "FAIL";
		msg->reason = std::string("Exception");

		if (pset)
			pset->release();
		if (pforce_buy)
			pforce_buy->release();
		if (pforce_sell)
			pforce_sell->release();
		if(pAction_msg)
			pAction_msg->release();
		if(palgo_setposition)
			palgo_setposition->release();
		if(ploadpricetable)
			ploadpricetable->release();
		if(palgo_param_msg)
			palgo_param_msg->release();
		return msg;
	}
}

std::string bear::get_lib_name()
{
	return "bear";
}

rapid_ring::spmc_ring_buffer_object_pool<bear::algo_err_msg, 8192> bear::algo_err_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<bear::algo_param_msg, 8192> bear::algo_param_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<bear::algo_positionorder_msg, 8192> bear::algo_positionorder_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<bear::algo_order_msg, 8192> bear::algo_order_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<bear::algo_loadpricetable, 8192> bear::algo_loadpricetable_pool;
rapid_ring::spmc_ring_buffer_object_pool<bear::algo_validate_msg, 8192> bear::algo_validate_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<bear::algo_portfolio_msg, 8192> bear::algo_portfolio_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<bear::algo_warrantprice_msg, 8192> bear::algo_warrantprice_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<bear::algo_pricetable_msg, 8192> bear::algo_pricetable_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<bear::algo_setposition, 8192> bear::algo_setposition_pool;
rapid_ring::spsc_ring_buffer_object_pool<bear::algo_set_msg, 8192> bear::algo_set_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<bear::algo_action_msg, 8192> bear::algo_action_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<bear::algo_force_buy, 8192> bear::algo_force_buy_pool;
rapid_ring::spsc_ring_buffer_object_pool<bear::algo_force_sell, 8192> bear::algo_force_sell_pool;

