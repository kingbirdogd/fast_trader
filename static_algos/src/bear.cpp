#include <bear.hpp>

#include <vector>

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
	logger = new ThreadLogger("log/" + name + DateUtil::getToday() + ".log");
	logger->start();
}

void bear::on_omdc_book(const Tradable& tradable)
{

	auto uit = uprice_map.find(tradable.m_Code);
	if(uprice_map.end() != uit){


		//unsigned int code = tradable.m_Code;

		auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
		auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
		auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
		auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;


		/*
		auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
		auto best_bid1_price = static_cast<unsigned long long>(tradable.m_Bid[1].m_iPrice) * 100000;
		auto best_bid2_price = static_cast<unsigned long long>(tradable.m_Bid[2].m_iPrice) * 100000;
		auto best_bid3_price = static_cast<unsigned long long>(tradable.m_Bid[3].m_iPrice) * 100000;

		auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
		auto best_bid1_qty = tradable.m_Bid[1].m_uQuantity;
		auto best_bid2_qty = tradable.m_Bid[2].m_uQuantity;
		auto best_bid3_qty = tradable.m_Bid[3].m_uQuantity;
		auto best_bid4_qty = tradable.m_Bid[4].m_uQuantity;


		auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
		auto best_ask1_price = static_cast<unsigned long long>(tradable.m_Ask[1].m_iPrice) * 100000;
		auto best_ask2_price = static_cast<unsigned long long>(tradable.m_Ask[2].m_iPrice) * 100000;
		auto best_ask3_price = static_cast<unsigned long long>(tradable.m_Ask[3].m_iPrice) * 100000;

		auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
		auto best_ask1_qty = tradable.m_Ask[1].m_uQuantity;
		auto best_ask2_qty = tradable.m_Ask[2].m_uQuantity;
		auto best_ask3_qty = tradable.m_Ask[3].m_uQuantity;
		auto best_ask4_qty = tradable.m_Ask[4].m_uQuantity;

		unsigned long long average2 = (best_bid_qty + best_bid1_qty + best_ask_qty + best_ask1_qty)/2;
		unsigned long long average5 = (best_bid_qty + best_bid1_qty + best_bid2_qty + best_bid3_qty + best_bid4_qty +
				best_ask_qty + best_ask1_qty + best_ask2_qty + best_ask3_qty + best_ask4_qty)/2;


		float W1 = 5.0;
		float W2 = 3.0;
		float W3 = 1.5;
		float W4 = 0.5;

		vector<unsigned long long> values;
		values.push_back(0);
		values.push_back(average2);
		values.push_back(average5);

		unsigned long long quotevol = static_cast<unsigned long long>(CalcMedian(values));

		unsigned long long accbq1 = best_bid_qty;
		unsigned long long accbq2 = best_bid1_qty + accbq1;
		unsigned long long accbq3 = best_bid2_qty + accbq2;
		unsigned long long accbq4 = best_bid3_qty + accbq3;

		unsigned long long accaq1 = best_ask_qty;
		unsigned long long accaq2 = best_ask1_qty + accaq1;
		unsigned long long accaq3 = best_ask2_qty + accaq2;
		unsigned long long accaq4 = best_ask3_qty + accaq3;


		unsigned long long bq1 = calBidAskQuantity(accbq1, 0, quotevol, best_bid_qty, accbq1);
		unsigned long long bq2 = calBidAskQuantity(accbq2, accbq1, quotevol, best_bid1_qty, quotevol-accbq1);
		unsigned long long bq3 = calBidAskQuantity(accbq3, accbq2, quotevol, best_bid2_qty, quotevol-accbq2);
		unsigned long long bq4 = calBidAskQuantity(accbq4, accbq3, quotevol, best_bid3_qty, quotevol-accbq3);

		unsigned long long aq1 = calBidAskQuantity(accaq1, 0, quotevol, best_ask_qty, accaq1);
		unsigned long long aq2 = calBidAskQuantity(accaq2, accaq1, quotevol, best_ask1_qty, quotevol-accaq1);
		unsigned long long aq3 = calBidAskQuantity(accaq3, accaq2, quotevol, best_ask2_qty, quotevol-accaq2);
		unsigned long long aq4 = calBidAskQuantity(accaq4, accaq3, quotevol, best_ask3_qty, quotevol-accaq3);


		unsigned long long wbp1 = static_cast<unsigned long long>(best_bid_price*W1*bq1);
		unsigned long long wbp2 = static_cast<unsigned long long>(best_bid1_price*W2*bq2);
		unsigned long long wbp3 = static_cast<unsigned long long>(best_bid2_price*W3*bq3);
		unsigned long long wbp4 = static_cast<unsigned long long>(best_bid3_price*W4*bq4);

		unsigned long long wap1 = static_cast<unsigned long long>(best_ask_price*W1*aq1);
		unsigned long long wap2 = static_cast<unsigned long long>(best_ask1_price*W2*aq2);
		unsigned long long wap3 = static_cast<unsigned long long>(best_ask2_price*W3*aq3);
		unsigned long long wap4 = static_cast<unsigned long long>(best_ask3_price*W4*aq4);


		unsigned long long wbq1 = static_cast<unsigned long long>(bq1*W1);
		unsigned long long wbq2 = static_cast<unsigned long long>(bq2*W2);
		unsigned long long wbq3 = static_cast<unsigned long long>(bq3*W3);
		unsigned long long wbq4 = static_cast<unsigned long long>(bq4*W4);

		unsigned long long waq1 = static_cast<unsigned long long>(aq1*W1);
		unsigned long long waq2 = static_cast<unsigned long long>(aq2*W2);
		unsigned long long waq3 = static_cast<unsigned long long>(aq3*W3);
		unsigned long long waq4 = static_cast<unsigned long long>(aq4*W4);

		unsigned long long wb =  static_cast<unsigned long long>((wbp1 + wbp2 + wbp3 + wbp4)/(wbq1 + wbq2 + wbq3 + wbq4));
		unsigned long long wa = static_cast<unsigned long long>((wap1 + wap2 + wap3 + wap4)/(waq1 + waq2 + waq3 + waq4));

		unsigned long long bremainder = wb%100000;
		unsigned long long aremainder = wa%100000;

		unsigned long long swb = wb - bremainder;
		unsigned long long swa = wa - aremainder;
*/

		//Log("Data = " + to_string(tradable.m_Code) + " ----- " + to_string(best_bid_price) + ":" + to_string(best_ask_price));

		if(uit->second->FBestbid != best_bid_price){
		//if(uit->second->FBestbid != swb){

			//Log("UCODE  = " + to_string(tradable.m_Code) + " Bid Change from  " + to_string(uit->second->FBestbid) + " To  " + to_string(best_bid_price));
			uit->second->BidSeq++;
			uit->second->PFBestbid = uit->second->FBestbid;
			//uit->second->FBestbid = swb;
			uit->second->FBestbid = best_bid_price;

		}
		if(uit->second->FBestask != best_ask_price ){
		//if(uit->second->FBestask != swa ){


			//Log("UCODE  = " + to_string(tradable.m_Code) + " Ask Change from  " + to_string(uit->second->FBestask) + " To  " + to_string(best_ask_price));
			uit->second->AskSeq++;
			uit->second->PFBestask = uit->second->FBestask;
			//uit->second->FBestask = swa;
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

		return;
	}


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

void bear::on_omdc_trade(const Tradable& tradable)
{
	/*
	auto it = _w_map.find(tradable.m_Code);
	if (_w_map.end() != it)
	{
		auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;
		auto side = tradable.m_TradeSide;

		it->second->Log(std::to_string(tradable.m_Code) + " Side=" + std::to_string(side) + " Price=" + std::to_string(trade_price) + " Qty=" + std::to_string(tradable.m_LastTradeQuantity) + " Buyin=" + to_string(it->second->getBuyIn()) + " Sellout=" + std::to_string(it->second->getSellOut()) );

	}*/

	auto it = _u_map.find(tradable.m_Code);
	if (_u_map.end() != it)
	{

		priceinfo* uprice = uprice_map[tradable.m_Code];

/*
		auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
		auto best_bid1_price = static_cast<unsigned long long>(tradable.m_Bid[1].m_iPrice) * 100000;
		auto best_bid2_price = static_cast<unsigned long long>(tradable.m_Bid[2].m_iPrice) * 100000;
		auto best_bid3_price = static_cast<unsigned long long>(tradable.m_Bid[3].m_iPrice) * 100000;

		auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
		auto best_bid1_qty = tradable.m_Bid[1].m_uQuantity;
		auto best_bid2_qty = tradable.m_Bid[2].m_uQuantity;
		auto best_bid3_qty = tradable.m_Bid[3].m_uQuantity;
		auto best_bid4_qty = tradable.m_Bid[4].m_uQuantity;


		//auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
		auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
		auto best_ask1_price = static_cast<unsigned long long>(tradable.m_Ask[1].m_iPrice) * 100000;
		auto best_ask2_price = static_cast<unsigned long long>(tradable.m_Ask[2].m_iPrice) * 100000;
		auto best_ask3_price = static_cast<unsigned long long>(tradable.m_Ask[3].m_iPrice) * 100000;

		auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
		auto best_ask1_qty = tradable.m_Ask[1].m_uQuantity;
		auto best_ask2_qty = tradable.m_Ask[2].m_uQuantity;
		auto best_ask3_qty = tradable.m_Ask[3].m_uQuantity;
		auto best_ask4_qty = tradable.m_Ask[4].m_uQuantity;

		unsigned long long average2 = (best_bid_qty + best_bid1_qty + best_ask_qty + best_ask1_qty)/2;
		unsigned long long average5 = (best_bid_qty + best_bid1_qty + best_bid2_qty + best_bid3_qty + best_bid4_qty +
				best_ask_qty + best_ask1_qty + best_ask2_qty + best_ask3_qty + best_ask4_qty)/2;


		float W1 = 5.0;
		float W2 = 3.0;
		float W3 = 1.5;
		float W4 = 0.5;

		vector<unsigned long long> values;
		values.push_back(0);
		values.push_back(average2);
		values.push_back(average5);

		unsigned long long quotevol = static_cast<unsigned long long>(CalcMedian(values));

		unsigned long long accbq1 = best_bid_qty;
		unsigned long long accbq2 = best_bid1_qty + accbq1;
		unsigned long long accbq3 = best_bid2_qty + accbq2;
		unsigned long long accbq4 = best_bid3_qty + accbq3;

		unsigned long long accaq1 = best_ask_qty;
		unsigned long long accaq2 = best_ask1_qty + accaq1;
		unsigned long long accaq3 = best_ask2_qty + accaq2;
		unsigned long long accaq4 = best_ask3_qty + accaq3;


		unsigned long long bq1 = calBidAskQuantity(accbq1, 0, quotevol, best_bid_qty, accbq1);
		unsigned long long bq2 = calBidAskQuantity(accbq2, accbq1, quotevol, best_bid1_qty, quotevol-accbq1);
		unsigned long long bq3 = calBidAskQuantity(accbq3, accbq2, quotevol, best_bid2_qty, quotevol-accbq2);
		unsigned long long bq4 = calBidAskQuantity(accbq4, accbq3, quotevol, best_bid3_qty, quotevol-accbq3);

		unsigned long long aq1 = calBidAskQuantity(accaq1, 0, quotevol, best_ask_qty, accaq1);
		unsigned long long aq2 = calBidAskQuantity(accaq2, accaq1, quotevol, best_ask1_qty, quotevol-accaq1);
		unsigned long long aq3 = calBidAskQuantity(accaq3, accaq2, quotevol, best_ask2_qty, quotevol-accaq2);
		unsigned long long aq4 = calBidAskQuantity(accaq4, accaq3, quotevol, best_ask3_qty, quotevol-accaq3);


		unsigned long long wbp1 = static_cast<unsigned long long>(best_bid_price*W1*bq1);
		unsigned long long wbp2 = static_cast<unsigned long long>(best_bid1_price*W2*bq2);
		unsigned long long wbp3 = static_cast<unsigned long long>(best_bid2_price*W3*bq3);
		unsigned long long wbp4 = static_cast<unsigned long long>(best_bid3_price*W4*bq4);

		unsigned long long wap1 = static_cast<unsigned long long>(best_ask_price*W1*aq1);
		unsigned long long wap2 = static_cast<unsigned long long>(best_ask1_price*W2*aq2);
		unsigned long long wap3 = static_cast<unsigned long long>(best_ask2_price*W3*aq3);
		unsigned long long wap4 = static_cast<unsigned long long>(best_ask3_price*W4*aq4);


		unsigned long long wbq1 = static_cast<unsigned long long>(bq1*W1);
		unsigned long long wbq2 = static_cast<unsigned long long>(bq2*W2);
		unsigned long long wbq3 = static_cast<unsigned long long>(bq3*W3);
		unsigned long long wbq4 = static_cast<unsigned long long>(bq4*W4);

		unsigned long long waq1 = static_cast<unsigned long long>(aq1*W1);
		unsigned long long waq2 = static_cast<unsigned long long>(aq2*W2);
		unsigned long long waq3 = static_cast<unsigned long long>(aq3*W3);
		unsigned long long waq4 = static_cast<unsigned long long>(aq4*W4);

		unsigned long long wb =  static_cast<unsigned long long>((wbp1 + wbp2 + wbp3 + wbp4)/(wbq1 + wbq2 + wbq3 + wbq4));
		unsigned long long wa = static_cast<unsigned long long>((wap1 + wap2 + wap3 + wap4)/(waq1 + waq2 + waq3 + waq4));

		unsigned long long bremainder = wb%100000;
		unsigned long long aremainder = wa%100000;

		unsigned long long swb = wb - bremainder;
		unsigned long long swa = wa - aremainder;
*/



		auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
		auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
		auto best_bid_price1 = static_cast<unsigned long long>(tradable.m_Bid[1].m_iPrice) * 100000;
		auto best_ask_price1 = static_cast<unsigned long long>(tradable.m_Ask[1].m_iPrice) * 100000;
		auto best_bid_qty = static_cast<unsigned long long>(tradable.m_Bid[0].m_uQuantity);
		auto best_ask_qty = static_cast<unsigned long long>(tradable.m_Ask[0].m_uQuantity);


		//auto trade_quantity = tradable.m_uAccumulatedQuantity;
		auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;
		//Log(" Code = " + to_string(tradable.m_Code) + " START ");
		//Log(" Code = " + to_string(tradable.m_Code) + " Best Bid  = " + to_string(best_bid_price) + " Best Ask  = " + to_string(best_ask_price));
		//Log(" Code = " + to_string(tradable.m_Code) + " Best Bid1 = " + to_string(best_bid_price1) + " Best Ask1 = " + to_string(best_ask_price1));
		//Log(" Code = " + to_string(tradable.m_Code) + " Best Bid Qty = " + to_string(best_bid_qty) + " Best Ask Qty = " + to_string(best_ask_qty));

		/*
		if(tradable.m_TradeSide == BUY_ORDER){
			Log(" Code = " + to_string(tradable.m_Code) + " BID SIDE TRADE = 1");
		}else{
			Log(" Code = " + to_string(tradable.m_Code) + " ASK SIDE TRADE = -1");
		}
		*/


































		if(BUY_ORDER == tradable.m_TradeSide){


			auto trade_quantity = static_cast<unsigned long long>(tradable.m_AccumulateSellQuantity);

			Log(" Code = " + to_string(tradable.m_Code) + " Trade Price = " + to_string(trade_price) + " tradable.m_TradeSide = BUY " + " Qty = " + to_string(trade_quantity));

			if(uprice->TBestbid != trade_price){
				uprice->PTBestbid = uprice->TBestbid;
				uprice->TBestbid = trade_price;
			}


			if (trade_price == best_bid_price && trade_quantity >= tradable.m_Bid[0].m_uQuantity &&	0 != tradable.m_Bid[0].m_uQuantity )
			{
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						if((p->getSellOut() == trade_price) || (best_bid_price > p->getSellOut() && p->getSellOut() > best_bid_price1))
						{
							p->on_bull_trade(tradable);
						}
					}else{
						if(p->getBuyIn() == trade_price && (p->status() == STATUS_READY || p->status() == STATUS_DONE) && p->action_status() == STAGE_START)
						{
							p->on_bear_trade(tradable);
						}
					}

				}
			}else{
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						if((p->getSellOut() == trade_price || (best_bid_price > p->getSellOut() && p->getSellOut() > best_bid_price1)) && uprice->PTBestbid > trade_price && p->has_position())
						{
							p->doSellLevel();
						}
					}
				}
			}

		}

		if(SELL_ORDER == tradable.m_TradeSide){
			auto trade_quantity = static_cast<unsigned long long>(tradable.m_AccumulateBuyQuantity);

			Log(" Code = " + to_string(tradable.m_Code) + " Trade Price = " + to_string(trade_price) + " tradable.m_TradeSide = SELL " + " Qty = " + to_string(trade_quantity));


			if(uprice->TBestask != trade_price ){
				uprice->PTBestask = uprice->TBestask;
				uprice->TBestask = trade_price;
			}

			if (trade_price == best_ask_price && trade_quantity >= tradable.m_Ask[0].m_uQuantity &&	0 != tradable.m_Ask[0].m_uQuantity )
			{
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){

						Log(" Code = " + to_string(tradable.m_Code) + " Trade Price = " + to_string(trade_price) + " Buyin = " + to_string(p->getBuyIn()) + " sTATUS = " + to_string(p->status()));
						if(p->getBuyIn() == trade_price  && (p->status() == STATUS_READY || p->status() == STATUS_DONE) && p->action_status() == STAGE_START)
						{
							p->on_bull_trade(tradable);
						}
					}else{
						if(p->getSellOut() == trade_price || (best_ask_price < p->getSellOut() && p->getSellOut() < best_ask_price1))
						{
							p->on_bear_trade(tradable);
						}
					}
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

void bear::on_omdd_book(const Tradable& tradable)
{

	//Log("Data = " + to_string(tradable.m_Code) + " Bid = " + to_string(tradable.m_Bid[0].m_iPrice) + " Ask = " + to_string(tradable.m_Ask[0].m_iPrice));

	auto uit = uprice_map.find(tradable.m_Code);
	if(uprice_map.end() != uit){


		//unsigned int code = tradable.m_Code;

		//auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;
		auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;

		//auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
		auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;

		//Log("Data = " + to_string(tradable.m_Code) + " ----- " + to_string(best_bid_price) + ":" + to_string(best_ask_price));

		if(uit->second->FBestbid != best_bid_price){

			//Log("UCODE  = " + to_string(tradable.m_Code) + " Bid Change from  " + to_string(uit->second->FBestbid) + " To  " + to_string(best_bid_price));
			uit->second->BidSeq++;
			uit->second->PFBestbid = uit->second->FBestbid;
			uit->second->FBestbid = best_bid_price;

		}
		if(uit->second->FBestask != best_ask_price ){


			//Log("UCODE  = " + to_string(tradable.m_Code) + " Ask Change from  " + to_string(uit->second->FBestask) + " To  " + to_string(best_ask_price));
			uit->second->AskSeq++;
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

void bear::on_tcp_book(const Tradable& tradable)
{

	string code(tradable.m_TcpCode);
	//rtrim(code);


	//Log("on_tcp_book symbol = >>>" + code + "<<<");

	auto uit = rprice_map.find(code);
	//auto uit = rprice_map.find("NQZ0");
	if(rprice_map.end() != uit){

		//unsigned int code = tradable.m_Code;

		//auto best_bid_qty = tradable.m_Bid[0].m_uQuantity;


		//auto best_ask_qty = tradable.m_Ask[0].m_uQuantity;
		auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice)*100000;
		auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice)*100000;

		//Log(" Code = " + code + " Best Bid = " + to_string(best_bid_price) + " Best Ask = " + to_string(best_ask_price));


		if(uit->second->FBestbid != best_bid_price){
			uit->second->BidSeq++;
			uit->second->PFBestbid = uit->second->FBestbid;
			uit->second->FBestbid = best_bid_price;
		}
		if(uit->second->FBestask != best_ask_price ){
			uit->second->AskSeq++;
			uit->second->PFBestask = uit->second->FBestask;
			uit->second->FBestask = best_ask_price;
		}

		auto it = _ru_map.find(code);
		if (_ru_map.end() != it)
		{
			for (const auto& p : it->second)
			{
				if(p->action_status() == STAGE_START && p->has_position()){
					p->on_tcp_book(tradable);
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
		auto best_bid_qty = static_cast<unsigned long long>(tradable.m_Bid[0].m_uQuantity);
		auto best_ask_qty = static_cast<unsigned long long>(tradable.m_Ask[0].m_uQuantity);

		//auto trade_quantity = tradable.m_uAccumulatedQuantity;
		auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;
		/*
		Log(" Code = " + to_string(tradable.m_Code) + " START ");
		Log(" Code = " + to_string(tradable.m_Code) + " Best Bid  = " + to_string(best_bid_price) + " Best Ask  = " + to_string(best_ask_price));
		Log(" Code = " + to_string(tradable.m_Code) + " Best Bid1 = " + to_string(best_bid_price1) + " Best Ask1 = " + to_string(best_ask_price1));
		Log(" Code = " + to_string(tradable.m_Code) + " Best Bid Qty = " + to_string(best_bid_qty) + " Best Ask Qty = " + to_string(best_ask_qty));

		if(tradable.m_TradeSide == BUY_ORDER){
			Log(" Code = " + to_string(tradable.m_Code) + " BID SIDE TRADE = 1");
		}else{
			Log(" Code = " + to_string(tradable.m_Code) + " ASK SIDE TRADE = -1");
		}
		*/


		if(BUY_ORDER == tradable.m_TradeSide){


			auto trade_quantity = static_cast<unsigned long long>(tradable.m_AccumulateSellQuantity);

			Log(" Code = " + to_string(tradable.m_Code) + " Trade Price = " + to_string(trade_price) + " tradable.m_TradeSide = BUY " + " Qty = " + to_string(trade_quantity));

			if(uprice->TBestbid != trade_price){
				uprice->PTBestbid = uprice->TBestbid;
				uprice->TBestbid = trade_price;
			}


			if (trade_price == best_bid_price && trade_quantity >= tradable.m_Bid[0].m_uQuantity &&	0 != tradable.m_Bid[0].m_uQuantity )
			{
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						if((p->getLvlBid() == trade_price && p->isLevel()) || (p->getSellOut() == trade_price) || (best_bid_price > p->getSellOut() && p->getSellOut() > best_bid_price1))
						{
							p->on_bull_trade(tradable);
						}
					}else{
						if(p->getBuyIn() == trade_price && (p->status() == STATUS_READY || p->status() == STATUS_DONE) && p->action_status() == STAGE_START)
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
				/*
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						if((p->getSellOut() == trade_price || (best_bid_price > p->getSellOut() && p->getSellOut() > best_bid_price1)) && uprice->PTBestbid > trade_price && p->has_position())
						{
							p->doSellLevel();
						}
					}
				}*/
			}

		}

		if(SELL_ORDER == tradable.m_TradeSide){
			auto trade_quantity = static_cast<unsigned long long>(tradable.m_AccumulateBuyQuantity);

			Log(" Code = " + to_string(tradable.m_Code) + " Trade Price = " + to_string(trade_price) + " tradable.m_TradeSide = SELL " + " Qty = " + to_string(trade_quantity));


			if(uprice->TBestask != trade_price ){
				uprice->PTBestask = uprice->TBestask;
				uprice->TBestask = trade_price;
			}

			if (trade_price == best_ask_price && trade_quantity >= tradable.m_Ask[0].m_uQuantity &&	0 != tradable.m_Ask[0].m_uQuantity )
			{
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){

						Log(" Code = " + to_string(tradable.m_Code) + " Trade Price = " + to_string(trade_price) + " Buyin = " + to_string(p->getBuyIn()) + " sTATUS = " + to_string(p->status()));
						if(p->getBuyIn() == trade_price  && (p->status() == STATUS_READY || p->status() == STATUS_DONE) && p->action_status() == STAGE_START)
						{
							p->on_bull_trade(tradable);
						}
					}else{
						if((p->getLvlBid() == trade_price && p->isLevel()) || (p->getSellOut() == trade_price) || (best_ask_price < p->getSellOut() && p->getSellOut() < best_ask_price1))
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
				/*
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BEAR){
						if((p->getSellOut() == trade_price  || (best_ask_price < p->getSellOut() && p->getSellOut() < best_ask_price1) ) && uprice->PTBestask < trade_price && p->has_position())
						{
							p->doSellLevel();
						}
					}

				}*/
			}

		}

	}
}

void bear::on_tcp_trade(const Tradable& tradable)
{

	//Log("on_tcp_trade symbol = " + string(tradable.m_TcpCode));

	string code(tradable.m_TcpCode);
	//rtrim(code);
	//Log("on_tcp trade symbol = >>>" + code + "<<<");
	auto it = _ru_map.find(tradable.m_TcpCode);
	if (_ru_map.end() != it)
	{

		priceinfo* uprice = rprice_map[code];


		auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice)*100000;
		auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice)*100000;
		auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice)*100000;


		//auto best_bid_price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
		//auto best_ask_price = static_cast<unsigned long long>(tradable.m_Ask[0].m_iPrice) * 100000;
		//auto trade_price = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;


		//auto best_bid_price1 = static_cast<unsigned long long>(tradable.m_Bid[1].m_iPrice) * 100000;
		//auto best_ask_price1 = static_cast<unsigned long long>(tradable.m_Ask[1].m_iPrice) * 100000;

		//auto trade_quantity = tradable.m_uAccumulatedQuantity;

		//auto trade_q = static_cast<unsigned long long>(tradable.m_LastTradePrice) * 100000;


		if(BUY_ORDER == tradable.m_TradeSide){
			auto trade_quantity = static_cast<unsigned long long>(tradable.m_LastTradeQuantity);

			if(uprice->TBestbid != trade_price){
				uprice->PTBestbid = uprice->TBestbid;
				uprice->TBestbid = trade_price;
				uprice->TBidSeq++;
			}


			if (trade_price == best_bid_price && trade_quantity >= tradable.m_Bid[0].m_uQuantity &&	0 != tradable.m_Bid[0].m_uQuantity )
			{
				/*
				Log(code + " BestBid = " + to_string(best_bid_price) + "(" + to_string(tradable.m_Bid[0].m_uQuantity)+ ")" +
						   " BestAsk = " + to_string(best_ask_price) + "(" + to_string(tradable.m_Ask[0].m_uQuantity)+ ")"
						);
				Log(code + "BID Trade Price = " + to_string(trade_price) + " Qty = " + to_string(trade_quantity));
*/
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						//if((p->getSellOut() == trade_price) || (best_bid_price > p->getSellOut() && p->getSellOut() > best_bid_price1))
						if((p->getSellOut() == trade_price))
						{
							p->on_bull_trade(tradable);
						}
					}else{
						if(p->getBuyIn() == trade_price)
						{
							p->on_bear_trade(tradable);
						}
					}
				}
			}else{
				/*
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						if((p->getSellOut() == trade_price || (best_bid_price > p->getSellOut() && p->getSellOut() > best_bid_price1)) && uprice->PTBestbid > trade_price && p->has_position())
						{
							p->doSellLevel();
						}
					}
				}
				*/
			}

		}

		if(SELL_ORDER == tradable.m_TradeSide){
			auto trade_quantity = static_cast<unsigned long long>(tradable.m_LastTradeQuantity);

			if(uprice->TBestask != trade_price ){
				uprice->PTBestask = uprice->TBestask;
				uprice->TBestask = trade_price;
				uprice->TAskSeq++;
			}

			if (trade_price == best_ask_price && trade_quantity >= tradable.m_Ask[0].m_uQuantity &&	0 != tradable.m_Ask[0].m_uQuantity )
			{

/*
				Log(code + " BestBid = " + to_string(best_bid_price) + "(" + to_string(tradable.m_Bid[0].m_uQuantity)+ ")" +
						   " BestAsk = " + to_string(best_ask_price) + "(" + to_string(tradable.m_Ask[0].m_uQuantity)+ ")"
						);
				Log(code + "ASK Trade Price = " + to_string(trade_price) + " Qty = " + to_string(trade_quantity));
*/
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BULL){
						if(p->getBuyIn() == trade_price && p->status() == STATUS_READY && p->action_status() == STAGE_START)
						{
							p->on_bull_trade(tradable);
						}
					}else{
						//if(p->getSellOut() == trade_price || (best_ask_price < p->getSellOut() && p->getSellOut() < best_ask_price1))
						if(p->getSellOut() == trade_price)
						{
							p->on_bear_trade(tradable);
						}
					}
				}
			}else{
				/*
				for (const auto& p : it->second)
				{

					if(p->getWtype() == BEAR){
						if((p->getSellOut() == trade_price  || (best_ask_price < p->getSellOut() && p->getSellOut() < best_ask_price1) ) && uprice->PTBestask < trade_price && p->has_position())
						{
							p->doSellLevel();
						}
					}

				}*/
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
			Log("Duplication Warrant Code");
			return "Duplicate Warrant";
		}else{
			_w_ref_map.erase(p.warrant_code());
			Log("remove Old entry Warrant Code");
		}
	}




	Log("set_pair 1 " + ref);

	if (_p_map.end() != it)
	{
		if ((it->second.commodity_symbol() != p.commodity_symbol())	|| (it->second.warrant_code() != p.warrant_code()))
		{

			Log("set_pair 1 Code Exist = " + to_string(p.warrant_code()));
			auto symbol = it->second.commodity_symbol();
			auto warrant_code = it->second.warrant_code();
			auto underlying_code = it->second.underlying_code();

			auto utype = it->second.getUtype();


			Log("OLD Pair 1 Symbol = " + symbol + " Warrant Code= " + to_string(warrant_code) + " Underlying Code= " + to_string(underlying_code));

			auto node = &(it->second);

			if(utype == HSI_SYMBOL){

				auto u_it = _u_map.find(underlying_code);
				if (_u_map.end() != u_it)
				{
					if (u_it->second.end() != u_it->second.find(node))
					{
						u_it->second.erase(node);
						if(u_it->second.empty())
						{
							_u_map.erase(u_it);
							Log("set_pair 1 Delete  Exist = " + to_string(p.warrant_code()));
							//subscribe_omdd_trade(underlying_code, false);
							//subscribe_omdd_book(underlying_code, false);
						}
					}
				}
			}else if (utype == NQ_SYMBOL){
				auto u_it = _ru_map.find(symbol);
				if (_ru_map.end() != u_it)
				{
					if (u_it->second.end() != u_it->second.find(node))
					{
						u_it->second.erase(node);
						if(u_it->second.empty())
						{
							_ru_map.erase(u_it);
							Log("set_pair RU 1 Delete  Exist = " + to_string(p.warrant_code()));
						}
					}
				}
			}else if(utype == ST_SYMBOL){
				auto u_it = _u_map.find(underlying_code);
				if (_u_map.end() != u_it)
				{
					if (u_it->second.end() != u_it->second.find(node))
					{
						u_it->second.erase(node);
						if(u_it->second.empty())
						{
							_u_map.erase(u_it);
							Log("set_pair 1 Delete  Exist = " + to_string(p.warrant_code()));
							//subscribe_omdd_trade(underlying_code, false);
							//subscribe_omdd_book(underlying_code, false);
						}
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

	Log("set_pair 2");

	auto symbol = it->second.commodity_symbol();
	auto underlying_code = it->second.underlying_code();
	auto warrant_code = it->second.warrant_code();
	auto utype = it->second.getUtype();
	//json["result"] = "SUCCESS";
	//send_out(json.dump());
	auto node = &(it->second);
	if(utype == HSI_SYMBOL){
		_u_map[underlying_code].insert(node);
	}else if(utype == NQ_SYMBOL){
		_ru_map[symbol].insert(node);
	}else if(utype == ST_SYMBOL){
		_u_map[underlying_code].insert(node);
	}
	_w_map[warrant_code] = node;

	_w_ref_map[warrant_code] = it->second.ref();



	Log("NEW Pair 1 Symbol = >>>" + symbol + "<<< Warrant Code= " + to_string(warrant_code ) + " Underlying Code= " + to_string(underlying_code) + " UType = " + to_string(utype));

	if(utype == HSI_SYMBOL){
		auto itu = uprice_map.find(underlying_code);
		if(itu  == uprice_map.end()){
			Log("Init HSI Priceinfo : Orderbook ID = >>>" +to_string(underlying_code)+"<<<" );
			uprice_map[underlying_code] = std::move(new priceinfo());
			uprice_map[underlying_code]->BidSeq = 0;
			uprice_map[underlying_code]->AskSeq = 0;
		}
	}else if (utype == NQ_SYMBOL){
		auto itu = rprice_map.find(symbol);
		if(itu  == rprice_map.end()){
			Log("Init NQ Priceinfo : Symbol = >>>" +symbol+"<<<" );

			//string a = "NQZ0";

			rprice_map[symbol] = std::move(new priceinfo());
			rprice_map[symbol]->BidSeq = 0;
			rprice_map[symbol]->AskSeq = 0;
		}
	}else if(utype == ST_SYMBOL){
		auto itu = uprice_map.find(underlying_code);
		if(itu  == uprice_map.end()){
			Log("Init Stock Priceinfo : Underlying Code = >>>" +to_string(underlying_code)+"<<<" );
			uprice_map[underlying_code] = std::move(new priceinfo());
			uprice_map[underlying_code]->BidSeq = 0;
			uprice_map[underlying_code]->AskSeq = 0;
		}
	}

	Log("set_pair 3");

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

	Log("do set_start");

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

	if(p.action_status() == STAGE_NEW){
		load_pricetable(code, ref);
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

	Log("do set_pause");
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

	Log("do set_stop");

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

	Log("do set_param");

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
	}else if(type == "BUYOFFSET"){
		int _ivalue = static_cast<int>(ivalue);
		p.set_BuyOffset(_ivalue);
	}else if(type == "SELLOFFSET"){
		int _ivalue = static_cast<int>(ivalue);
		p.set_SellOffset(_ivalue);
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

	//p.Log(to_string(p.warrant_code()) + std::string("Do Force Sell 1"));

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


	p.Log(to_string(p.warrant_code()) + std::string("Do Force Sell 2"));

	if(p.status() != STATUS_AVAILABLE){
		return "Invalid order status";
	}


	p.Log(to_string(p.warrant_code()) + std::string("Do Force Sell 3"));

	if(p.has_position()){
		p.Log(to_string(p.warrant_code()) + std::string("Position = ") + to_string(p.getPosition()));
		if(p.getPosition()<quantity){
			return "Insufficient Position";
		}

		p.Log(to_string(p.warrant_code()) + std::string("Do Force Sell 4"));

		//auto result = p.sell(0, false, quantity);
		bool result = p.doSell(newWarrant);
		if(!result){
			return "Order Error";;
		}

		p.Log(to_string(p.warrant_code()) + std::string("Do Force Sell 5"));

	}else{

		p.Log(to_string(p.warrant_code()) + std::string("No Position"));

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

void bear::Log(string msg){
	logger->Log(string(DateUtil::getCurrentTime()) + " tm: " + to_string(dbp::tools::srv::current()) + " " +  msg);
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

		Log(json.dump());
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

			Log("bear 1");

			//p._Symbol = json["symbol"].get<std::string>();
			p._IssuerSize = json["issuersize"].get<unsigned long long>();
			p._Quantity = json["quantity"].get<unsigned long long>();
			p._SPREAD = json["spread"].get<unsigned long long>();
			p._warrant_code = json["warrant_code"].get<unsigned int>();


			p._DELTA = 0;

			auto it_delta = json.find("delta");
			if (json.end() != it_delta)
			{
				std::string strDelta = json["delta"].get<string>();
				p._DELTA = atof(strDelta.c_str());
			}

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

			Log("bear 2");

			std::string str_underlying = json["symbol"].get<std::string>();


			p._Symbol = "";
			if(str_underlying.rfind("HSI", 0) == 0 || str_underlying.rfind("HTI", 0) == 0 || str_underlying.rfind("HHI", 0) == 0){
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
				p._Utype = HSI_SYMBOL;
			}

			if(str_underlying.rfind("NQ", 0) == 0){
				p._Symbol = str_underlying;

				auto ittcpMap = tcpMap.find(str_underlying);
				if (tcpMap.end() == ittcpMap)
				{
					auto msg = algo_err_msg_pool.get_obj();
					msg->al = this;
					msg->algo_name = _name;
					msg->id = _u.get_id();
					msg->ref = ref;
					msg->action = "cmd set";
					msg->result = "FAIL";
					msg->reason = "fail command set " + str_underlying + " mapping not found";
					pset->release();
					return msg;
				}
				p._Underlying_code = 0;
				p._Utype = NQ_SYMBOL;
			}

			if("" == p._Symbol){
				unsigned int ucode = 0;
				try
				{
					int iucode = std::stoi(str_underlying);
					ucode = static_cast<unsigned int>(iucode);

					if(stockWarrantomdcMap.end() == stockWarrantomdcMap.find(ucode)){
						auto msg = algo_err_msg_pool.get_obj();
						msg->al = this;
						msg->algo_name = _name;
						msg->id = _u.get_id();
						msg->ref = ref;
						msg->action = "cmd set";
						msg->result = "FAIL";
						msg->reason = "fail command set " + str_underlying + " underlying not found";
						pset->release();
						return msg;
					}
				}
				catch(const std::exception& e)
				{
					auto msg = algo_err_msg_pool.get_obj();
					msg->al = this;
					msg->algo_name = _name;
					msg->id = _u.get_id();
					msg->ref = ref;
					msg->action = "cmd set";
					msg->result = "FAIL";
					msg->reason = "fail command set " + str_underlying + " Invalid underlying code";
					pset->release();
					return msg;
				}

				p._Symbol = str_underlying;
				p._Underlying_code = ucode;
				p._Utype = ST_SYMBOL;


			}



			Log("Set Pair Utype = " + to_string(p._Utype));

			if(p._Symbol == ""){
				auto msg = algo_err_msg_pool.get_obj();
				msg->al = this;
				msg->algo_name = _name;
				msg->id = _u.get_id();
				msg->ref = ref;
				msg->action = "cmd set";
				msg->result = "FAIL";
				msg->reason = "fail command set Invalid Symbol";
				pset->release();
				return msg;
			}

			/*
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
*/



			Log("bear 3");
			p._OBSetting = new OBSetting();
			p._PriceInfo = new priceinfo();
			p._PriceInfo->LastBidSeq=0;
			p._PriceInfo->LastAskSeq=0;
			p._PriceInfoU = new priceinfo();
			if(p._Utype == HSI_SYMBOL){

				Log("HSI_SYMBOL Code = " + to_string(p._warrant_code));

				p._CbbcPriceMark = new CbbcPriceMark(INDEX_TYPE, p._Wtype, p._SPREAD, 100000);

				auto itpm = pricemarkMap.find(p._warrant_code);
				if(itpm != pricemarkMap.end()){
					p._CbbcPriceMark->copyTable(itpm->second->getBidTable(), itpm->second->getAskTable());
					p._CbbcPriceMark->copyUpTable(itpm->second->getUpBidTable(), itpm->second->getUpAskTable());
					p._CbbcPriceMark->copyDnTable(itpm->second->getDnBidTable(), itpm->second->getDnAskTable());
				}
				if(ivLoader.exist(p._warrant_code)){

					Log("Load IV = " + to_string(p._warrant_code));

					WarrantIv wiv = ivLoader.getWarrantIv(p._warrant_code);
					if (wiv.Delta > 0){
						unsigned long long sensitivity = static_cast<unsigned long long>(0.001f * wiv.Cratio / wiv.Delta) * 100000;
						p._CbbcPriceMark->setSensitivity(sensitivity);

						Log("1 Sensitivity = " + to_string(sensitivity));
					}else if(p._DELTA > 0){
						unsigned long long sensitivity = static_cast<unsigned long long>(0.001f * wiv.Cratio / p._DELTA) * 100000;
						p._CbbcPriceMark->setSensitivity(sensitivity);

						Log("2 Sensitivity = " + to_string(sensitivity));
					}

				}

			}
			if(p._Utype == NQ_SYMBOL){
				p._CbbcPriceMark = new CbbcPriceMark(INDEX_TYPE, p._Wtype, p._SPREAD, 250000000);
			}
			if(p._Utype == ST_SYMBOL){

				if(omdcAdditionDefinitionsMap.end() == omdcAdditionDefinitionsMap.find(p._Underlying_code)){
					auto msg = algo_err_msg_pool.get_obj();
					msg->al = this;
					msg->algo_name = _name;
					msg->id = _u.get_id();
					msg->ref = ref;
					msg->action = "cmd set";
					msg->result = "FAIL";
					msg->reason = "fail command set " + str_underlying + " Invalid underlying code";
					pset->release();
					return msg;
				}

				/*
				unsigned long long price = 0;
				auto it = stockWarrantomdcMap.find(p._Underlying_code);
				if (stockWarrantomdcMap.end() != it)
				{
					auto& tradable = it->second;
					if (0 != tradable.m_Bid[0].m_iPrice)
					{
						price = static_cast<unsigned long long>(tradable.m_Bid[0].m_iPrice) * 100000;
					}
				}

				COmdcAdditionDefinitions omdcdef = omdcAdditionDefinitionsMap[p._Underlying_code];
				unsigned long long spread = spreadTable.getSpread(omdcdef.SpreadTableCode, price);
				 */
				//p._CbbcPriceMark = new CbbcPriceMark(p._Wtype, p._SPREAD, spread);
				p._CbbcPriceMark = new CbbcPriceMark(STOCK_TYPE, p._Wtype, p._SPREAD, 1000000);



				auto itpm = pricemarkMap.find(p._warrant_code);
				if(itpm != pricemarkMap.end()){
					p._CbbcPriceMark->copyTable(itpm->second->getBidTable(), itpm->second->getAskTable());
					p._CbbcPriceMark->copyUpTable(itpm->second->getUpBidTable(), itpm->second->getUpAskTable());
					p._CbbcPriceMark->copyDnTable(itpm->second->getDnBidTable(), itpm->second->getDnAskTable());
				}

				/*
				if(ivLoader.exist(p._warrant_code)){
					WarrantIv wiv = ivLoader.getWarrantIv(p._warrant_code);
					if (wiv.Delta > 0){
						unsigned long long sensitivity = static_cast<unsigned long long>(0.001f * wiv.Cratio / wiv.Delta * 100) * 100000;
						p._CbbcPriceMark->setSensitivity(sensitivity);
					}
				}*/


			}

			Log("bear 4");

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

