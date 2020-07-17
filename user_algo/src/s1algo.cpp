#include <vector>
#include <s1algo.hpp>

//std::unordered_map<unsigned int, priceinfo*> bear::uprice_map;

s1algo::s1algo(user& u, const std::string& name):
	algo(u, name)
{
	selectedIssuer.insert("MB");
	selectedIssuer.insert("GS");
	selectedIssuer.insert("MS");
	selectedIssuer.insert("CS");

	string today =  DateUtil::getToday();
	forceSoldTime =  DateUtil::getTodayTime(today + " 154659");
	undetectedTime =  DateUtil::getTodayTime(today + " 153600");
	soldendTime =  DateUtil::getTodayTime(today + " 155959");

	MarketStatus = MARKET_START;

	vector<WarrantIv> allW = ivLoader.allWarrants();
	for(unsigned int i=0; i<allW.size(); i++){
		warrantPriceMap[allW[i].Code] = new priceinfo();
		warrantPriceMap[allW[i].Code]->Bestbid = 0;
		warrantPriceMap[allW[i].Code]->Bestask = 0;
		//Log("Init priceinfo = " + to_string(allW[i].Code));
	}

	logger = new ThreadLogger("log/" + name + DateUtil::getToday() + ".log");
	logger->start();

	Log("Logger Inited");

}

void s1algo::on_omdc_book(const Tradable& tradable)
{

	auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
	auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
	auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
	//auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
	unsigned int code = tradable.m_Code;

	auto itw = warrantPriceMap.find(code);
	if(itw != warrantPriceMap.end()){
		priceinfo* p = itw->second;
		p->Bestbid = best_bid_price;
		p->Bestask = best_ask_price;
		return;
	}

	auto itob = obMap.find(code);
	if(itob != obMap.end())
	{
		//Log("on_omdc_book code = " + to_string(tradable.m_Code) + " OBSetting");
		OBSetting* obs = itob->second;
		if(obs->hasPosition)
		{
			unsigned long long spread = spreadTable.getSpread(obs->SpreadTableCode, best_bid_price-1);
			unsigned long long diffu = best_bid_price - obs->StopLostPrice;
			unsigned long long diffw = best_bid_price - obs->getHighestStopLostPrice();

			int countspreadu = static_cast<int>(diffu/spread);
			int countspreadw = static_cast<int>(diffw/spread);

			s1signal* s1 = s1SignalMap[code];
			if( ((countspreadw > 0) || (countspreadu > 0) || (best_bid_price > obs->StopLostPrice && best_bid_qty > s1->RaiseStopLost)) && ((obs->Status == STATUS_AVAILABLE))){
				unsigned long long oldstoplost = obs->StopLostPrice;
				if(countspreadu > 0){
					if(best_bid_qty > s1->RaiseStopLost){
						obs->StopLostPrice = best_bid_price;
					}
				}

				if(obs->StopLostPrice > oldstoplost){
					Log("bid->Quantity0 = " + to_string(best_bid_qty) + " as->RaiseStopLost = " + to_string(s1->RaiseStopLost));
					Log("Security Code = " + to_string(code) + " Rise Stop Lost Price from " + to_string(oldstoplost) + " To " + to_string(obs->StopLostPrice));
				}

				if(countspreadw > 0){
					vector<warrant*> obsw = obs->getRelatedWarrant();
					for(unsigned int i=0; i<obsw.size(); i++){

						PriceMark* spm = pricemarkMap[obsw[i]->Code];

						unsigned long long wbest_bid_price = warrantPriceMap[obsw[i]->Code]->Bestbid;

						//auto it2 = omdcMap.find(obsw[i]->Code);
						//if(it2 != omdcMap.end()){
						//	auto wbest_bid_price = static_cast<unsigned long long>(it2->second.m_Bid[0].m_iPrice) * 100000;
							//auto wbest_ask_price = static_cast<unsigned long long>(it->second.m_Ask[0].m_iPrice) * 100000;

							unsigned long long fpcb = spm->sellOut(wbest_bid_price);
							if(fpcb > obsw[i]->StopLostPrice  && fpcb <= obs->StopLostPrice && fpcb <= best_bid_price){
								obsw[i]->StopLostPrice = fpcb;
								obsw[i]->RefWBid = wbest_bid_price;
							}else if(fpcb > obsw[i]->StopLostPrice && fpcb <= obs->StopLostPrice && fpcb > best_bid_price){
								obsw[i]->StopLostPrice = best_bid_price;
								obsw[i]->RefWBid = wbest_bid_price;
							}
						//}

					}
				}
			}
			return;
		}

		auto it = s1SignalMap.find(code);
		if(it != s1SignalMap.end()){

			s1signal* signal = it->second;
			bool hasSignal = signal->hasSignal;
			unsigned long long DetectAsk = signal->DetectAsk;

			if(obs->detected){

				if(best_ask_price != obs->DetectedAsk && obs->Status == STATUS_READY){
					//obs->Status = STATUS_NEW;
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
					obs->detected = false;

					Log("Code = " + to_string(code) + " Reset Signal 1");
					Log("Pass1");
					return;
				}
				if(!hasSignal  && obs->Status == STATUS_READY){
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
					obs->detected = false;

					Log("Code = " + to_string(code) + " Reset Signal 2");
					Log("Pass2");
				}
				return;
			}
			else
			{
				if(hasSignal){

					if(best_ask_price == DetectAsk){
						obs->DetectedAsk = best_ask_price;
						obs->StopLostPrice = best_bid_price;

						if(obs->SpreadTableCode == ""){
							COmdcAdditionDefinitions omdcdef = omdcAdditionDefinitionsMap[code];
							obs->SpreadTableCode = omdcdef.SpreadTableCode;
						}

						auto pmsg = algo_signal_msg_pool.get_obj();
						pmsg->al = this;
						pmsg->algo_name = this->_name;
						pmsg->id = this->_u.get_id();
						pmsg->ref = to_string(code);
						pmsg->code = code;
						pmsg->detect_ask = obs->DetectedAsk;
						pmsg->selected = true;

						int selectcount = 0;

						//for(unsigned int j=0; j<selectedIssuer.size(); j++){
						for(auto f : selectedIssuer) {
							string issuer = f;

							vector<warrant*> selectedWarrant = getSelectedWarrantFromMarketByIssuer(issuer,code, best_bid_price,best_ask_price );
							if(selectedWarrant.size() == 0)
								continue;

							for(unsigned int i=0; i<selectedWarrant.size(); i++){
								warrant* w = selectedWarrant[i];

								Log("Issuer = " + issuer + " Selected Code = " + to_string(w->Code));

								w->Status = STATUS_READY;
								obs->addWarrantOrCbbc(w);

								std::string wStr = to_string(w->Code) + "|" + w->Name + "|" + to_string(w->RefWAsk);
								pmsg->detectedlist.insert(wStr);
								selectcount++;
							}

						}

						if(selectcount > 0){

							ouputQueue.enqueue(pmsg);


							obs->Status = STATUS_READY;
							obs->detected = true;

							Log("Code = " + to_string(code) + " Has Signal @ " + to_string(best_ask_price));
							Log("Pass3");
						}else{
							algo_signal_msg_pool.release_obj(pmsg);
						}
					}
				}
			}
			//}
		}
	}else{
		auto it = s1SignalMap.find(code);
		if(it != s1SignalMap.end()){
			obMap[code] = new OBSetting();
			obMap[code]->detected = false;
			obMap[code]->SpreadTableCode = "";
			obMap[code]->hasPosition = false;
			obMap[code]->Status = STATUS_NEW;
			Log("Init = " + to_string(code) + " OBSetting");
		}
	}

}

