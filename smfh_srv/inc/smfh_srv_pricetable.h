#ifndef __SMFH_SRV_PRICETABLE__
#define __SMFH_SRV_PRICETABLE__
#include <tools.h>
#include <global_memory.hpp>
#include "smfh_srv_orderbook.h"



inline static unsigned long long issuerSize80(unsigned long long size){
	return static_cast<unsigned long long>(size*0.8);
}


#ifndef NOT_MEASURE
inline static void handlePricetable(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm)
#else
inline static void handlePricetable(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long)
#endif
{
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);

/*
	DEBUG("tm:%llu, Refresh : %u Type = %d\n",
											dbp::tools::srv::current(),
											uSecurityCode,
											_pMsg->m_uMsgType
											);
*/

	auto it = ptomdcMap.find(uSecurityCode);
	if (ptomdcMap.end() == it)
	{
		return;
	}
	COmdOrderbook& rOrderBook = it->second;
#ifndef NOT_MEASURE
	rOrderBook.m_PkgTime = _uPkgTm;
	rOrderBook.m_MsgTime = dbp::tools::srv::current();
#endif
	if (53 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
		//buildOmdcOrderBook(_pMsg, rOrderBook);
		unsigned char updatelvl = buildSlimOmdcOrderBook(_pMsg, rOrderBook);

		//pricedata* pd = pricedataMap[uSecurityCode];

		//OrderItem m_Bid[3];
		//OrderItem m_Ask[3];


		if(updatelvl <= 2){
			std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, 2 * sizeof(OrderItem));
			std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, 2 * sizeof(OrderItem));
		}else{
			return;
		}


