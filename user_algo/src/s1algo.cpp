#include <vector>
#include <s1algo.hpp>

//std::unordered_map<unsigned int, priceinfo*> bear::uprice_map;

s1algo::s1algo(user& u, const std::string& name):
	algo(u, name)
{
	selectedIssuer.insert("MB");

}

void s1algo::on_omdc_book(const Tradable& tradable)
{

	auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
	auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
	auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
	//auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;



	auto itob = obMap.find(tradable.m_Code);
	if(itob != obMap.end())
	{
		//Log("on_omdc_book code = " + to_string(tradable.m_Code) + " OBSetting");
		OBSetting* obs = obMap[tradable.m_Code];
		if(obs->hasPosition)
		{
			unsigned long long spread = spreadTable.getSpread(obs->SpreadTableCode, best_bid_price-1);
			unsigned long long diffu = best_bid_price - obs->StopLostPrice;
			unsigned long long diffw = best_bid_price - obs->getHighestStopLostPrice();

			int countspreadu = static_cast<int>(diffu/spread);
			int countspreadw = static_cast<int>(diffw/spread);

			s1signal* s1 = s1SignalMap[tradable.m_Code];
			if( ((countspreadw > 0) || (countspreadu > 0) || (best_bid_price > obs->StopLostPrice && best_bid_qty > s1->RaiseStopLost)) && ((obs->Status == STATUS_AVAILABLE))){
				unsigned long long oldstoplost = obs->StopLostPrice;
				if(countspreadu > 0){
					if(best_bid_qty > s1->RaiseStopLost){
						obs->StopLostPrice = best_bid_price;
					}
				}

				if(obs->StopLostPrice > oldstoplost){
					Log("bid->Quantity0 = " + to_string(best_bid_qty) + " as->RaiseStopLost = " + to_string(s1->RaiseStopLost));
					Log("Security Code = " + to_string(tradable.m_Code) + " Rise Stop Lost Price from " + to_string(oldstoplost) + " To " + to_string(obs->StopLostPrice));
				}

				if(countspreadw > 0){
					vector<warrant*> obsw = obs->getRelatedWarrant();
					for(unsigned int i=0; i<obsw.size(); i++){

						PriceMark* spm = pricemarkMap[obsw[i]->Code];

						auto it = omdcMap.find(obsw[i]->Code);
						if(it != omdcMap.end()){
							auto wbest_bid_price = static_cast<unsigned long long>(it->second.m_Bid[0].m_iPrice) * 100000;
							//auto wbest_ask_price = static_cast<unsigned long long>(it->second.m_Ask[0].m_iPrice) * 100000;

							unsigned long long fpcb = spm->sellOut(wbest_bid_price);
							if(fpcb > obsw[i]->StopLostPrice  && fpcb <= obs->StopLostPrice && fpcb <= best_bid_price){
								obsw[i]->StopLostPrice = fpcb;
							}else if(fpcb > obsw[i]->StopLostPrice && fpcb <= obs->StopLostPrice && fpcb > best_bid_price){
								obsw[i]->StopLostPrice = best_bid_price;
							}
						}

					}
				}
			}
		}
	}



	auto it = s1SignalMap.find(tradable.m_Code);
	if(it != s1SignalMap.end()){

		auto itob = obMap.find(tradable.m_Code);
		if(itob == obMap.end()){
			obMap[tradable.m_Code] = new OBSetting();

			obMap[tradable.m_Code]->detected = false;
			obMap[tradable.m_Code]->SpreadTableCode = "";
			obMap[tradable.m_Code]->hasPosition = false;
			Log("Init = " + to_string(tradable.m_Code) + " OBSetting");
		}


		OBSetting* obs = obMap[tradable.m_Code];
		if(obs == nullptr)
			return;
		if(it->second->hasSignal){
			if(obs->detected){
				if(best_ask_price != obs->DetectedAsk){
					obs->Status = STATUS_NEW;
					obs->detected = false;
					Log("Code = " + to_string(tradable.m_Code) + " Reset Signal");

					auto pmsg = algo_signal_msg_pool.get_obj();
					pmsg->al = this;
					pmsg->algo_name = this->_name;
					pmsg->id = this->_u.get_id();
					pmsg->ref = to_string(tradable.m_Code);
					pmsg->code = tradable.m_Code;
					pmsg->detect_ask = 0;
					pmsg->selected = false;
					ouputQueue.enqueue(pmsg);
				}
			}
			else
			{
				if(best_ask_price == it->second->DetectAsk){
					obs->DetectedAsk = best_ask_price;

					if(obs->SpreadTableCode == ""){
						COmdcAdditionDefinitions itdef = omdcAdditionDefinitionsMap[tradable.m_Code];
						obs->SpreadTableCode = itdef.SpreadTableCode;
					}

					vector<warrant*> selectedWarrant = getSelectedWarrantFromMarketByIssuer("MB",tradable.m_Code, best_bid_price,best_ask_price );
					if(selectedWarrant.size() == 0)
						return;

					auto pmsg = algo_signal_msg_pool.get_obj();
					pmsg->al = this;
					pmsg->algo_name = this->_name;
					pmsg->id = this->_u.get_id();
					pmsg->ref = to_string(code);
					pmsg->code = code;
					pmsg->detect_ask = obs->DetectedAsk;
					pmsg->selected = true;


					for(unsigned int i=0; i<selectedWarrant.size(); i++){
						warrant* w = selectedWarrant[i];

						Log("Selected Code = " + to_string(w->Code));

						w->Status = STATUS_READY;
						obs->addWarrantOrCbbc(w);

						detectedlist.insert(w);
					}


					ouputQueue.enqueue(pmsg);


					obs->Status = STATUS_READY;
					obs->detected = true;

					Log("Code = " + to_string(tradable.m_Code) + " Has Signal @ " + to_string(best_ask_price));
				}
			}
		}
		else
		{
			if(obs->detected ){

				auto pmsg = algo_signal_msg_pool.get_obj();
				pmsg->al = this;
				pmsg->algo_name = this->_name;
				pmsg->id = this->_u.get_id();
				pmsg->ref = to_string(code);
				pmsg->code = code;
				pmsg->detect_ask = 0;
				pmsg->selected = false;
				ouputQueue.enqueue(pmsg);

				obs->removeAllWarrants();
				obs->Status = STATUS_NEW;
				obs->detected = false;

				Log("Code = " + to_string(tradable.m_Code) + " Reset Signal");
			}
		}
	}

}

