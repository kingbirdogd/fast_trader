#include <vector>
#include <s1csalgo.hpp>

//std::unordered_map<unsigned int, priceinfo*> bear::uprice_map;

s1csalgo::s1csalgo(user& u, const std::string& name):
	algo(u, name)
{
	MaxBuyNoWarrant = 2;

	selectedIssuer.insert("CS");
	//selectedIssuer.insert("GS");
	//selectedIssuer.insert("MS");
	//selectedIssuer.insert("CS");

	string today =  DateUtil::getToday();
	forceSoldTime =  DateUtil::getTodayTime(today + " 154659");
	undetectedTime =  DateUtil::getTodayTime(today + " 153600");
	soldendTime =  DateUtil::getTodayTime(today + " 155959");


	lastReadyTime =  0;


	MarketStatus = MARKET_PAUSE;

	vector<WarrantIv> allW = ivLoader.allWarrants();
	for(unsigned int i=0; i<allW.size(); i++){
		warrantPriceMap[allW[i].Code] = new priceinfo();
		warrantPriceMap[allW[i].Code]->Code = allW[i].Code;
		warrantPriceMap[allW[i].Code]->Bestbid = 0;
		warrantPriceMap[allW[i].Code]->Bestask = 0;
		warrantPriceMap[allW[i].Code]->PBestbid = 0;
		warrantPriceMap[allW[i].Code]->PBestask = 0;
		warrantPriceMap[allW[i].Code]->UCode = allW[i].UCode;
		warrantPriceMap[allW[i].Code]->Issuer = allW[i].Issuer;
		//Log("Init priceinfo = " + to_string(allW[i].Code));

		auto it = availableUCode.find(allW[i].UCode);
		if(it == availableUCode.end()){
			availableUCode.insert(allW[i].UCode);
		}
	}

	//algoBet.selectBet("Bet100");
	algoBet.selectBet("BetSmall");

	logger = new ThreadLogger("log/" + name + DateUtil::getToday() + ".log");
	logger->start();


	//unordered_set<unsigned int> allucode = ivLoader.allUnderlying();

	for(auto f : availableUCode) {
		unsigned int ucode = f;
		obMap[ucode] = new OBSetting();
		obMap[ucode]->detected = false;
		//obMap[code]->SpreadTableCode = "";
		obMap[ucode]->hasPosition = false;
		obMap[ucode]->Status = STATUS_NEW;

		COmdcAdditionDefinitions omdcdef = omdcAdditionDefinitionsMap[ucode];
		obMap[ucode]->SpreadTableCode = omdcdef.SpreadTableCode;

		underlyingPriceMap[ucode] = new priceinfo();
		underlyingPriceMap[ucode]->Code = ucode;

		Log("Init = " + to_string(ucode) + " OBSetting and SpreadCode = " + obMap[ucode]->SpreadTableCode);
	}


	Log("Logger Inited ");
	Log("forceSoldTime = " + to_string(forceSoldTime) + " @" + today + " 154659");
	Log("undetectedTime = " + to_string(undetectedTime) + " @" + today + " 153600");
	Log("soldendTime = " + to_string(soldendTime) + " @" + today + " 155959");

	for(auto f : selectedIssuer) {
		string issuer = f;
		Log("Trade Issuer = " + issuer);
	}
}

