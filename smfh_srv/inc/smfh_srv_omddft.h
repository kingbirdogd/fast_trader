#ifndef __SMFH_SRV_OMDDFT__
#define __SMFH_SRV_OMDDFT__
#include <tools.h>
#include <global_memory.hpp>
#include "smfh_srv_orderbook.h"
#ifndef NOT_MEASURE
inline static void handleOmddFt(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm)
#else
inline static void handleOmddFt(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long)
#endif
{
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
	auto it = omddMap.find(uSecurityCode);
	if (omddMap.end() == it)
	{
		return;
	}
	COmdOrderbook& rOrderBook = it->second;
	if (330 == _pMsg->m_uMsgType)
	{
		auto& book = omddFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto price = OMD_GET_VALUE(_pMsg, 16, int);
		auto quantity = OMD_GET_VALUE(_pMsg, 20, unsigned int);
		auto side = OMD_GET_VALUE(_pMsg, 24, OmddFullTickBook::OrderSide);
		auto rest = quantity;

		if(uSecurityCode != 201527202)
			return;

		DEBUG("tm:%llu, OMDD Add order, code: %u, side: %u, price: %d, quantity: %u\n",
						dbp::tools::srv::current(),
						uSecurityCode,
						static_cast<unsigned int>(side),
						price,
						quantity);
		if (OmddFullTickBook::OrderSide::BID == side)
		{
			for (auto it = book.Asks.begin(); it != book.Asks.end(); ++it)
			{
				if (it->first <= price)
				{
					auto matched_price = it->first;
					auto book_quantity = it->second.quantity;
					auto matched_quantity = rest < book_quantity ? rest : book_quantity;
					rOrderBook.m_MsgType = MsgType::OMDD_TRADE;
					rOrderBook.m_LastTradeQuantity = matched_quantity;
					rOrderBook.m_LastTradePrice = matched_price;
					rOrderBook.m_TradeType = 0;
					rOrderBook.m_TradeSide = TradeSide::BUY_SIDE;
					rOrderBook.m_AccumulateBuyQuantity += rOrderBook.m_LastTradeQuantity;
					broadcastQueue.enqueue(rOrderBook);
					rest -= matched_quantity;
					if (0 == rest)
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			for (auto it = book.Bids.begin(); it != book.Bids.end(); ++it)
			{
				if (it->first >= price)
				{
					auto matched_price = it->first;
					auto book_quantity = it->second.quantity;
					auto matched_quantity = rest < book_quantity ? rest : book_quantity;
					rOrderBook.m_MsgType = MsgType::OMDD_TRADE;
					rOrderBook.m_LastTradeQuantity = matched_quantity;
					rOrderBook.m_LastTradePrice = matched_price;
					rOrderBook.m_TradeType = 0;
					rOrderBook.m_TradeSide = TradeSide::SELL_SIDE;
					rOrderBook.m_AccumulateSellQuantity += rOrderBook.m_LastTradeQuantity;
					broadcastQueue.enqueue(rOrderBook);
					rest -= matched_quantity;
					if (0 == rest)
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
		auto is_top = book.new_order(id, price, quantity, side);
		if (is_top)
		{
			//book.new_order(id, price, quantity, side);
			rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
			if (OmddFullTickBook::OrderSide::BID == side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				if (!book.isCross())
				{
					broadcastQueue.enqueue(rOrderBook);
				}
			}
			else if (OmddFullTickBook::OrderSide::ASK == side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				if (!book.isCross())
				{
					broadcastQueue.enqueue(rOrderBook);
				}
			}
		}
	}
	else if (331 == _pMsg->m_uMsgType)
	{
		auto& book = omddFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto price = OMD_GET_VALUE(_pMsg, 16, int);
		auto quantity = OMD_GET_VALUE(_pMsg, 20, unsigned int);
		auto result = book.modify_order(id, quantity, price);
		if (result.is_top)
		{
			rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
			if (OmddFullTickBook::OrderSide::BID == result.side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}
			else if (OmddFullTickBook::OrderSide::ASK == result.side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}
		}
	}
	else if (332 == _pMsg->m_uMsgType)
	{
		auto& book = omddFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto result = book.cancel_order(id);
		if (result.is_top)
		{
			rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
			if (OmddFullTickBook::OrderSide::BID == result.side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}
			else if (OmddFullTickBook::OrderSide::ASK == result.side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}
		}
	}
	else if (335 == _pMsg->m_uMsgType)
	{
		omddFullTickBook[uSecurityCode].clear();
		std::memset(rOrderBook.m_Bid, 0, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		std::memset(rOrderBook.m_Ask, 0, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		rOrderBook.m_AccumulateBuyQuantity = 0;
		rOrderBook.m_AccumulateSellQuantity = 0;
		rOrderBook.m_AccumulateBlankQuantity = 0;
		rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
		broadcastQueue.enqueue(rOrderBook);
	}

}

#endif


