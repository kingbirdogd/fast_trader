#ifndef __SMFH_SRV_OMDD__
#define __SMFH_SRV_OMDD__
#include <tools.h>
#include <global_memory.hpp>
#include "smfh_srv_orderbook.h"
#ifndef NOT_MEASURE
inline static void handleOmdd(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm)
#else
inline static void handleOmdd(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long)
#endif
{
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
	DEBUG("tm:%llu, OMDD Msg, code: %u, MsgType: %u\n",
			dbp::tools::srv::current(),
			uSecurityCode,
			_pMsg->m_uMsgType);
	auto it_underlyging = codeTounderlying.find(uSecurityCode);
	if (codeTounderlying.end() == it_underlyging)
	{
		DEBUG("tm:%llu, OMDD Msg Ignore, underlying not found, code: %u, MsgType: %u\n",
				dbp::tools::srv::current(),
				uSecurityCode,
				_pMsg->m_uMsgType);
		return;
	}
	const auto& underlying = it_underlyging->second;
	if (4 != underlying.InstrumentGroup)
	{
		DEBUG("tm:%llu, OMDD Msg Ignore, InstrumentGroup not match, code: %u, MsgType: %u, InstrumentGroup:%u\n",
				dbp::tools::srv::current(),
				uSecurityCode,
				_pMsg->m_uMsgType,
				underlying.InstrumentGroup);
		return;
	}
	if (4001 != underlying.CommodityCode && 4002 != underlying.CommodityCode)
	{
		DEBUG("tm:%llu, OMDD Msg Ignore, CommodityCode not match, code: %u, MsgType: %u, CommodityCode:%u\n",
				dbp::tools::srv::current(),
				uSecurityCode,
				_pMsg->m_uMsgType,
				underlying.CommodityCode);
		return;
	}
	auto it = omddMap.find(uSecurityCode);
	if (omddMap.end() == it)
	{
		DEBUG("tm:%llu, OMD Msg Ignore, not found in omddMap, code: %u, MsgType: %u\n",
				dbp::tools::srv::current(),
				uSecurityCode,
				_pMsg->m_uMsgType);
		return;
	}
	COmdOrderbook& rOrderBook = it->second;
#ifndef NOT_MEASURE
	rOrderBook.m_PkgTime = _uPkgTm;
	rOrderBook.m_MsgTime = dbp::tools::srv::current();
#endif
#ifdef DF
	if (330 == _pMsg->m_uMsgType)
	{
		auto& book = omddFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto price = OMD_GET_VALUE(_pMsg, 16, int);
		auto quantity = OMD_GET_VALUE(_pMsg, 20, unsigned int);
		auto side = OMD_GET_VALUE(_pMsg, 24, OmddFullTickBook::OrderSide);
		auto rest = quantity;
		DEBUG("tm:%llu, OMDD Add order, code: %u, type: %c, side: %u, price: %d, quantity: %u\n",
				dbp::tools::srv::current(),
				uSecurityCode,
				static_cast<char>(type),
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
					DEBUG("tm:%llu, OMDD BUY_SIDE match, code: %u, type: %c, matched_price: %d, matched_quantity: %llu\n",
							dbp::tools::srv::current(),
							uSecurityCode,
							static_cast<char>(type),
							matched_price,
							matched_quantity);
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
					DEBUG("tm:%llu, OMDD SELL_SIDE match, code: %u, type: %c, matched_price: %d, matched_quantity: %llu\n",
							dbp::tools::srv::current(),
							uSecurityCode,
							static_cast<char>(type),
							matched_price,
							matched_quantity);
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
#ifndef FULL_BOOK
		auto is_top = book.new_order(id, price, quantity, side);
		DEBUG("tm:%llu, OMDD Add Limit order, code: %u, type: %c, side: %u, price: %d, quantity: %u, id:%llu\n",
				dbp::tools::srv::current(),
				uSecurityCode,
				static_cast<char>(type),
				static_cast<unsigned int>(side),
				price,
				quantity,
				id);
		if (is_top)
		{
#else
		book.new_order(id, price, quantity, side);
#endif //ifndef FULL_BOOK
			rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
			if (OmddFullTickBook::OrderSide::BID == side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				if (!book.isCross())
				{
					broadcastQueue.enqueue(rOrderBook);
					DEBUG("tm:%llu, OMDD Send Book Bid by Add Order, code: %u, id: %llu\n",
						dbp::tools::srv::current(),
						uSecurityCode,
						id);
				}
			}
			else if (OmddFullTickBook::OrderSide::ASK == side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				if (!book.isCross())
				{
					broadcastQueue.enqueue(rOrderBook);
					DEBUG("tm:%llu, OMDD Send Book Ask by Add Order, code: %u, id: %llu\n",
						dbp::tools::srv::current(),
						uSecurityCode,
						id);
				}

			}
#ifndef FULL_BOOK
		}
#endif //ifndef FULL_BOOK
	}
	else if (331 == _pMsg->m_uMsgType)
	{
		auto& book = omddFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto price = OMD_GET_VALUE(_pMsg, 16, int);
		auto quantity = OMD_GET_VALUE(_pMsg, 20, unsigned int);
		auto result = book.modify_order(id, quantity, price);
		DEBUG("tm:%llu, OMDD Modify Order, code: %u, id: %llu, quantity:%u\n",
				dbp::tools::srv::current(),
				uSecurityCode,
				id,
				quantity);
#ifndef FULL_BOOK
		if (result.is_top)
		{
#endif //ifndef FULL_BOOK
			rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
			if (OmddFullTickBook::OrderSide::BID == result.side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
				DEBUG("tm:%llu, OMDD Send Book Bid by Modify Order, code: %u, id: %llu, quantity:%u\n",
						dbp::tools::srv::current(),
						uSecurityCode,
						id,
						quantity);
			}
			else if (OmddFullTickBook::OrderSide::ASK == result.side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
				DEBUG("tm:%llu, OMDD Send Book Ask by Modify Order, code: %u, id: %llu, quantity:%u\n",
						dbp::tools::srv::current(),
						uSecurityCode,
						id,
						quantity);
			}
#ifndef FULL_BOOK
		}
#endif //ifndef FULL_BOOK
	}
	else if (332 == _pMsg->m_uMsgType)
	{
		auto& book = omddFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto result = book.cancel_order(id);
		DEBUG("tm:%llu, OMDD Delete Order, code: %u, id: %llu\n",
				dbp::tools::srv::current(),
				uSecurityCode,
				id);
#ifndef FULL_BOOK
		if (result.is_top)
		{
#endif //ifndef FULL_BOOK
			rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
			if (OmddFullTickBook::OrderSide::BID == result.side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
				DEBUG("tm:%llu, OMDD Send Book Bid by Delete Order, code: %u, id: %llu\n",
						dbp::tools::srv::current(),
						uSecurityCode,
						id);
			}
			else if (OmddFullTickBook::OrderSide::ASK == result.side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
				DEBUG("tm:%llu, OMDD Send Book Ask by Delete Order, code: %u, id: %llu\n",
						dbp::tools::srv::current(),
						uSecurityCode,
						id);
			}
#ifndef FULL_BOOK
		}
#endif //ifndef FULL_BOOK
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
		DEBUG("tm:%llu, OMDD Clean OrderBook, code: %u, id: %llu",
				dbp::tools::srv::current(),
				uSecurityCode,
				id);
	}
#else
	if (353 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
		buildOmddOrderBook(_pMsg, rOrderBook);
		//std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		//std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
		std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, 2 * sizeof(OrderItem));
		std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, 2 * sizeof(OrderItem));
		rOrderBook.m_AccumulateBuyQuantity = 0;
		rOrderBook.m_AccumulateSellQuantity = 0;
		rOrderBook.m_AccumulateBlankQuantity = 0;
		broadcastQueue.enqueue(rOrderBook);
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
		broadcastQueue.enqueue(rOrderBook);
	}
#endif //DF
}

#endif