/*
		if(pd->isWarrant){
			if(updatelvl <= 1){
				std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, 1 * sizeof(OrderItem));
				std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, 1 * sizeof(OrderItem));
			}else{
				return;
			}
		}else{
			//if(updatelvl <= 3){
			if(updatelvl <= 1){

				//OrderItem m_Bid[3];
				//OrderItem m_Ask[3];

				//std::memcpy(m_Bid, rOrderBook.m_BidOrder, 3 * sizeof(OrderItem));
				//std::memcpy(m_Ask, rOrderBook.m_AskOrder, 3 * sizeof(OrderItem));
				std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, 1 * sizeof(OrderItem));
				std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, 1 * sizeof(OrderItem));

				//std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, 3 * sizeof(OrderItem));
				//std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, 3 * sizeof(OrderItem));


			}else{
				return;
			}
		}
*/


		//std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, 3 * sizeof(OrderItem));
		//std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, 3 * sizeof(OrderItem));
		//std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		//std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));

		unsigned long long issuer_bid = 0;
		unsigned long long issuer_ask = 0;

		auto best_bid_price = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_iPrice) * 100000;
		auto best_bid_qty = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_uQuantity);

		auto best_ask_price = static_cast<unsigned long long>(rOrderBook.m_Ask[0].m_iPrice) * 100000;
		auto best_ask_qty = static_cast<unsigned long long>(rOrderBook.m_Ask[0].m_uQuantity);




		//gSeq++;

		pricedata* pd = pricedataMap[uSecurityCode];

		if(pd->isWarrant){

			auto best_bid_price1 = static_cast<unsigned long long>(rOrderBook.m_Bid[1].m_iPrice) * 100000;
			auto best_bid_qty1 = static_cast<unsigned long long>(rOrderBook.m_Bid[1].m_uQuantity);

			auto best_ask_price1 = static_cast<unsigned long long>(rOrderBook.m_Ask[1].m_iPrice) * 100000;
			auto best_ask_qty1 = static_cast<unsigned long long>(rOrderBook.m_Ask[1].m_uQuantity);

			if(best_bid_qty >= issuerSize80(pd->BidIssuerSize)){
				issuer_bid = best_bid_price;
			}else if(best_bid_qty1 >= issuerSize80(pd->BidIssuerSize)){
				issuer_bid = best_bid_price1;
			}
			if(best_ask_qty >= issuerSize80(pd->AskIssuerSize)){
				issuer_ask = best_ask_price;
			}else if(best_ask_qty1 >= issuerSize80(pd->AskIssuerSize)){
				issuer_ask = best_ask_price1;
			}

			//pricedata* pdu = nullptr;

			//pricedata* pdu =

			unsigned long long _UBestbid = 0;
			unsigned long long _UBestask = 0;
			unsigned long long _PUBestbid = 0;
			unsigned long long _PUBestask = 0;
			unsigned long long BidSeq = 0;
			unsigned long long AskSeq = 0;



			if(pd->IsIndex){
				unsigned int orderbook_id = 0;
				if(OmddDefReady ){

					if(pd->Orderbook_id == 0){
						unsigned int omddcode = pd->UCode;
						if(pd->UCode == 100001){
							auto symbol = hsiVec.front();
							orderbook_id = nameToCode[symbol];
							pd->Orderbook_id = orderbook_id;

							flush_printf("tm:%llu, OMDD PriceTable, code : %d, orderbookid : %d, ucode : %d, symbol : %s\n",
							dbp::tools::srv::current(), uSecurityCode,
							orderbook_id, pd->UCode, symbol.c_str());

						}
						if(pd->UCode == 100002){
							auto symbol = hsceiVec.front();
							orderbook_id = nameToCode[symbol];
							pd->Orderbook_id = orderbook_id;
							flush_printf("tm:%llu, OMDD PriceTable, code : %d, orderbookid : %d, ucode : %d, symbol : %s \n",
								dbp::tools::srv::current(), uSecurityCode,
								orderbook_id, pd->UCode, symbol.c_str());
						}
						if(pd->UCode == 100003){
							auto symbol = hstecVec.front();
							orderbook_id = nameToCode[symbol];
							pd->Orderbook_id = orderbook_id;
							flush_printf("tm:%llu, OMDD PriceTable, code : %d, orderbookid : %d, ucode : %d, symbol : %s \n",
								dbp::tools::srv::current(), uSecurityCode,
								orderbook_id, pd->UCode, symbol.c_str());
						}

						if(pd->Orderbook_id == 0)
							return;
					}

					pricedata* _pdu = pricedataMap[pd->Orderbook_id];
					_UBestbid = _pdu->Bestbid;
					_UBestask = _pdu->Bestask;
					_PUBestbid = _pdu->PBestbid;
					_PUBestask = _pdu->PBestask;
					BidSeq = _pdu->BidSeq;
					AskSeq = _pdu->AskSeq;
				}
			}else{
				pricedata* _pdu = pricedataMap[pd->UCode];
				_UBestbid = _pdu->Bestbid;
				_UBestask = _pdu->Bestask;
				_PUBestbid = _pdu->PBestbid;
				_PUBestask = _pdu->PBestask;
				BidSeq = _pdu->BidSeq;
				AskSeq = _pdu->AskSeq;
			}


			//if(pdu == nullptr)
			//	return;


			//pricedata* pdu = pricedataMap[pd->UCode];
			PriceMark* _PriceMark = pricemarkMap[uSecurityCode];

			if(_PriceMark == nullptr){
				return;
			}

			if(_PriceMark->getWType() == 1){
				//if(pd->Bestbid != best_bid_price && best_bid_price > 0){
				if(pd->IBestbid != issuer_bid && issuer_bid > 0){
					//if(best_bid_qty >= issuerSize80(pd->BidIssuerSize) && pd->BidIssuerSize > 0){
						if(pd->LastBidSeq != BidSeq && BidSeq>0){
							pd->LastBidSeq = BidSeq;
							//_PriceMark->updateBid(best_bid_price, pd->Bestbid, pdu->Bestbid, pdu->PBestbid);
							if(pd->IsIndex)
								_PriceMark->updateIBid(issuer_bid, pd->IBestbid, _UBestbid, _PUBestbid);
							else
								_PriceMark->updateBid(issuer_bid, pd->IBestbid, _UBestbid, _PUBestbid);
							_PriceMark->setBidIssuerQty(best_bid_qty);
						}
					//}
					pd->IBestbid = issuer_bid;
				}
				//if(pd->Bestask != best_ask_price && best_ask_price > 0){
				if(pd->IBestask != issuer_ask && issuer_ask > 0){
					//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark ASK");
					//if(best_ask_qty >= issuerSize80(pd->AskIssuerSize) && pd->AskIssuerSize > 0){
						if(pd->LastAskSeq != AskSeq && AskSeq>0){
							pd->LastAskSeq = AskSeq;
							//_PriceMark->updateAsk(best_ask_price, pd->Bestask, pdu->Bestask, pdu->PBestask);
							if(pd->IsIndex)
								_PriceMark->updateIAsk(issuer_ask, pd->IBestask, _UBestask, _PUBestask);
							else
								_PriceMark->updateAsk(issuer_ask, pd->IBestask, _UBestask, _PUBestask);
							_PriceMark->setAskIssuerQty(best_ask_qty);
						}
					//}
					pd->IBestask = issuer_ask;
				}

			}else{
				//if(pd->Bestbid != best_bid_price && best_bid_price > 0){
				if(pd->IBestbid != issuer_bid && issuer_bid > 0){
					//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark BID");
					//if(best_bid_qty >= issuerSize80(pd->BidIssuerSize) && pd->BidIssuerSize > 0){
						if(pd->LastAskSeq != AskSeq && AskSeq>0){
							pd->LastAskSeq = AskSeq;
							//_PriceMark->updateBid(best_bid_price, pd->Bestbid, pdu->Bestask, pdu->PBestask);
							if(pd->IsIndex)
								_PriceMark->updateIBid(issuer_bid, pd->IBestbid, _UBestask, _PUBestask);
							else
								_PriceMark->updateBid(issuer_bid, pd->IBestbid, _UBestask, _PUBestask);
							_PriceMark->setBidIssuerQty(best_bid_qty);
						}
					//}
					pd->IBestbid = issuer_bid;
				}
				//if(pd->Bestask != best_ask_price && best_ask_price > 0){
				if(pd->IBestask != issuer_ask && issuer_ask > 0){
					//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark ASK");
					//if(best_ask_qty >= issuerSize80(pd->AskIssuerSize) && pd->AskIssuerSize > 0){
						if(pd->LastBidSeq != BidSeq && BidSeq>0){
							pd->LastBidSeq = BidSeq;
							//_PriceMark->updateAsk(best_ask_price, pd->Bestask, pdu->Bestbid, pdu->PBestbid);
							if(pd->IsIndex)
								_PriceMark->updateIAsk(issuer_ask, pd->IBestask, _UBestbid, _PUBestask);
							else
								_PriceMark->updateAsk(issuer_ask, pd->IBestask, _UBestbid, _PUBestask);
							_PriceMark->setAskIssuerQty(best_ask_qty);
						}
					//}
					pd->IBestask = issuer_ask;
				}
			}
		}



		pd->BSeq++;
		if(pd->Bestbid != best_bid_price){
			pd->BidSeq = pd->BSeq;
			pd->PBestbid = pd->Bestbid;
			pd->PBestBidQty = pd->BestBidQty;
			pd->Bestbid = best_bid_price;
			pd->BestBidQty = best_bid_qty;
			pd->Type = PRICE_CHANGE;
			//pd->gBSeq = gSeq;
		}else{
			pd->Type = VOL_CHANGE;
			pd->BidVolSeq = pd->BSeq;
			pd->PBestBidQty = pd->BestBidQty;
			pd->BestBidQty = best_bid_qty;
		}
		//pd->BestBidQty = best_bid_qty;

		pd->ASeq++;
		if(pd->Bestask != best_ask_price ){
			pd->AskSeq = pd->ASeq;
			pd->PBestask = pd->Bestask;
			pd->PBestAskQty = pd->BestAskQty;
			pd->Bestask = best_ask_price;
			pd->BestAskQty = best_ask_qty;
			pd->Type = PRICE_CHANGE;
			//pd->gASeq = gSeq;
		}else{
			pd->Type = VOL_CHANGE;
			pd->AskVolSeq = pd->ASeq;
			pd->PBestAskQty = pd->BestAskQty;
			pd->BestAskQty = best_ask_qty;
		}
		//pd->BestAskQty = best_ask_qty;

