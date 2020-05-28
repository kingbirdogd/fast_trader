#ifndef __SMFH_SRV_OMDD__
#define __SMFH_SRV_OMDD__
#include <tools.h>
#include <global_memory.hpp>
#include "smfh_srv_orderbook.h"
inline static void handleOmdd(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm)
{
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
	auto it = omddMap.find(uSecurityCode);
	if (omddMap.end() == it)
	{
		return;
	}
	COmdOrderbook& rOrderBook = it->second;
	rOrderBook.m_PkgTime = _uPkgTm;
	rOrderBook.m_MsgTime = dbp::tools::srv::current();
	if (353 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
		buildOmddOrderBook(_pMsg, rOrderBook);
		std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		rOrderBook.m_AccumulateBuyQuantity = 0;
		rOrderBook.m_AccumulateSellQuantity = 0;
		rOrderBook.m_AccumulateBlankQuantity = 0;
	}
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
	}
	broadcastQueue.enqueue(rOrderBook);
}

#endif


