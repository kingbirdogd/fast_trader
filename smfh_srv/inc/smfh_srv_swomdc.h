#ifndef __SMFH_SRV_SWOMDC__
#define __SMFH_SRV_SWOMDC__
#include <tools.h>
#include <global_memory.hpp>
#include "smfh_srv_orderbook.h"
#ifndef NOT_MEASURE
inline static void handleStockWarrantOmdc(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm)
#else
inline static void handleStockWarrantOmdc(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long)
#endif
{
	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
/*
	auto it = omdcSlimMap.find(uSecurityCode);
	if (omdcSlimMap.end() == it)
	{
		return;
	}
	*/

	auto it = stockWarrantomdcMap.find(uSecurityCode);
	if (stockWarrantomdcMap.end() == it)
	{
		return;
	}



	COmdOrderbook& rOrderBook = it->second;
#ifndef NOT_MEASURE
	rOrderBook.m_PkgTime = _uPkgTm;
	rOrderBook.m_MsgTime = dbp::tools::srv::current();
#endif
#ifdef SF
	if (30 == _pMsg->m_uMsgType)
	{
		auto& book = omdcFullTickBook[uSecurityCode];
		auto type = OMD_GET_VALUE(_pMsg, 26, OmdcFullTickBook::OrderType);
		auto side = OMD_GET_VALUE(_pMsg, 24, OmdcFullTickBook::OrderSide);
		auto price = OMD_GET_VALUE(_pMsg, 16, int);
		auto quantity = OMD_GET_VALUE(_pMsg, 20, unsigned int);
		auto rest = quantity;
		if(uSecurityCode<10000){

			if (OmdcFullTickBook::OrderSide::BID == side)
			{

				auto it = book.Ask.begin();
				if(it->first <= price || OmdcFullTickBook::OrderType::Market == type)
				{
					auto matched_price = it->first;
					auto book_quantity = it->second.quantity;
					auto matched_quantity = rest < book_quantity ? rest : book_quantity;
					rOrderBook.m_MsgType = MsgType::OMDC_TRADE;
					rOrderBook.m_LastTradeQuantity = matched_quantity;
					rOrderBook.m_LastTradePrice = matched_price;
					rOrderBook.m_TradeType = 0;
					rOrderBook.m_TradeSide = TradeSide::BUY_SIDE;
					rOrderBook.m_AccumulateBuyQuantity += rOrderBook.m_LastTradeQuantity;
					broadcastQueue.enqueue(rOrderBook);
				}

				/*
				for (auto it = book.Asks.begin(); it != book.Asks.end(); ++it)
				//for (auto it = book.Bids.begin(); it != book.Bids.end(); ++it)
				{
					if (it->first <= price || OmdcFullTickBook::OrderType::Market == type)
					{
						auto matched_price = it->first;
						auto book_quantity = it->second.quantity;
						auto matched_quantity = rest < book_quantity ? rest : book_quantity;
						rOrderBook.m_MsgType = MsgType::OMDC_TRADE;
						rOrderBook.m_LastTradeQuantity = matched_quantity;
						rOrderBook.m_LastTradePrice = matched_price;
						rOrderBook.m_TradeType = 0;
						rOrderBook.m_TradeSide = TradeSide::BUY_SIDE;
						rOrderBook.m_AccumulateBuyQuantity += rOrderBook.m_LastTradeQuantity;
						//if(uSecurityCode<10000){
						broadcastQueue.enqueue(rOrderBook);
						//}
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
				}*/
			}
			else
			{
				auto it = book.Bids.begin();
				if (it->first >= price || OmdcFullTickBook::OrderType::Market == type)
				{
					auto matched_price = it->first;
					auto book_quantity = it->second.quantity;
					auto matched_quantity = rest < book_quantity ? rest : book_quantity;
					rOrderBook.m_MsgType = MsgType::OMDC_TRADE;
					rOrderBook.m_LastTradeQuantity = matched_quantity;
					rOrderBook.m_LastTradePrice = matched_price;
					rOrderBook.m_TradeType = 0;
					rOrderBook.m_TradeSide = TradeSide::SELL_SIDE;
					rOrderBook.m_AccumulateSellQuantity += rOrderBook.m_LastTradeQuantity;
					//if(uSecurityCode<10000){
					broadcastQueue.enqueue(rOrderBook);
				}
				/*
				for (auto it = book.Bids.begin(); it != book.Bids.end(); ++it)
				//for (auto it = book.Asks.begin(); it != book.Asks.end(); ++it)
				{
					if (it->first >= price || OmdcFullTickBook::OrderType::Market == type)
					{
						auto matched_price = it->first;
						auto book_quantity = it->second.quantity;
						auto matched_quantity = rest < book_quantity ? rest : book_quantity;
						rOrderBook.m_MsgType = MsgType::OMDC_TRADE;
						rOrderBook.m_LastTradeQuantity = matched_quantity;
						rOrderBook.m_LastTradePrice = matched_price;
						rOrderBook.m_TradeType = 0;
						rOrderBook.m_TradeSide = TradeSide::SELL_SIDE;
						rOrderBook.m_AccumulateSellQuantity += rOrderBook.m_LastTradeQuantity;
						//if(uSecurityCode<10000){
						broadcastQueue.enqueue(rOrderBook);
						//}
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
				}*/
			}
		}
		if (OmdcFullTickBook::OrderType::Limit == type)
		{
			auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
#ifndef FULL_BOOK
			auto is_top = book.new_order(id, price, quantity, side);
			if (is_top)
			{
#else
			book.new_order(id, price, quantity, side);
#endif //ifndef FULL_BOOK
				rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
				if (OmdcFullTickBook::OrderSide::BID == side)
				{
					if (!book.isCross())
					{
						ConvertFullBookToBookBid(book.Bids, rOrderBook);
						rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
						broadcastQueue.enqueue(rOrderBook);
					}
				}
				else if (OmdcFullTickBook::OrderSide::ASK == side)
				{
					if (!book.isCross())
					{
						ConvertFullBookToBookAsk(book.Asks, rOrderBook);
						rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
						broadcastQueue.enqueue(rOrderBook);
					}
				}
#ifndef FULL_BOOK
			}
#endif //ifndef FULL_BOOK
		}
	}
	else if (31 == _pMsg->m_uMsgType)
	{
		auto& book = omdcFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto quantity = OMD_GET_VALUE(_pMsg, 16, unsigned int);
		auto result = book.modify_order(id, quantity);
#ifndef FULL_BOOK
		if (result.is_top)
		{
#endif //ifndef FULL_BOOK
			rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
			if (OmdcFullTickBook::OrderSide::BID == result.side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}
			else if (OmdcFullTickBook::OrderSide::ASK == result.side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}
#ifndef FULL_BOOK
		}
#endif //ifndef FULL_BOOK
	}
	else if (32 == _pMsg->m_uMsgType)
	{
		auto& book = omdcFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto result = book.cancel_order(id);
		rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
#ifndef FULL_BOOK
		if (result.is_top)
		{
#endif //ifndef FULL_BOOK
			if (OmdcFullTickBook::OrderSide::BID == result.side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
					broadcastQueue.enqueue(rOrderBook);
			}
			else if (OmdcFullTickBook::OrderSide::ASK == result.side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
					broadcastQueue.enqueue(rOrderBook);
			}
#ifndef FULL_BOOK
		}
#endif //ifndef FULL_BOOK
	}



#else
	if (53 == _pMsg->m_uMsgType)
	{
		rOrderBook.m_MsgType = MsgType::OMDC_BOOK;
		unsigned char updatelvl = buildSlimOmdcOrderBook(_pMsg, rOrderBook);

		rOrderBook.m_AccumulateBuyQuantity = 0;
		rOrderBook.m_AccumulateSellQuantity = 0;
		rOrderBook.m_AccumulateBlankQuantity = 0;
		//if(updatelvl <= TRADABLE_BOOK_SIZE ){
		if(updatelvl <= TRADABLE_BOOK_SIZE){
			std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
			std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, TRADABLE_BOOK_SIZE * sizeof(OrderItem));
			//std::memcpy(rOrderBook.m_Bid, rOrderBook.m_BidOrder, 3 * sizeof(OrderItem));
			//std::memcpy(rOrderBook.m_Ask, rOrderBook.m_AskOrder, 3 * sizeof(OrderItem));
			broadcastQueue.enqueue(rOrderBook);
		}
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
		if(uSecurityCode<10000){
			broadcastQueue.enqueue(rOrderBook);
		}
	}
#endif //SF
}
#endif