/*
		if(pd->isUnderlying){
			auto best_bid_price1 = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_iPrice);
			auto best_ask_price1 = static_cast<unsigned long long>(rOrderBook.m_Ask[0].m_iPrice);
			auto best_bid_price2 = static_cast<unsigned long long>(rOrderBook.m_Bid[1].m_iPrice);
			auto best_ask_price2 = static_cast<unsigned long long>(rOrderBook.m_Ask[1].m_iPrice);
			auto best_bid_price3 = static_cast<unsigned long long>(rOrderBook.m_Bid[2].m_iPrice);
			auto best_ask_price3 = static_cast<unsigned long long>(rOrderBook.m_Ask[2].m_iPrice);
			auto best_bid_qty1 = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_uQuantity);
			auto best_ask_qty1 = static_cast<unsigned long long>(rOrderBook.m_Ask[0].m_uQuantity);
			auto best_bid_qty2 = static_cast<unsigned long long>(rOrderBook.m_Bid[1].m_uQuantity);
			auto best_ask_qty2 = static_cast<unsigned long long>(rOrderBook.m_Ask[1].m_uQuantity);
			auto best_bid_qty3 = static_cast<unsigned long long>(rOrderBook.m_Bid[2].m_uQuantity);
			auto best_ask_qty3 = static_cast<unsigned long long>(rOrderBook.m_Ask[2].m_uQuantity);
			pd->Bestbid1 = best_bid_price1;
			pd->Bestbid2 = best_bid_price2;
			pd->Bestbid3 = best_bid_price3;
			pd->Bestask1 = best_ask_price1;
			pd->Bestask2 = best_ask_price2;
			pd->Bestask3 = best_ask_price3;
			pd->BestBidQty1 = best_bid_qty1;
			pd->BestBidQty2 = best_bid_qty2;
			pd->BestBidQty3 = best_bid_qty3;
			pd->BestAskQty1 = best_ask_qty1;
			pd->BestAskQty2 = best_ask_qty2;
			pd->BestAskQty3 = best_ask_qty3;
		}
*/
	}
	else if( 54 == _pMsg->m_uMsgType)
	{



		pricedata* pd = pricedataMap[uSecurityCode];
		if(pd->isUnderlying)
			return;

		unsigned char ItemCount = OMD_GET_VALUE(_pMsg, 8, unsigned char);
		unsigned short int Side = OMD_GET_VALUE(_pMsg, 9, unsigned short int);
		if(1 == Side) //BUY
		{
			int bidspread = -1;
			unsigned short int bidbrokerno = 0;

			int count = 11;
			for(int i=0; i<ItemCount; i++){
				if(i == 0)
					bidspread = 0;

				std::string BQMoreFlag = OMD_GET_STR(_pMsg, count, 1);
				unsigned short int Item = OMD_GET_VALUE(_pMsg, count+1, unsigned short int);
				std::string Type = OMD_GET_STR(_pMsg, count+3, 1);



				if(Type == "B")
				{
					if(Item > 9499 && Item < 9800){
						bidbrokerno = Item;
						break;
					}
				}
				if(Type == "S")
				{
					bidspread++;
					break;
				}

				count += 4;
			}
			if(bidbrokerno>0 && bidspread==0){
				if(pd->BidIssuerSize != pd->BestBidQty || pd->BidIssuerSize == 0){
					pd->BidIssuerSize = pd->BestBidQty;
				}
				//System.out.println(pe.BidIssuerSize);

			}
		}
		else if(2 == Side) // SELL
		{
			int askspread = -1;
			unsigned short int askbrokerno = 0;

			int count = 11;
			for(int i=0; i<ItemCount; i++){
				if(i == 0)
					askspread = 0;

				std::string BQMoreFlag = OMD_GET_STR(_pMsg, count, 1);
				unsigned short int Item = OMD_GET_VALUE(_pMsg, count+1, unsigned short int);
				std::string Type = OMD_GET_STR(_pMsg, count+3, 1);


				if(Type == "B")
				{
					if(Item > 9499 && Item < 9800){
						askbrokerno = Item;
						break;
					}
				}
				if(Type == "S")
				{
					askspread++;
					break;
				}

				count += 4;
			}

			if(askbrokerno>0 && askspread==0){
				if(pd->AskIssuerSize != pd->BestAskQty || pd->AskIssuerSize == 0){
					pd->AskIssuerSize = pd->BestAskQty;
				}
			}
		}
	}
}
#endif