void s1csalgo::on_omdc_book(const Tradable& tradable)
{

	auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
	auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
	auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
	auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
	//auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
	unsigned int code = tradable.m_Code;

	auto itw = warrantPriceMap.find(code);
	if(itw != warrantPriceMap.end()){
		priceinfo* p = itw->second;

		auto issuerit = selectedIssuer.find(p->Issuer);
		if(issuerit == selectedIssuer.end())
			return;

		auto itobs = obMap.find(p->UCode);
		if(itobs == obMap.end()){
			return;
		}

		OBSetting* obs = itobs->second;
		if(obs == nullptr)
			return;

		if(p->Bestbid != best_bid_price){
			p->PBestbid = p->Bestbid;

			if(obs->hasPosition){
				if(obs->isExist(code)){

					warrant* w = obs->getRelatedWarrant(code);
					if(w->isWinSell){
						if(w->BuyPrice > 0 && best_bid_price > 0){
							if(best_bid_price > w->BuyPrice && w->Status == STATUS_AVAILABLE){
								Log("Warrant Code = " + to_string(code) + " Do Quick Win Sell @ " + to_string(best_bid_price) + " Buy Price = " + to_string(w->BuyPrice));
								w->Status = STATUS_SELLING;
								bool result = doWarrantAction(w, dbp::top::order_side::sell, best_bid_price, w->Quantity);
								if(!result){
									obs->setRelatedWarrantStatus(w->Code, STATUS_AVAILABLE);
								}
							}
						}
					}
					if(w->isWinOrLvlSell){
						if(w->BuyPrice > 0 && best_bid_price > 0){
							if(best_bid_price >= w->BuyPrice && w->Status == STATUS_AVAILABLE){
								Log("Warrant Code = " + to_string(code) + " Do Quick Win Lvl Sell @ " + to_string(best_bid_price) + " Buy Price = " + to_string(w->BuyPrice));
								w->Status = STATUS_SELLING;
								bool result = doWarrantAction(w, dbp::top::order_side::sell, best_bid_price, w->Quantity);
								if(!result){
									obs->setRelatedWarrantStatus(w->Code, STATUS_AVAILABLE);
								}
							}
						}
					}

					auto msg = algo_warrantprice_msg_pool.get_obj();
					msg->al = this;
					msg->algo_name = _name;
					msg->id = _u.get_id();
					msg->ref = std::to_string(code);
					msg->warrant_code = code;
					msg->side = "BID";
					msg->wprice = best_bid_price;
					ouputQueue.enqueue(msg);

					Log("UCODE = " + to_string(p->UCode) + " Warrant Code = " + to_string(code) + " WBid Change from " + to_string(p->PBestbid) + " To " + to_string(best_bid_price));
				}
			}

		}

		if(p->Bestask != best_ask_price){
			p->PBestask = p->Bestask;

			if(obs->Status == STATUS_READY || obs->Status == STATUS_AVAILABLE || obs->Status == STATUS_PENDING){
				if(obs->isExist(code)){
					Log("UCODE = " + to_string(p->UCode) + " Warrant Code = " + to_string(code) + " WAsk Change from " + to_string(p->PBestask) + " To " + to_string(best_ask_price));
				}
			}
		}

		p->Bestbid = best_bid_price;
		p->Bestask = best_ask_price;
		p->BidQty = best_bid_qty;
		p->AskQty = best_ask_qty;

		if(best_bid_price >= 25000000 ){
			return;
		}



		PriceMark* spm = pricemarkMap[code];

		unsigned long long spread = spreadTable.getSpread("01", best_bid_price + 1llu);
		unsigned long long refask = best_bid_price+spread;

		unsigned long long buyin = spm->buyIn(refask);
		unsigned long long sellout = spm->buyIn(refask);

		priceinfo* uprice = underlyingPriceMap[p->UCode];

		if(obs->isExist(code)){
			warrant* newWarrant = obs->getRelatedWarrant(code);
			if(buyin==sellout){
				if(newWarrant->Status == STATUS_READY){
					newWarrant->UBid = uprice->Bestbid;
					newWarrant->UAsk = uprice->Bestask;
					newWarrant->RefWBid = best_bid_price;
					newWarrant->RefWAsk = best_ask_price;
					newWarrant->StopLostPrice = uprice->Bestbid;
				}
			}else{
				if(newWarrant->Status == STATUS_READY){
					obs->removeWarrantOrCbbc(code);

					Log("UCODE = " + to_string(p->UCode) + " Warrant Code = " + to_string(code) + " Remove Detected");

					return;
				}
			}

			if(newWarrant->Status == STATUS_AVAILABLE){
				unsigned long long newsellout = spm->sellOut(best_bid_price);
				if(newsellout > newWarrant->StopLostPrice && newsellout <= uprice->Bestbid){
					Log("UCODE = " + to_string(p->UCode) + " Warrant Code = " + to_string(code) + " Raise Stoplost from " + to_string(newWarrant->StopLostPrice) + " To " + to_string(newsellout));
					newWarrant->StopLostPrice = newsellout;
				}
			}
		}else{
			if(buyin==sellout){
				COmdcAdditionDefinitions omdcdef = omdcAdditionDefinitionsMap[code];

				warrant* newWarrant = new warrant;
				newWarrant->Date = DateUtil::getToday();
				newWarrant->Code = code;
				newWarrant->UCode = p->UCode;
				newWarrant->RefWBid = best_bid_price;
				newWarrant->RefWAsk = best_ask_price;
				newWarrant->BuyQuantity = algoBet.fixQuantityBySpread(best_ask_price, omdcdef.LotSize, 0)*100000000ull;
				newWarrant->Quantity = 0;
				newWarrant->Status = STATUS_READY;
				newWarrant->UBid = uprice->Bestbid;
				newWarrant->UAsk = uprice->Bestask;
				newWarrant->isWinSell = false;
				newWarrant->isWinOrLvlSell = false;
				newWarrant->StopLostPrice = uprice->Bestbid;
				obs->addWarrantOrCbbc(newWarrant);

				Log("UCODE = " + to_string(p->UCode) + " Warrant Code = " + to_string(code) + " Add Detected");

			}
		}
		return;
	}

	auto upriceauto = underlyingPriceMap.find(code);
	if(upriceauto == underlyingPriceMap.end()){
		return;
	}
	priceinfo* uprice  = upriceauto->second;
	uprice->Bestbid = best_bid_price;
	uprice->Bestask = best_ask_price;
}