bool myfunction (warrant* i,warrant* j) {
	return i->Egearing > j->Egearing;
}

void s1algo::setBetsize(std::string betsize){
	algoBet.selectBet(betsize);
}

vector<warrant*> s1algo::getSelectedWarrantFromMarketByIssuer(std::string issuer, unsigned int underlying, unsigned long long ubid, unsigned long long uask)
{

	//long long uspread =  static_cast<long long>(uask/100000 - ubid/100000);
	unsigned long long uspread =  uask - ubid;

	vector<warrant*> selectedWarrant;
	unordered_set<unsigned int> warrantVector = ivLoader.getWarrantByIssuer(issuer,underlying);
	for (const auto &n: warrantVector) {


		unsigned long long wbest_bid_price = warrantPriceMap[n]->Bestbid;
		unsigned long long wbest_ask_price = warrantPriceMap[n]->Bestask;

		//auto it2 = omdcMap.find(n);
		//if(it2 != omdcMap.end()){
			//auto wbest_bid_price = static_cast<unsigned long long>(it2->second.m_Bid[0].m_iPrice) * 100000;
			//auto wbest_ask_price = static_cast<unsigned long long>(it2->second.m_Ask[0].m_iPrice) * 100000;

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

			//float fuspread = static_cast<float>(uspread/100000)/1000.0f;
			//Log("USPREAD = " + to_string(fuspread));
			//float fwspread = static_cast<float>(wspread/100000)/1000.0f;
			//Log("WSPREAD = " + to_string(fwspread));

			//bool accept = CSelectedWarrant.isAccept(uspread, wiv.Delta, wiv.Cratio, wspread, 2);
			bool accept = CSelectedWarrant.isAccept(fuspread, wiv.Delta, wiv.Cratio, fwspread, 2);

			if(accept){

				auto itdef = omdcAdditionDefinitionsMap.find(wiv.Code);
				unsigned long long lotsize = 0;
				if(itdef != omdcAdditionDefinitionsMap.end()){
					lotsize = static_cast<unsigned long long>(itdef->second.LotSize);

					if(lotsize == 0)
						continue;

					warrant* newWarrant = new warrant;
					newWarrant->Date = DateUtil::getToday();
					newWarrant->Code = n;
					newWarrant->Name = itdef->second.SecuritySortName;
					//newWarrant->Status = STATUS_READY;
					newWarrant->Egearing = wiv.Egearing;
					newWarrant->UCode = underlying;
					newWarrant->RefWBid = wbest_bid_price;
					newWarrant->RefWAsk = wbest_ask_price;
					newWarrant->BuyQuantity = algoBet.fixQuantity(wbest_ask_price, lotsize)*100000000ull;
					newWarrant->Quantity = 0;
					newWarrant->Issuer = wiv.Issuer;
					newWarrant->Status = STATUS_READY;
					newWarrant->UBid = ubid;
					newWarrant->UAsk = uask;


					selectedWarrant.push_back(newWarrant);
				}
			}
		//}
	}
	unsigned int ssize = selectedWarrant.size();
	if(selectedWarrant.size() > (unsigned int)MaxBuyNoWarrant){

		Log("Accepted Size = " + to_string(ssize));
		std::sort (selectedWarrant.begin(), selectedWarrant.end(), myfunction);
		for(unsigned int i=MaxBuyNoWarrant; i<ssize; i++){
			delete selectedWarrant[i];
		}
		selectedWarrant.erase(selectedWarrant.begin()+MaxBuyNoWarrant, selectedWarrant.end());
		Log("Maximum selected= " + to_string(MaxBuyNoWarrant) + " " + issuer + " Selected = " + to_string(selectedWarrant.size()) + " on " + to_string(underlying) );
	}
	return selectedWarrant;
}

