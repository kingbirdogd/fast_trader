#ifndef __SMFH_SRV_PRICETABLE__
#define __SMFH_SRV_PRICETABLE__
#include <tools.h>
#include <global_memory.hpp>
#include "smfh_srv_orderbook.h"
#ifndef NOT_MEASURE
inline static void handlePricetable(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm)
#else
inline static void handlePricetable(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long)
#endif
{
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
	auto it = ptomdcMap.find(uSecurityCode);
	if (ptomdcMap.end() == it)
	{
		return;
	}
	COmdOrderbook& rOrderBook = it->second;
	if (53 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
		buildOmdcOrderBook(_pMsg, rOrderBook);
		std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));

		auto best_bid_price = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_iPrice) * 100000;
		auto best_bid_qty = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_uQuantity);

		auto best_ask_price = static_cast<unsigned long long>(rOrderBook.m_Ask[0].m_iPrice) * 100000;
		auto best_ask_qty = static_cast<unsigned long long>(rOrderBook.m_Ask[0].m_uQuantity);

		pricedata* pd = pricedataMap[uSecurityCode];



		if(pd->isWarrant){

			pricedata* pdu = pricedataMap[pd->UCode];
			PriceMark* _PriceMark = pricemarkMap[uSecurityCode];

			if(_PriceMark->getWType() == 1){
				if(pd->Bestbid != best_bid_price && best_bid_price > 0){
					if(best_bid_qty >= pd->BidIssuerSize){
						_PriceMark->updateBid(best_bid_price, pd->Bestbid, pdu->Bestbid, pdu->PBestbid);
					}
				}
				if(pd->Bestask != best_ask_price && best_ask_price > 0){
					//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark ASK");
					if(best_ask_qty >= pd->AskIssuerSize){
						_PriceMark->updateAsk(best_ask_price, pd->Bestask, pdu->Bestask, pdu->PBestask);
					}
				}
			}else{
				if(pd->Bestbid != best_bid_price && best_bid_price > 0){
					//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark BID");
					if(best_bid_qty >= pd->BidIssuerSize){
						_PriceMark->updateBid(best_bid_price, pd->Bestbid, pdu->Bestask, pdu->PBestask);
					}
				}
				if(pd->Bestask != best_ask_price && best_ask_price > 0){
					//_algo->log_info(std::string(" WCODE ") + std::to_string(code) + " DO Mark ASK");
					if(best_ask_qty >= pd->AskIssuerSize){
						_PriceMark->updateAsk(best_ask_price, pd->Bestask, pdu->Bestbid, pdu->PBestbid);
					}
				}
			}


		}

		if(pd->Bestbid != best_bid_price){
			pd->PBestbid = pd->Bestbid;
			pd->Bestbid = best_bid_price;
			pd->BestBidQty = best_bid_qty;
		}
		if(pd->Bestask != best_ask_price ){
			pd->PBestask = pd->Bestask;
			pd->Bestask = best_ask_price;
			pd->BestAskQty = best_ask_qty;
		}

	}
	else if( 60 == _pMsg->m_uMsgType)
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
					if(Item > 9499 && 9499 < 9800){
						bidbrokerno = Item;
						break;
					}
				}
				if(Type == "S")
				{
					bidspread++;
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
					if(Item > 9499 && 9499 < 9800){
						askbrokerno = Item;
						break;
					}
				}
				if(Type == "S")
				{
					askspread++;
				}

				count += 4;
			}

			if(pd->AskIssuerSize != pd->BestAskQty || pd->AskIssuerSize == 0){
				pd->AskIssuerSize = pd->BestAskQty;
			}
		}
	}
}
#endif