bool myfunction (warrant* i,warrant* j) {
	return i->Egearing > j->Egearing;
}

vector<warrant*> s1algo::getSelectedWarrantFromMarketByIssuer(std::string issuer, unsigned int underlying, unsigned long long ubid, unsigned long long uask)
{

	unsigned long long uspread =  uask - ubid;

	vector<warrant*> selectedWarrant;
	unordered_set<unsigned int> warrantVector = ivLoader.getWarrantByIssuer(issuer,underlying);
	for (const auto &n: warrantVector) {
		auto it = omdcMap.find(n);
		if(it != omdcMap.end()){
			auto wbest_bid_price = static_cast<unsigned long long>(it->second.m_Bid[0].m_iPrice) * 100000;
			auto wbest_ask_price = static_cast<unsigned long long>(it->second.m_Ask[0].m_iPrice) * 100000;

			if(wbest_ask_price < 4000000){
				continue;
			}

			if(wbest_bid_price == 0 || wbest_ask_price == 0)
				continue;

			unsigned long long wspread = wbest_ask_price - wbest_bid_price;

			WarrantIv wiv = ivLoader.getWarrantIv(n);

			if(wspread <= 0){
				continue;
			}

			if(wiv.Code == 0){
				continue;
			}
			if(wiv.Egearing < 4){
				continue;
			}

			bool accept = CSelectedWarrant.isAccept(uspread, wiv.Delta, wiv.Cratio, wspread, 2);

			if(accept){

				auto itdef = omdcAdditionDefinitionsMap.find(wiv.Code);
				unsigned long long lotsize = 0;
				if(itdef != omdcAdditionDefinitionsMap.end()){
					lotsize = static_cast<unsigned long long>(itdef->second.LotSize);
				}
				if(lotsize == 0)
					continue;

				warrant* newWarrant = new warrant;
				newWarrant->Date = DateUtil::getToday();
				newWarrant->Code = n;
				newWarrant->Name = itdef->second.SecuritySortName;
				newWarrant->Status = STATUS_READY;
				newWarrant->Egearing = wiv.Egearing;
				newWarrant->UCode = underlying;
				newWarrant->RefWBid = wbest_bid_price;
				newWarrant->RefWAsk = wbest_ask_price;
				newWarrant->BuyQuantity = algoBet.fixQuantity(wbest_ask_price, lotsize);
				newWarrant->Quantity = 0;
				newWarrant->Issuer = wiv.Issuer;
				newWarrant->Status = STATUS_READY;

				selectedWarrant.push_back(newWarrant);
			}
		}
	}

	if(selectedWarrant.size() > (unsigned int)MaxBuyNoWarrant){

		std::sort (selectedWarrant.begin(), selectedWarrant.end(), myfunction);

		selectedWarrant.erase(selectedWarrant.begin()+MaxBuyNoWarrant, selectedWarrant.end());
	}
	return selectedWarrant;
}

