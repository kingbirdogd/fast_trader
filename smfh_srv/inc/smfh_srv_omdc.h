#ifndef __SMFH_SRV_OMDC__
#define __SMFH_SRV_OMDC__
#include <tools.h>
#include <global_memory.hpp>
#include "smfh_srv_orderbook.h"
#ifndef NOT_MEASURE
inline static void handleOmdc(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm)
#else
inline static void handleOmdc(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long)
#endif
{
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
	auto it = omdcMap.find(uSecurityCode);
	if (omdcMap.end() == it)
	{
		return;
	}
	COmdOrderbook& rOrderBook = it->second;
#ifndef NOT_MEASURE
	rOrderBook.m_PkgTime = _uPkgTm;
	rOrderBook.m_MsgTime = dbp::tools::srv::current();
#endif
	if (30 == _pMsg->m_uMsgType)
	{
		auto type = OMD_GET_VALUE(_pMsg, 26, FullTickBook::OrderType);
		if (FullTickBook::OrderType::Limit == type)
		{
			auto& book = omdcFullTickBook[uSecurityCode];
			auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
			auto price = OMD_GET_VALUE(_pMsg, 16, int);
			auto quantity = OMD_GET_VALUE(_pMsg, 20, unsigned int);
			auto side = OMD_GET_VALUE(_pMsg, 24, FullTickBook::OrderSide);
			book.new_order(id, price, quantity, side);
			rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
			if (FullTickBook::OrderSide::BID == side)
			{
				ConvertFullBookToBook(book.Bids, rOrderBook);
			}
			else
			{
				ConvertFullBookToBook(book.Asks, rOrderBook);
			}
		}
	}
	else if (31 == _pMsg->m_uMsgType)
	{
		auto& book = omdcFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto quantity = OMD_GET_VALUE(_pMsg, 16, unsigned int);
		auto side = book.modify_order(id, quantity);
		rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
		if (FullTickBook::OrderSide::BID == side)
		{
			ConvertFullBookToBook(book.Bids, rOrderBook);
		}
		else
		{
			ConvertFullBookToBook(book.Asks, rOrderBook);
		}

	}
	else if (32 == _pMsg->m_uMsgType)
	{
		auto& book = omdcFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto side = book.cancel_order(id);
		rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
		if (FullTickBook::OrderSide::BID == side)
		{
			ConvertFullBookToBook(book.Bids, rOrderBook);
		}
		else
		{
			ConvertFullBookToBook(book.Asks, rOrderBook);
		}
	}
	else if (53 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
		buildOmdcOrderBook(_pMsg, rOrderBook);
		std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		rOrderBook.m_AccumulateBuyQuantity = 0;
		rOrderBook.m_AccumulateSellQuantity = 0;
		rOrderBook.m_AccumulateBlankQuantity = 0;
		broadcastQueue.enqueue(rOrderBook);
	}
	else if (50 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDC_TRADE;
		rOrderBook.m_LastTradeQuantity = static_cast<unsigned long long>(OMD_GET_VALUE(_pMsg, 16, unsigned int));
		rOrderBook.m_LastTradePrice = OMD_GET_VALUE(_pMsg, 12, int);
		rOrderBook.m_TradeType = OMD_GET_VALUE(_pMsg, 20, unsigned short int);
		if (rOrderBook.m_LastTradePrice <= rOrderBook.m_Bid[0].m_iPrice)
		{
			rOrderBook.m_TradeSide = TradeSide::SELL_SIDE;
			rOrderBook.m_AccumulateSellQuantity += rOrderBook.m_LastTradeQuantity;
		}
		else if (rOrderBook.m_LastTradePrice >= rOrderBook.m_Ask[0].m_iPrice)
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
	}
}
#endif


