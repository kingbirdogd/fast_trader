#ifndef __SMFH_SRV_OMDD__
#define __SMFH_SRV_OMDD__
#include <tools.h>
#include <spin.h>
#include "smfh_srv_orderbook.h"
inline static void handleOmdd(dbp::omd::COmdMsgHeader* _pMsg, unsigned int _uSeq, unsigned long long _uChannelIdx, unsigned long long _uPkgTm)
{
	if (_uSeq == 1){}
	if (_uChannelIdx == 1){}
	if (_uPkgTm == 1){}
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
	auto it = omddMap.find(uSecurityCode);
	if (omddMap.end() == it)
	{
		return;
	}
	COmdOrderbook& rOrderBook = it->second;
	if (353 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
		buildOmddOrderBook(_pMsg, rOrderBook);
		std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
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
		}
		else if (3 == omdd_side)
		{
			rOrderBook.m_TradeSide = TradeSide::BUY_SIDE;
		}
		else
		{
			rOrderBook.m_TradeSide = TradeSide::NO_SIDE;
		}
	}
}

#endif