void s1algo::on_omdc_trade(const Tradable& tradable)
{



	auto it = obMap.find(tradable.m_Code);
	if(it != obMap.end())
	{
		auto type = tradable.m_TradeType;
		auto side = tradable.m_TradeSide;
		auto bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
		auto ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
		auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;
		auto best_bid_vol = static_cast<unsigned long long>(tradable.m_Bid[0].m_uQuantity);
		auto best_ask_vol = static_cast<unsigned long long>(tradable.m_Ask[0].m_uQuantity);

		if (0 != type && 100 != type)
			return;

		//Log("on_omdc_trade code = " + to_string(tradable.m_Code) + " OBSetting");
		OBSetting* obs = obMap[tradable.m_Code];

		if(obs->hasPosition)
		{
			auto trade_quantity = static_cast<unsigned long long>(tradable.m_AccumulateSellQuantity);
			if(TradeSide::SELL_SIDE == side && trade_quantity >= best_bid_vol){
				vector<warrant*> wobsArray = obs->getRelatedWarrant();

				if(trade_price <= obs->getHighestStopLostPrice()){
					if(obs->hasRelatedWarrant(STATUS_AVAILABLE)){
						for(unsigned int i=0; i<wobsArray.size(); i++){
							if(wobsArray[i]->Status != STATUS_AVAILABLE){
								continue;
							}
							if(wobsArray[i]->StopLostPrice < best_bid_vol){
								continue;
							}

							auto it = omdcMap.find(wobsArray[i]->Code);
							if(it != omdcMap.end()){
								auto wbest_bid_price = static_cast<unsigned long long>(it->second.m_Bid[0].m_iPrice) * 100000;

								wobsArray[i]->Status = STATUS_SELLING;
								bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::sell, wbest_bid_price, wobsArray[i]->Quantity);
								if(!result){
									obs->setRelatedWarrantStatus(wobsArray[i]->Code, STATUS_AVAILABLE);
								}
							}
						}
					}
				}

			}

			return;
		}

		if(obs->detected)
		{
			auto trade_quantity = static_cast<unsigned long long>(tradable.m_AccumulateBuyQuantity);
			if(TradeSide::BUY_SIDE == side && trade_quantity >= best_ask_vol){
				vector<warrant*> wobsArray = obs->getRelatedWarrant();

				for(unsigned int i=0; i<wobsArray.size(); i++){
					if(wobsArray[i]->Status != STATUS_READY){
						continue;
					}

					wobsArray[i]->Status = STATUS_PENDING;
					wobsArray[i]->StopLostPrice = wobsArray[i]->RefWBid;
					bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::buy, wobsArray[i]->RefWAsk, wobsArray[i]->BuyQuantity);
					if(!result){
						obs->removeWarrantOrCbbc(wobsArray[i]->Code);
					}
					Log("Do Buy Warrant Code =  " = to_string(wobsArray[i]->Code) + " @ " + to_string(wobsArray[i]->RefWAsk));
				}

				if(obs->hasWarrants()){
					Log("Ready Buy Enter :obs->hasWarrants() ");
					//if(algoActionInterface->enableIB()){
					if(obs->hasRelatedWarrant(STATUS_PENDING)){
						Log("Ready Buy Enter :obs->hasRelatedWarrant(STATUS_PENDING)");
						obs->Status = STATUS_PENDING;
						obs->hasPosition = false;
						obs->BuyPrice = ask_price;
						obs->StopLostPrice = bid_price;
						obs->BuyTime = DateUtil::getCurrentTime();
						obs->TradeTime = DateUtil::getCurrentSystemTime();

						//algoActionInterface->showLog("Ready Buy Enter :obs->Status=STATUS_PENDING");
					}else{
						Log("Ready Buy Enter :obs->hasRelatedWarrant(STATUS_PENDING) False");
					}
						//}
				}else{
					Log("Ready Buy Enter :obs->hasWarrants() false ");
					obs->removeAllWarrants();
					obs->detected = false;
				}
			}
		}
	}

}