unsigned long long s1algo::getBestBid(unsigned int code){
	auto it2 = omdcMap.find(code);
	if(it2 != omdcMap.end()){
		auto wbest_bid_price = static_cast<unsigned long long>(it2->second.m_Bid[0].m_iPrice) * 100000;
		return wbest_bid_price;
	}
	return 0;
}

void s1algo::on_omdc_trade(const Tradable& tradable)
{

	if(MarketStatus == MARKET_PAUSE)
		return;

	auto it = obMap.find(tradable.m_Code);
	if(it != obMap.end())
	{
		auto code = tradable.m_Code;
		auto type = tradable.m_TradeType;
		auto side = tradable.m_TradeSide;
		auto bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
		auto ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
		auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;
		auto best_bid_vol = static_cast<unsigned long long>(tradable.m_Bid[0].m_uQuantity);
		auto best_ask_vol = static_cast<unsigned long long>(tradable.m_Ask[0].m_uQuantity);
		auto trade_sell_quantity = static_cast<unsigned long long>(tradable.m_AccumulateSellQuantity);
		auto trade_buy_quantity = static_cast<unsigned long long>(tradable.m_AccumulateBuyQuantity);
		if (0 != type && 100 != type)
			return;

		//Log("on_omdc_trade code = " + to_string(tradable.m_Code) + " OBSetting");
		OBSetting* obs = it->second;

		if(obs->hasPosition)
		{

			if(TradeSide::SELL_SIDE == side && trade_sell_quantity >= best_bid_vol){

				unsigned long long highestStopLost = obs->getHighestStopLostPrice();
				Log("UCode = " + to_string(code) + " Highest StopLost = " + to_string(highestStopLost));
				vector<warrant*> wobsArray = obs->getRelatedWarrant();

				if(trade_price <= highestStopLost){
					if(obs->hasRelatedWarrant(STATUS_AVAILABLE)){
						for(unsigned int i=0; i<wobsArray.size(); i++){
							if(wobsArray[i]->Status != STATUS_AVAILABLE){
								continue;
							}
							if(wobsArray[i]->StopLostPrice < bid_price){
								continue;
							}

							unsigned long long wbest_bid_price = warrantPriceMap[wobsArray[i]->Code]->Bestbid;

							//unsigned long long wbest_bid_price = getBestBid(wobsArray[i]->Code);

							if(wbest_bid_price == 0)
								continue;
							//auto it2 = omdcMap.find(wobsArray[i]->Code);
							//if(it2 != omdcMap.end()){
							//	auto wbest_bid_price = static_cast<unsigned long long>(it2->second.m_Bid[0].m_iPrice) * 100000;

							wobsArray[i]->Status = STATUS_SELLING;
							bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::sell, wbest_bid_price, wobsArray[i]->Quantity);
							//bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::sell, RefWBid, wobsArray[i]->Quantity);
							if(!result){
								obs->setRelatedWarrantStatus(wobsArray[i]->Code, STATUS_AVAILABLE);
							}
							//}
						}
						if(obs->hasRelatedWarrant(STATUS_SELLING)){
							obs->hasPosition = true;
							obs->SellPrice = bid_price;
							obs->SoldTime = DateUtil::getCurrentTime();
						}
					}
				}

			}

			return;
		}

		if(obs->detected)
		{

			if(TradeSide::BUY_SIDE == side && trade_buy_quantity >= best_ask_vol && obs->Status == STATUS_READY){
				vector<warrant*> wobsArray = obs->getRelatedWarrant();

				for(unsigned int i=0; i<wobsArray.size(); i++){
					if(wobsArray[i]->Status != STATUS_READY){
						continue;
					}

					wobsArray[i]->Status = STATUS_PENDING;
					wobsArray[i]->StopLostPrice = wobsArray[i]->UBid;
					bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::buy, wobsArray[i]->RefWAsk, wobsArray[i]->BuyQuantity);
					if(!result){
						warrant* w = obs->removeWarrantOrCbbc(wobsArray[i]->Code);
						delete w;
					}
					Log("Do Buy Warrant Code =  " + to_string(wobsArray[i]->Code) + " @ " + to_string(wobsArray[i]->RefWAsk));
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

					obsw->Status = STATUS_AVAILABLE;

					//unsigned long long wbest_bid_price = getBestBid(code);
					unsigned long long wbest_bid_price = warrantPriceMap[code]->Bestbid;
					//auto it2 = omdcMap.find(code);
					//if(it2 != omdcMap.end()){
						//auto wbest_bid_price = static_cast<unsigned long long>(it2->second.m_Bid[0].m_iPrice) * 100000;
						//auto wbest_ask_price = static_cast<unsigned long long>(it->second.m_Ask[0].m_iPrice) * 100000;

					PriceMark* spm = pricemarkMap[code];
					unsigned long long pcb = spm->sellOut(wbest_bid_price);
					if(pcb == 99999999){
						pcb = obs->StopLostPrice;
					}

					obsw->StopLostPrice = pcb;
					obsw->RefWBid = wbest_bid_price;

					Log("Warrant Code = " + to_string(code) + " PCB@" + to_string(pcb) + " @WBid = " + to_string(wbest_bid_price) );
					//}


					auto msg = algo_order_msg_pool.get_obj();
					msg->al = this;
					msg->algo_name = this->_name;
					msg->id = this->_u.get_id();
					msg->ref = to_string(code);
					msg->orderid = odr.order_id;
					msg->warrant_code = code;
					msg->ucode = ucode;
					msg->side = "BUY";
					msg->filled_price = odr.match_price;
					msg->filled_quantity = odr.filled_quantity;
					msg->order_price = obsw->BuyPrice;
					msg->order_quantity = obsw->BuyQuantity;
					msg->stoplost = obsw->StopLostPrice;
					msg->status = "filled";
					msg->transaction_time = obsw->BuyTime;
					ouputQueue.enqueue(msg);

					if(obs->allStatus(STATUS_AVAILABLE)){
						obs->Status = STATUS_AVAILABLE;
						obs->hasPosition = true;
					}

					Log("Filed Buy Warrant Code =  " + to_string(code) + " UCode = " + to_string(ucode) + " Status = " + to_string(obs->Status));
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
						msg->ucode = ucode;
						msg->side = "BUY";
						msg->stoplost = 0;
						msg->order_price = wobs->BuyPrice;
						msg->order_quantity = wobs->BuyQuantity;
						msg->transaction_time = wobs->BuyTime;
						msg->status = "cancel";
						msg->reason = string(odr.reject_reason);
						ouputQueue.enqueue(msg);

						wobs->Status = STATUS_REJECTED;
						warrant* w = obs->removeWarrantOrCbbc(code);
						delete w;
						Log( "Cancelled Warrant Code = " + to_string(code) + " UCode = " + to_string(ucode));
					}

					if(obs->getRelatedWarrantCount() == 0 ){
						obs->detected = false;
					}else{
						if(obs->allStatus(STATUS_AVAILABLE)){
							obs->Status = STATUS_AVAILABLE;
							obs->hasPosition = true;
						}
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
						msg->ucode = ucode;
						msg->side = "SELL";
						msg->stoplost = 0;
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
						msg->ucode = ucode;
						pmsg->buy_price = obsw->BuyPrice;
						pmsg->sell_price = obsw->SellPrice;
						pmsg->quantity = obsw->Quantity;
						pmsg->buytime = obsw->BuyTime;
						pmsg->selltime= obsw->SoldTime;
						ouputQueue.enqueue(pmsg);

						delete obsw;
					}else{
						auto msg = algo_order_msg_pool.get_obj();
						msg->al = this;
						msg->algo_name = this->_name;
						msg->id = this->_u.get_id();
						msg->ref = to_string(code);
						msg->orderid = odr.order_id;
						msg->warrant_code = code;
						msg->ucode = ucode;
						msg->side = "SELL";
						msg->stoplost = 0;
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
						msg->ucode = ucode;
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

					Log("Filed Sell Warrant Code =  " + to_string(code) + " UCode = " + to_string(ucode));

					if(obs->getRelatedWarrantCount() == 0){
						//obs->Status = STATUS_NEW;
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
						msg->stoplost = 0;
						msg->order_price = wobs->SellPrice;
						msg->order_quantity = wobs->Quantity;
						msg->transaction_time =  string(odr.transaction_tm);
						msg->status = "cancel";
						msg->reason = string(odr.reject_reason);
						ouputQueue.enqueue(msg);

						Log("Sell Cancelled Warrant Code = " + to_string(code) + " Update Status to Available "  + " UCode = " + to_string(ucode));

						obs->setRelatedWarrantStatus(code, STATUS_AVAILABLE);
						obs->Status = STATUS_AVAILABLE;
					}
				}
			}
		}

	}
}

