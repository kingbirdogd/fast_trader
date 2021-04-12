#ifndef __SMFH_SRV_OMDDPRICETABLE__
#define __SMFH_SRV_OMDDPRICETABLE__
#include <tools.h>
#include <global_memory.hpp>
#include "smfh_srv_orderbook.h"
#ifndef NOT_MEASURE
inline static void handleOmddPricetable(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm)
#else
inline static void handleOmddPricetable(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long)
#endif
{
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);

	auto it = ptomddMap.find(uSecurityCode);
	if (ptomddMap.end() == it)
	{
		return;
	}
	COmdOrderbook& rOrderBook = it->second;

	if (353 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
		unsigned char updatelvl = buildSlimOmdcOrderBook(_pMsg, rOrderBook);

		if(updatelvl == 1){
			std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, 1 * sizeof(OrderItem));
			std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, 1 * sizeof(OrderItem));


			auto best_bid_price = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_iPrice) * 100000;
			auto best_bid_qty = static_cast<unsigned long long>(rOrderBook.m_Bid[0].m_uQuantity);

			auto best_ask_price = static_cast<unsigned long long>(rOrderBook.m_Ask[0].m_iPrice) * 100000;
			auto best_ask_qty = static_cast<unsigned long long>(rOrderBook.m_Ask[0].m_uQuantity);

			pricedata* pd = pricedataMap[uSecurityCode];

			pd->BSeq++;
			if(pd->Bestbid != best_bid_price){
				pd->BidSeq = pd->BSeq;
				pd->PBestbid = pd->Bestbid;
				pd->PBestBidQty = pd->BestBidQty;
				pd->Bestbid = best_bid_price;
				pd->BestBidQty = best_bid_qty;
				pd->Type = PRICE_CHANGE;
			}

			pd->ASeq++;
			if(pd->Bestask != best_ask_price ){
				pd->AskSeq = pd->ASeq;
				pd->PBestask = pd->Bestask;
				pd->PBestAskQty = pd->BestAskQty;
				pd->Bestask = best_ask_price;
				pd->BestAskQty = best_ask_qty;
				pd->Type = PRICE_CHANGE;
			}
		}
	}
	/*
	else if (350 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDD_TRADE;
		rOrderBook.m_LastTradeQuantity = static_cast<unsigned long long>(OMD_GET_VALUE(_pMsg, 40, unsigned int));
		rOrderBook.m_LastTradePrice = OMD_GET_VALUE(_pMsg, 16, int);
		rOrderBook.m_TradeType = OMD_GET_VALUE(_pMsg, 33, unsigned short int);
		unsigned char omdd_side = OMD_GET_VALUE(_pMsg, 32, unsigned char);
		if (2 == omdd_side)
		{
			rOrderBook.m_TradeSide = TradeSide::SELL_SIDE;
			rOrderBook.m_AccumulateSellQuantity += rOrderBook.m_LastTradeQuantity;
		}
		else if (3 == omdd_side)
		{
			rOrderBook.m_TradeSide = TradeSide::BUY_SIDE;
			rOrderBook.m_AccumulateBuyQuantity += rOrderBook.m_LastTradeQuantity;
		}
		else
		{
			rOrderBook.m_TradeSide = TradeSide::NO_SIDE;
			rOrderBook.m_AccumulateBlankQuantity += rOrderBook.m_LastTradeQuantity;
		}
		broadcastQueue.enqueue(rOrderBook);
	}*/
}

#endif