bool myfunction (warrant* i,warrant* j) {
	return i->Egearing > j->Egearing;
}

string s1csalgo::setBetsize(std::string betsize){
	return algoBet.selectBet(betsize);
}


bool s1csalgo::setWinSell(std::string action, unsigned int ucode, unsigned int code){
	if(action == "set"){
		auto it = obMap.find(ucode);
		if(it != obMap.end()){
			OBSetting* obs = it->second;
			if(obs->hasPosition){
				if(obs->isExist(code)){
					warrant* w = obs->getRelatedWarrant(code);
					if(w != nullptr){
						w->isWinSell = true;
						return true;
					}
				}
			}
		}
	}
	if(action == "unset"){
		auto it = obMap.find(ucode);
		if(it != obMap.end()){
			OBSetting* obs = it->second;
			if(obs->hasPosition){
				if(obs->isExist(code)){
					warrant* w = obs->getRelatedWarrant(code);
					if(w != nullptr){
						w->isWinSell = false;
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool s1csalgo::setWinLvlSell(std::string action, unsigned int ucode, unsigned int code){
	if(action == "set"){
		auto it = obMap.find(ucode);
		if(it != obMap.end()){
			OBSetting* obs = it->second;
			if(obs->hasPosition){
				if(obs->isExist(code)){
					warrant* w = obs->getRelatedWarrant(code);
					if(w != nullptr){
						w->isWinOrLvlSell = true;
						return true;
					}
				}
			}
		}
	}
	if(action == "unset"){
		auto it = obMap.find(ucode);
		if(it != obMap.end()){
			OBSetting* obs = it->second;
			if(obs->hasPosition){
				if(obs->isExist(code)){
					warrant* w = obs->getRelatedWarrant(code);
					if(w != nullptr){
						w->isWinOrLvlSell = false;
						return true;
					}
				}
			}
		}
	}
	return false;
}


bool s1csalgo::setSelectedIssuer(std::string action, std::string issuer){

	if(MarketStatus == MARKET_START)
		return false;

	if(action == "select"){
		auto it = selectedIssuer.find(issuer);
		if(it == selectedIssuer.end()){
			selectedIssuer.insert(issuer);
			Log("Selected Issuer = " + issuer);
			return true;
		}
	}
	if(action == "remove"){
		auto it = selectedIssuer.find(issuer);
		if(it != selectedIssuer.end()){
			selectedIssuer.erase(issuer);
			Log("Remove Issuer = " + issuer);
			return true;
		}
	}
	return false;
}

bool s1csalgo::setSelectedUnderlying(std::string action, unsigned int ucode){

	//if(MarketStatus == MARKET_START)
	//	return false;

	if(action == "select"){
		auto it = unselectedUCode.find(ucode);
		if(it != unselectedUCode.end()){
			unselectedUCode.erase(ucode);
			Log("Selected Underlying = " + to_string(ucode));
			return true;
		}
	}
	if(action == "remove"){
		auto it = unselectedUCode.find(ucode);
		if(it == unselectedUCode.end()){
			unselectedUCode.insert(ucode);
			Log("Unselected Underlying = " + to_string(ucode));
			return true;
		}
	}
	return false;
}

bool s1csalgo::checkPrice(unsigned int code, unsigned long long ubid, unsigned long long uask)
{
	COmdcAdditionDefinitions omdcdef = omdcAdditionDefinitionsMap[code];
	string SpreadTableCode = omdcdef.SpreadTableCode;

	unsigned long long uspread =  uask - ubid;
	unsigned long long wbest_bid_price = warrantPriceMap[code]->Bestbid;
	unsigned long long wbest_ask_price = warrantPriceMap[code]->Bestask;
	unsigned long long wBidQty = warrantPriceMap[code]->BidQty;
	unsigned long long wAskQty = warrantPriceMap[code]->AskQty;

	PriceMark* spm = pricemarkMap[code];

	//unsigned long long wbidaskspread = wbest_ask_price - wbest_bid_price;

	if(wbest_bid_price == 0 || wbest_ask_price == 0 ||wBidQty<spm->getIssuerBidQty() || wAskQty<spm->getIssuerAskQty()){
		Log("Code = " + to_string(code) + " wbest_bid_price == 0 || wbest_ask_price == 0 ||wBidQty<spm->getIssuerBidQty() || wAskQty<spm->getIssuerAskQty() ");
		return false;
	}

	unsigned long long wspread = wbest_ask_price - wbest_bid_price;
	WarrantIv wiv = ivLoader.getWarrantIv(code);


	string key = to_string(ubid) +"-"+to_string(uask);

	unsigned long long refwbidaskSpread = spm->getWarrantBidAskSpread(key);
	Log("Code = " + to_string(code) + " BidaskSpread = " + to_string(wspread) + " RefBidAskSpread = " + to_string(refwbidaskSpread));

	float fuspread = static_cast<float>(uspread/100000)/1000.0f;

	float fwspread = static_cast<float>(wspread/100000)/1000.0f;


	unsigned long long _wspread = spreadTable.getSpread(SpreadTableCode, wbest_bid_price);

	if(_wspread == 0){
		Log("Code = " + to_string(code) + " if(_wspread == 0) ");
		return false;
	}

	int noofspread = static_cast<int>(wspread / _wspread);

	bool acceptspread = CSelectedWarrant.isSpreadAccept(noofspread, wbest_bid_price);
	if(!acceptspread){
		Log("Code = " + to_string(code) + " acceptspread ");
		return false;
	}

	bool accept = CSelectedWarrant.isAccept(fuspread, wiv.Delta, wiv.Cratio, fwspread, 2);
	if(!accept){
		Log("Code = " + to_string(code) + " CSelectedWarrant.isAccept ");
	}
	return accept;
}

bool s1csalgo::force_sell(unsigned int ucode, unsigned int code, unsigned long long price){
	auto it  = obMap.find(ucode);
	if(it != obMap.end()){
		OBSetting* obs = it->second;
		if(obs->hasPosition){

			if(obs->isExist(code)){
				if(obs->getWarrantStatus(code) == STATUS_AVAILABLE){
					warrant* wobs = obs->getRelatedWarrant(code);
					wobs->Status = STATUS_SELLING;
					bool result = doWarrantAction(wobs, dbp::top::order_side::sell, price, wobs->Quantity);
					if(!result){
						obs->setRelatedWarrantStatus(code, STATUS_AVAILABLE);
						return false;
					}
					return true;
				}

			}
		}
	}
	return false;
}

vector<warrant*> s1csalgo::getSelectedWarrantFromMarketByIssuer(std::string , unsigned int , unsigned long long , unsigned long long )
{
	vector<warrant*> selectedWarrant;
	return selectedWarrant;
}

unsigned long long s1csalgo::getBestBid(unsigned int code){
	auto it2 = omdcMap.find(code);
	if(it2 != omdcMap.end()){
		auto wbest_bid_price = static_cast<unsigned long long>(it2->second.m_Bid[0].m_iPrice) * 100000;
		return wbest_bid_price;
	}
	return 0;
}

void s1csalgo::on_omdc_trade(const Tradable& tradable)
{
	auto it = obMap.find(tradable.m_Code);
	if(it != obMap.end())
	{
		if(MarketStatus == MARKET_PAUSE)
			return;

		time_t currentTime = DateUtil::getCurrentSystemTime();
		if(currentTime > forceSoldTime && currentTime < soldendTime){
			forcesold();
			return;
		}

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

		OBSetting* obs = it->second;

		if(obs->hasRelatedWarrant(STATUS_AVAILABLE) && TradeSide::SELL_SIDE == side)
		{

			if(TradeSide::SELL_SIDE == side && trade_sell_quantity >= best_bid_vol){

				unsigned long long highestStopLost = obs->getHighestStopLostPrice();
				Log("UCode = " + to_string(code) + " Trade Price = " + to_string(trade_price) + " Highest StopLost = " + to_string(highestStopLost) + " Best Bid = " + to_string(bid_price) + " Best Ask = " + to_string(ask_price));

				vector<warrant*> wobsArray = obs->getRelatedWarrant();

				if(trade_price <= highestStopLost){
					if(obs->hasRelatedWarrant(STATUS_AVAILABLE)){
						for(unsigned int i=0; i<wobsArray.size(); i++){
							//unsigned long long t_start = dbp::tools::srv::current();

							if(wobsArray[i]->Status != STATUS_AVAILABLE){
								continue;
							}
							if(wobsArray[i]->StopLostPrice < bid_price){
								continue;
							}

							if(wobsArray[i]->BuyPrice <= 0){
								continue;
							}

							unsigned long long wbest_bid_price = warrantPriceMap[wobsArray[i]->Code]->Bestbid;

							if(wbest_bid_price == 0)
								continue;

							if(wbest_bid_price >= wobsArray[i]->BuyPrice){


								wobsArray[i]->Status = STATUS_SELLING;
								bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::sell, wbest_bid_price, wobsArray[i]->Quantity);
								//bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::sell, RefWBid, wobsArray[i]->Quantity);
								if(!result){
									obs->setRelatedWarrantStatus(wobsArray[i]->Code, STATUS_AVAILABLE);
									continue;
								}

								Log("Do Sell Warrant Code =  " + to_string(wobsArray[i]->Code) + " @ " + to_string(wbest_bid_price));

							}else{


								PriceMark* spm = pricemarkMap[wobsArray[i]->Code];

								unsigned long long expectSellOut = spm->sellOut(wbest_bid_price);

								unsigned long long spread = spreadTable.getSpread(obs->SpreadTableCode, bid_price + 1llu);
								unsigned long long refask = spread + bid_price;
								string ukey = to_string(bid_price) +"-"+ to_string(refask);

								unsigned long long refbid = spm->getRefWarrantBid(ukey);

								Log("WCode = " + to_string(wobsArray[i]->Code) + " Expect Sell Out = " + to_string(expectSellOut));
								Log("WCode = " + to_string(wobsArray[i]->Code) + " Ref Warrant bid = " + to_string(refbid) + " WBest Bid = " + to_string(wbest_bid_price));

								if(expectSellOut != 99999999){
									if(expectSellOut >=  trade_price){
										return;
									}
								}

								if(refbid > 0){
									if(wbest_bid_price < (refbid - 300000) ){
										return;
									}
								}

								wobsArray[i]->Status = STATUS_SELLING;
								bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::sell, wbest_bid_price, wobsArray[i]->Quantity);
								//bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::sell, RefWBid, wobsArray[i]->Quantity);
								if(!result){
									obs->setRelatedWarrantStatus(wobsArray[i]->Code, STATUS_AVAILABLE);
									continue;
								}


								Log("Do Sell Warrant Code =  " + to_string(wobsArray[i]->Code) + " @ " + to_string(wbest_bid_price));

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

		if(obs->getHighestDetectedBidPrice() == bid_price && TradeSide::BUY_SIDE == side)
		{
			Log("UCode =  " + to_string(code) + " Trade Price = " + to_string(trade_price) + " Qty = " + to_string(trade_buy_quantity) + " BestBid = " + to_string(bid_price) + " Ask Price = " + to_string(ask_price)) ;

			if(TradeSide::BUY_SIDE == side && trade_buy_quantity >= best_ask_vol){
				vector<warrant*> wobsArray = obs->getRelatedWarrant();

				for(unsigned int i=0; i<wobsArray.size(); i++){

					//unsigned long long t_start = dbp::tools::srv::current();

					if(wobsArray[i]->Status != STATUS_READY){
						continue;
					}

					wobsArray[i]->Status = STATUS_PENDING;
					wobsArray[i]->StopLostPrice = wobsArray[i]->UBid;
					bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::buy, wobsArray[i]->RefWAsk, wobsArray[i]->BuyQuantity);
					//bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::buy, wbest_ask_price, wobsArray[i]->BuyQuantity);
					if(!result){
						warrant* w = obs->removeWarrantOrCbbc(wobsArray[i]->Code);
						delete w;

						continue;
					}

					//Log("Do Buy Warrant Code =  " + to_string(wobsArray[i]->Code) + " @ " + to_string(wobsArray[i]->RefWAsk) + " time = " + to_string(t_diff) + " TCheck = " + to_string(t_docheck) + " TOrder = " + to_string(t_doorder));
					Log("Do Buy Warrant Code =  " + to_string(wobsArray[i]->Code) + " @ " + to_string(wobsArray[i]->RefWAsk) + "Wbid = " + to_string(wbest_ask_price));
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
						obs->HighestStopLost = bid_price;
						obs->BuyTime = DateUtil::getCurrentTime();
						obs->TradeTime = DateUtil::getCurrentSystemTime();

						//algoActionInterface->showLog("Ready Buy Enter :obs->Status=STATUS_PENDING");
					}else{
						Log("Ready Buy Enter :obs->hasRelatedWarrant(STATUS_PENDING) False");
					}
						//}
				}
			}
		}
	}

}

bool s1csalgo::doWarrantAction(warrant* w, dbp::top::order_side side, unsigned long long price, unsigned long long quantity)
{

	lastReadyTime += 10;

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

void s1csalgo::on_omdd_book(const Tradable& )
{

}

void s1csalgo::on_omdd_trade(const Tradable& )
{

}

void s1csalgo::forcesold(){

	for (auto& it: obMap) {
	    // Do stuff
	    OBSetting* obs = it.second;

	    if(obs->hasPosition){

			vector<warrant*> wobsArray = obs->getRelatedWarrant();
			for(unsigned int i = 0; i<wobsArray.size(); i++){
				warrant* obsw = wobsArray[i];
				if(obsw->Status == STATUS_AVAILABLE){
					unsigned long long wbest_bid_price = warrantPriceMap[obsw->Code]->Bestbid;

					if(wbest_bid_price == 0)
						continue;
					Log("UCode = " + to_string(obs->Code) + " > Force Sell Warrant Code = " + to_string(obsw->Code) + " @Wbid = " + to_string(wbest_bid_price));
					wobsArray[i]->Status = STATUS_SELLING;
					bool result = doWarrantAction(wobsArray[i], dbp::top::order_side::sell, wbest_bid_price, wobsArray[i]->Quantity);
					if(!result){
						obs->setRelatedWarrantStatus(wobsArray[i]->Code, STATUS_AVAILABLE);
					}
				}
				if(obs->hasRelatedWarrant(STATUS_SELLING)){
					obs->hasPosition = true;
					obs->SellPrice = 0;
					obs->SoldTime = DateUtil::getCurrentTime();
				}
			}
		}
	}
}


void s1csalgo::handler_order(const dbp::top::enhance_order& odr)
{
	auto status = odr.status;
	auto side = odr.side;

	auto it = order_map.find(odr.order_id);
	if(order_map.end() != it)
	{
		unsigned int ucode = it->second;

		auto itObs = obMap.find(ucode);
		if(itObs == obMap.end()){
			Log("Code = " + to_string(ucode) + "OBS not Found");
			return;
		}

		OBSetting* obs = itObs->second;
		unsigned int code = odr.code;
		if (dbp::top::order_status::rejected == status || dbp::top::order_status::canceled == status || dbp::top::order_status::deleted == status || dbp::top::order_status::filled == status)
		{
			if (dbp::top::order_side::buy == side)
			{
				if (dbp::top::order_status::filled == status)
				{


					if(odr.match_records.size() > 0){
						for(unsigned int i=0; i<odr.match_records.size(); i++){
							dbp::top::match_record mr = odr.match_records[i];
							Log("Match Price = " + to_string(mr.match_price) + " Match Qty = " + to_string(mr.match_quantity));
						}
					}

					warrant* obsw = obs->getRelatedWarrant(code);
					obsw->BuyPrice = odr.match_price;
					if(odr.match_price == 0){
						for(unsigned int i=0; i<odr.match_records.size(); i++){
							dbp::top::match_record mr = odr.match_records[i];
							Log(">>> Match Price = " + to_string(mr.match_price) + " Match Qty = " + to_string(mr.match_quantity));
							if(mr.match_price > 0){
								obsw->BuyPrice = mr.match_price;
								break;
							}

						}
					}
					obsw->BuyTime = std::string(odr.transaction_tm);
					obsw->Quantity += odr.filled_quantity;
					obsw->OrderId = odr.order_id;

					obsw->Status = STATUS_AVAILABLE;

					/*
					//unsigned long long wbest_bid_price = getBestBid(code);
					unsigned long long wbest_bid_price = warrantPriceMap[code]->Bestbid;
					//auto it2 = omdcMap.find(code);
					//if(it2 != omdcMap.end()){
						//auto wbest_bid_price = static_cast<unsigned long long>(it2->second.m_Bid[0].m_iPrice) * 100000;
						//auto wbest_ask_price = static_cast<unsigned long long>(it->second.m_Ask[0].m_iPrice) * 100000;

					PriceMark* spm = pricemarkMap[code];

					string tTable = spm->printTable(obs->StopLostPrice);

					Log("Warrant Code = " + to_string(code) + " Price Table");
					Log(tTable);
*/

					/*
					PriceMark* spm = pricemarkMap[code];
					unsigned long long pcb = spm->sellOut(wbest_bid_price);
					if(pcb == 99999999){
						pcb = obs->StopLostPrice;
					}else{
						if(pcb > obs->StopLostPrice){
							pcb = obs->StopLostPrice;
						}
					}


					obsw->StopLostPrice = pcb;
					obsw->RefWBid = wbest_bid_price;
					*/

					Log("Warrant Code = " + to_string(code) + " PCB@" + to_string(obsw->StopLostPrice));
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
					msg->wbid = obsw->RefWBid;
					msg->status = "filled";
					msg->transaction_time = obsw->BuyTime;

#ifndef NOT_MEASURE
					msg->pkg_tm = obsw->pkg_tm;
					msg->m_tm = obsw->m_tm;
					msg->t_tm = obsw->t_tm;
					msg->o_tm = dbp::tools::srv::current();
#endif


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
						msg->order_price = odr.ori_price;
						msg->order_quantity = wobs->BuyQuantity;
						msg->transaction_time = string(odr.transaction_tm);
						msg->status = "cancel";
						msg->reason = string(odr.reject_reason);

#ifndef NOT_MEASURE
					msg->pkg_tm = wobs->pkg_tm;
					msg->m_tm = wobs->m_tm;
					msg->t_tm = wobs->t_tm;
					msg->o_tm = dbp::tools::srv::current();
#endif

						ouputQueue.enqueue(msg);

						wobs->Status = STATUS_REJECTED;
						warrant* w = obs->removeWarrantOrCbbc(code);
						delete w;
						Log( "Cancelled Warrant Code = " + to_string(code) + " UCode = " + to_string(ucode));
					}

					if(obs->getRelatedWarrantCount() == 0 ){


						auto pmsg = algo_signal_msg_pool.get_obj();
						pmsg->al = this;
						pmsg->algo_name = this->_name;
						pmsg->id = this->_u.get_id();
						pmsg->ref = to_string(ucode);
						pmsg->code = ucode;
						pmsg->detect_ask = 0;
						pmsg->selected = false;
						ouputQueue.enqueue(pmsg);

						obs->detected = false;
						signalCount--;

						if(signalCount <= 0){
							lastReadyTime = 0;
							Log( "No Detected Signal ");
						}

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

					Log("Odr Match Price = " + to_string(odr.match_price) + " Odr Match Qty = " + to_string(odr.filled_quantity));
					if(odr.match_records.size() > 0){
						for(unsigned int i=0; i<odr.match_records.size(); i++){
							dbp::top::match_record mr = odr.match_records[i];

							obsw->SellPrice = mr.match_price;

							Log("Match Price = " + to_string(mr.match_price) + " Match Qty = " + to_string(mr.match_quantity));
						}
					}

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

#ifndef NOT_MEASURE
					msg->pkg_tm = obsw->pkg_tm;
					msg->m_tm = obsw->m_tm;
					msg->t_tm = obsw->t_tm;
					msg->o_tm = dbp::tools::srv::current();
#endif

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

						auto pmsg = algo_signal_msg_pool.get_obj();
						pmsg->al = this;
						pmsg->algo_name = this->_name;
						pmsg->id = this->_u.get_id();
						pmsg->ref = to_string(ucode);
						pmsg->code = ucode;
						pmsg->detect_ask = 0;
						pmsg->selected = false;
						ouputQueue.enqueue(pmsg);


						obs->hasPosition = false;
						obs->detected = false;



						signalCount--;

						if(signalCount <= 0){
							lastReadyTime = 0;
							Log( "No Detected Signal ");
						}
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
						msg->ucode = ucode;
						msg->side = "SELL";
						msg->stoplost = 0;
						msg->order_price = odr.ori_price;
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

void s1csalgo::Log(string msg){
	//fprintf(stderr, "%s %s \n",DateUtil::getCurrentTime(), msg.c_str());
	//logger

	logger->Log(string(DateUtil::getCurrentTime()) + " " + msg);

	//flush_printf("%s %s\n", DateUtil::getCurrentTime(), msg.c_str());
}

void s1csalgo::handle_command(algo_msg_base& msg)
{
	msg.on_command();
}

algo_msg_base* s1csalgo::json_to_msg(json& json)
{
	algo_marketstatus_msg* pMarketStatus_msg = nullptr;
	algo_setbet_msg* pSetBet_msg = nullptr;
	algo_issueraction_msg* pIssuerAction_msg = nullptr;
	algo_underlyingaction_msg* pUnderlyingAction_msg = nullptr;
	algo_force_sell* pforce_sell = nullptr;
	algo_issuerlist_msg* pissuerlist = nullptr;
	algo_underlyinglist_msg* punderlyinglist = nullptr;
	algo_winsell_msg* pwinsell = nullptr;
	algo_winlvlsell_msg* pwinlvlsell = nullptr;
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
		else if (cmd == "selectissuer"){
			pIssuerAction_msg = algo_issueraction_msg_pool.get_obj();
			pIssuerAction_msg->al = this;
			pIssuerAction_msg->algo_name = _name;
			pIssuerAction_msg->id = _u.get_id();
			pIssuerAction_msg->ref = ref;
			pIssuerAction_msg->issuer = json["issuer"].get<std::string>();
			pIssuerAction_msg->action = json["action"].get<std::string>();
			return pIssuerAction_msg;
		}
		else if (cmd == "selectunderlying"){
			pUnderlyingAction_msg = algo_underlyingaction_msg_pool.get_obj();
			pUnderlyingAction_msg->al = this;
			pUnderlyingAction_msg->algo_name = _name;
			pUnderlyingAction_msg->id = _u.get_id();
			pUnderlyingAction_msg->ref = ref;
			pUnderlyingAction_msg->ucode = json["ucode"].get<unsigned int>();
			pUnderlyingAction_msg->action = json["action"].get<std::string>();
			return pUnderlyingAction_msg;
		}
		else if (cmd == "issuerlist"){
			pissuerlist = algo_issuerlist_msg_pool.get_obj();
			pissuerlist->al = this;
			pissuerlist->algo_name = _name;
			pissuerlist->id = _u.get_id();
			pissuerlist->ref = ref;
			return pissuerlist;
		}
		else if (cmd == "underlyinglist"){
			punderlyinglist = algo_underlyinglist_msg_pool.get_obj();
			punderlyinglist->al = this;
			punderlyinglist->algo_name = _name;
			punderlyinglist->id = _u.get_id();
			punderlyinglist->ref = ref;
			return punderlyinglist;
		}
		else if(cmd == "force_sell")
		{
			pforce_sell = algo_force_sell_pool.get_obj();
			pforce_sell->al = this;
			pforce_sell->algo_name = _name;
			pforce_sell->id = _u.get_id();
			pforce_sell->ref = ref;
			pforce_sell->ucode = json["ucode"].get<unsigned int>();
			pforce_sell->code = json["code"].get<unsigned int>();
			pforce_sell->price = json["price"].get<unsigned long long>();
			return pforce_sell;
		}
		else if(cmd == "winsell")
		{
			pwinsell = algo_winsell_msg_pool.get_obj();
			pwinsell->al = this;
			pwinsell->algo_name = _name;
			pwinsell->id = _u.get_id();
			pwinsell->ref = ref;
			pwinsell->ucode = json["ucode"].get<unsigned int>();
			pwinsell->wcode = json["code"].get<unsigned int>();
			pwinsell->action = json["action"].get<std::string>();
			return pwinsell;
		}
		else if(cmd == "winlvlsell")
		{
			pwinlvlsell = algo_winlvlsell_msg_pool.get_obj();
			pwinlvlsell->al = this;
			pwinlvlsell->algo_name = _name;
			pwinlvlsell->id = _u.get_id();
			pwinlvlsell->ref = ref;
			pwinlvlsell->ucode = json["ucode"].get<unsigned int>();
			pwinlvlsell->wcode = json["code"].get<unsigned int>();
			pwinlvlsell->action = json["action"].get<std::string>();
			return pwinlvlsell;
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
		if (pforce_sell)
			pforce_sell->release();
		if (pSetBet_msg)
			pSetBet_msg->release();
		if(pIssuerAction_msg)
			pIssuerAction_msg->release();
		if(pissuerlist)
			pissuerlist->release();
		if(punderlyinglist)
			punderlyinglist->release();
		if(pUnderlyingAction_msg)
			pUnderlyingAction_msg->release();
		if(pwinsell)
			pwinsell->release();
		if(pwinlvlsell)
			pwinlvlsell->release();
		return msg;
	}
}

std::string s1csalgo::get_lib_name()
{
	return "s1csalgo";
}

rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_err_msg, 8192> s1csalgo::algo_err_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<s1csalgo::algo_marketstatus_msg, 8192> s1csalgo::algo_marketstatus_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<s1csalgo::algo_setbet_msg, 8192> s1csalgo::algo_setbet_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<s1csalgo::algo_issueraction_msg, 8192> s1csalgo::algo_issueraction_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<s1csalgo::algo_underlyingaction_msg, 8192> s1csalgo::algo_underlyingaction_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_order_msg, 8192> s1csalgo::algo_order_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_portfolio_msg, 8192> s1csalgo::algo_portfolio_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_signal_msg, 8192> s1csalgo::algo_signal_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_stoplost_msg, 8192> s1csalgo::algo_stoplost_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_winsell_msg, 8192> s1csalgo::algo_winsell_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_winlvlsell_msg, 8192> s1csalgo::algo_winlvlsell_msg_pool;
rapid_ring::spsc_ring_buffer_object_pool<s1csalgo::algo_force_sell, 8192> s1csalgo::algo_force_sell_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_warrantprice_msg, 8192> s1csalgo::algo_warrantprice_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_issuerlist_msg, 8192> s1csalgo::algo_issuerlist_msg_pool;
rapid_ring::spmc_ring_buffer_object_pool<s1csalgo::algo_underlyinglist_msg, 8192> s1csalgo::algo_underlyinglist_msg_pool;