void s1algo::Log(string msg){
	//fprintf(stderr, "%s %s \n",DateUtil::getCurrentTime(), msg.c_str());
	//logger

	logger->Log(string(DateUtil::getCurrentTime()) + " " + msg);

	//flush_printf("%s %s\n", DateUtil::getCurrentTime(), msg.c_str());
}

void s1algo::handle_command(algo_msg_base& msg)
{
	msg.on_command();
}

algo_msg_base* s1algo::json_to_msg(json& json)
{
	algo_marketstatus_msg* pMarketStatus_msg = nullptr;
	algo_setbet_msg* pSetBet_msg = nullptr;
	try
	{
		auto cmd = json["cmd"].get<std::string>();
		auto ref = json["ref"].get<std::string>();
		if (cmd == "marketstatus"){
			pMarketStatus_msg = algo_marketstatus_msg_pool.get_obj();
			pMarketStatus_msg->al = this;
			pMarketStatus_msg->algo_name = _name;
			pMarketStatus_msg->id = _u.get_id();
			pMarketStatus_msg->ref = ref;
			pMarketStatus_msg->action = json["action"].get<std::string>();
			return pMarketStatus_msg;
		}
		else if (cmd == "betsize"){
			pSetBet_msg = algo_setbet_msg_pool.get_obj();
			pSetBet_msg->al = this;
			pSetBet_msg->algo_name = _name;
			pSetBet_msg->id = _u.get_id();
			pSetBet_msg->ref = ref;
			pSetBet_msg->betsize = json["betsize"].get<std::string>();
			return pSetBet_msg;
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

		if (pMarketStatus_msg)
			pMarketStatus_msg->release();
		if (pSetBet_msg)
			pSetBet_msg->release();

		return msg;
	}
}

std::string s1algo::get_lib_name()
{
	return "s1algo";
}

rapid_ring::spmc_ring_buffer_object_pool<s1algo::algo_err_msg, 8192> s1algo::algo_err_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<s1algo::algo_marketstatus_msg, 8192> s1algo::algo_marketstatus_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<s1algo::algo_setbet_msg, 8192> s1algo::algo_setbet_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<s1algo::algo_order_msg, 8192> s1algo::algo_order_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1algo::algo_portfolio_msg, 8192> s1algo::algo_portfolio_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1algo::algo_signal_msg, 8192> s1algo::algo_signal_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1algo::algo_stoplost_msg, 8192> s1algo::algo_stoplost_msg_pool;