bool s1algo::doWarrantAction(warrant* w, dbp::top::order_side side, unsigned long long price, unsigned long long quantity)
{

	auto odr = this->_u.new_order(
			this,
			quantity,
			price,
			w->Code,
			side,
			dbp::top::order_type::sl,
			dbp::top::aon_type::non_ano,
			dbp::top::ignore_price_type::ignore,
			0);
	if (odr.is_valid())
	{
		order_map[odr.order_id] = w->UCode;
		return true;
	}
	return false;
}

void s1algo::on_omdd_book(const Tradable& )
{

}

void s1algo::on_omdd_trade(const Tradable& )
{

}


void s1algo::handler_order(const dbp::top::enhance_order& odr)
{
	auto status = odr.status;
	auto side = odr.side;

	auto it = order_map.find(odr.order_id);
	if(order_map.end() != it)
	{
		unsigned int ucode = it->second;
		OBSetting* obs = obMap[ucode];
		unsigned int code = odr.code;
		if (dbp::top::order_status::rejected == status || dbp::top::order_status::canceled == status || dbp::top::order_status::deleted == status || dbp::top::order_status::filled == status)
		{
			if (dbp::top::order_side::buy == side)
			{
				if (dbp::top::order_status::filled == status)
				{
					warrant* obsw = obs->getRelatedWarrant(code);
					obsw->BuyPrice = odr.match_price;
					obsw->BuyTime = std::string(odr.transaction_tm);
					obsw->Quantity += odr.filled_quantity;
					obsw->OrderId = odr.order_id;

					auto msg = algo_order_msg_pool.get_obj();
					msg->al = this;
					msg->algo_name = this->_name;
					msg->id = this->_u.get_id();
					msg->ref = to_string(code);
					msg->orderid = odr.order_id;
					msg->warrant_code = code;
					msg->side = "BUY";
					msg->filled_price = odr.match_price;
					msg->filled_quantity = odr.filled_quantity;
					msg->order_price = obsw->BuyPrice;
					msg->order_quantity = obsw->BuyQuantity;
					msg->status = "filled";
					msg->transaction_time = obsw->BuyTime;
					ouputQueue.enqueue(msg);

					if(obs->allStatus(STATUS_AVAILABLE)){
						obs->Status = STATUS_AVAILABLE;
						obs->hasPosition = true;
					}

					Log("Filed Buy Warrant Code =  " = to_string(code));
				}
				if (dbp::top::order_status::canceled == status || dbp::top::order_status::rejected == status)
				{

					warrant* wobs = obs->getRelatedWarrant(code);
					if(wobs->Status == STATUS_PENDING)
					{

						auto msg = algo_order_msg_pool.get_obj();
						msg->al = this;
						msg->algo_name = this->_name;
						msg->id = this->_u.get_id();
						msg->ref = to_string(code);
						msg->orderid = odr.order_id;
						msg->warrant_code = code;
						msg->side = "BUY";
						msg->order_price = wobs->BuyPrice;
						msg->order_quantity = wobs->BuyQuantity;
						msg->transaction_time = wobs->BuyTime;
						msg->status = "cancel";
						msg->reason = string(odr.reject_reason);
						ouputQueue.enqueue(msg);

						wobs->Status = STATUS_REJECTED;
						obs->removeWarrantOrCbbc(code);
						Log( "Cancelled Warrant Code = " + to_string(code));
					}
				}
			}
			else if (dbp::top::order_side::sell == side)
			{
				if (dbp::top::order_status::filled == status)
				{
					warrant* obsw = obs->removeWarrantOrCbbc(code);

					obsw->Status = STATUS_SOLD;
					obsw->Status = STATUS_SOLD;
					obsw->SoldTime = std::string(odr.transaction_tm);
					obsw->SellPrice = odr.match_price;

					obsw->UBuyPrice = obs->BuyPrice;
					obsw->USoldPrice = obs->SellPrice;

					if(obsw->Quantity == odr.filled_quantity)
					{
						Log(" Sell Security Code = " + to_string(code));

						auto msg = algo_order_msg_pool.get_obj();
						msg->al = this;
						msg->algo_name = this->_name;
						msg->id = this->_u.get_id();
						msg->ref = to_string(code);
						msg->orderid = odr.order_id;
						msg->warrant_code = code;
						msg->side = "SELL";
						msg->filled_price = odr.match_price;
						msg->filled_quantity = odr.filled_quantity;
						msg->order_price = obsw->SellPrice;
						msg->order_quantity = obsw->Quantity;
						msg->transaction_time = obsw->SoldTime;
						msg->status = "filled";
						ouputQueue.enqueue(msg);


						auto pmsg = algo_portfolio_msg_pool.get_obj();
						pmsg->al = this;
						pmsg->algo_name = this->_name;
						pmsg->id = this->_u.get_id();
						pmsg->ref = to_string(code);
						pmsg->warrant_code = code;
						pmsg->buy_price = obsw->BuyPrice;
						pmsg->sell_price = obsw->SellPrice;
						pmsg->quantity = obsw->Quantity;
						pmsg->buytime = obsw->BuyTime;
						pmsg->selltime= obsw->SoldTime;
						ouputQueue.enqueue(pmsg);

					}else{
						auto msg = algo_order_msg_pool.get_obj();
						msg->al = this;
						msg->algo_name = this->_name;
						msg->id = this->_u.get_id();
						msg->ref = to_string(code);
						msg->orderid = odr.order_id;
						msg->warrant_code = code;
						msg->side = "SELL";
						msg->filled_price = odr.match_price;
						msg->filled_quantity = odr.filled_quantity;
						msg->order_price = obsw->SellPrice;
						msg->order_quantity = obsw->Quantity;
						msg->transaction_time = obsw->SoldTime;
						msg->status = "Partial filled";
						ouputQueue.enqueue(msg);

						auto pmsg = algo_portfolio_msg_pool.get_obj();
						pmsg->al = this;
						pmsg->algo_name = this->_name;
						pmsg->id = this->_u.get_id();
						pmsg->ref = to_string(code);
						pmsg->warrant_code = code;
						pmsg->buy_price = obsw->BuyPrice;
						pmsg->sell_price = obsw->SellPrice;
						pmsg->quantity = odr.filled_quantity;
						pmsg->buytime = obsw->BuyTime;
						pmsg->selltime= obsw->SoldTime;
						ouputQueue.enqueue(pmsg);


						obsw->Quantity = obsw->Quantity - odr.filled_quantity;
						obsw->SoldTime = "";
						obsw->SellPrice = 0;
						obsw->Status = STATUS_AVAILABLE;
						obs->addWarrantOrCbbc(obsw);
					}

					Log("Filed Sell Warrant Code =  " = to_string(code));

					if(obs->getRelatedWarrantCount() == 0){
						obs->hasPosition = false;
						obs->detected = false;
					}

				}
				if (dbp::top::order_status::canceled == status || dbp::top::order_status::rejected == status)
				{
					warrant* wobs = obs->getRelatedWarrant(code);

					if(wobs->Status == STATUS_SELLING)
					{
						auto msg = algo_order_msg_pool.get_obj();
						msg->al = this;
						msg->algo_name = this->_name;
						msg->id = this->_u.get_id();
						msg->ref = to_string(code);
						msg->orderid = odr.order_id;
						msg->warrant_code = code;
						msg->side = "SELL";
						msg->order_price = wobs->SellPrice;
						msg->order_quantity = wobs->Quantity;
						msg->transaction_time =  string(odr.transaction_tm);
						msg->status = "cancel";
						msg->reason = string(odr.reject_reason);
						ouputQueue.enqueue(msg);

						Log("Sell Cancelled Warrant Code = " + to_string(code) + " Update Status to Available");

						obs->setRelatedWarrantStatus(code, STATUS_AVAILABLE);
						obs->Status = STATUS_AVAILABLE;
					}
				}
			}
		}

	}
}

void s1algo::Log(string msg){
	fprintf(stderr, "%s %s \n",DateUtil::getCurrentTime(), msg.c_str());
}

void s1algo::handle_command(algo_msg_base& msg)
{
	msg.on_command();
}

algo_msg_base* s1algo::json_to_msg(json& )
{
	return nullptr;
}

std::string s1algo::get_lib_name()
{
	return "s1algo";
}

rapid_ring::spsc_ring_buffer_object_pool<s1algo::algo_order_msg, 8192> s1algo::algo_order_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1algo::algo_portfolio_msg, 8192> s1algo::algo_portfolio_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1algo::algo_signal_msg, 8192> s1algo::algo_signal_msg_pool;
