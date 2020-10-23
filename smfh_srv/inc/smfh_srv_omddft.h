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
#ifdef DF
	COmdOrderbook& rOrderBook = it->second;
	if (330 == _pMsg->m_uMsgType)
	{

		auto& book = omddFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto price = OMD_GET_VALUE(_pMsg, 16, int);
		auto quantity = OMD_GET_VALUE(_pMsg, 20, unsigned int);
		auto side = OMD_GET_VALUE(_pMsg, 24, OmddFullTickBook::OrderSide);
		//auto rank = OMD_GET_VALUE(_pMsg, 28, unsigned int);
		//auto rest = quantity;




		DEBUG("tm:%llu, OMDD Add order, code: %u, side: %u, price: %d, quantity: %u \n ",
						dbp::tools::srv::current(),
						uSecurityCode,
						static_cast<unsigned int>(side),
						price,
						quantity);
		/*
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
		}*/
		auto is_top = book.omdd_new_order(id, price, quantity, side);
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
/*
			DEBUG("tm:%llu, OMDD Add order, code: %u, side: %u, price: %d, quantity: %u, rank: %u\n",
									dbp::tools::srv::current(),
									uSecurityCode,
									static_cast<unsigned int>(side),
									price,
									quantity,
									rank);
*/
		}
		DEBUG("tm:%llu, OMDD Add order Top, code: %u, BestBid: %d, BestAsk: %d\n",
			dbp::tools::srv::current(),
			uSecurityCode,
			rOrderBook.m_Bid[0].m_iPrice,
			rOrderBook.m_Ask[0].m_iPrice);
	}
	else if (331 == _pMsg->m_uMsgType)
	{
		auto& book = omddFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto price = OMD_GET_VALUE(_pMsg, 16, int);
		auto quantity = OMD_GET_VALUE(_pMsg, 20, unsigned int);
		auto side = OMD_GET_VALUE(_pMsg, 24, OmddFullTickBook::OrderSide);
		auto result = book.omdd_modify_order(id, quantity, price, side);
		if (result.is_top)
		{
			rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
			if (OmddFullTickBook::OrderSide::BID == side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}
			else if (OmddFullTickBook::OrderSide::ASK == side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}

			DEBUG("tm:%llu, OMDD Modify order Top, code: %u, BestBid: %d, BestAsk: %d\n",
							dbp::tools::srv::current(),
							uSecurityCode,
							rOrderBook.m_Bid[0].m_iPrice,
							rOrderBook.m_Ask[0].m_iPrice);
		}
	}
	else if (332 == _pMsg->m_uMsgType)
	{
		auto& book = omddFullTickBook[uSecurityCode];
		auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
		auto side = OMD_GET_VALUE(_pMsg, 16, OmddFullTickBook::OrderSide);
		//book.omdd_cancel_order(id, side);
		auto result = book.omdd_cancel_order(id, side);
		if (result.is_top)
		{
			rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
			if (OmddFullTickBook::OrderSide::BID == side)
			{
				ConvertFullBookToBookBid(book.Bids, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}
			else if (OmddFullTickBook::OrderSide::ASK == side)
			{
				ConvertFullBookToBookAsk(book.Asks, rOrderBook);
				rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
				broadcastQueue.enqueue(rOrderBook);
			}
		}
		DEBUG("tm:%llu, OMDD Delete order Top, code: %u, BestBid: %d, BestAsk: %d\n",
			dbp::tools::srv::current(),
			uSecurityCode,
			rOrderBook.m_Bid[0].m_iPrice,
			rOrderBook.m_Ask[0].m_iPrice);
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
	else if (350 == _pMsg->m_uMsgType)
		{
			//rOrderBook.m_MsgType = MsgType::OMDD_TRADE;

		auto& book = omddFullTickBook[uSecurityCode];

			unsigned long long m_LastTradeQuantity = static_cast<unsigned long long>(OMD_GET_VALUE(_pMsg, 40, unsigned int));
			auto id = OMD_GET_VALUE(_pMsg, 8, unsigned long long);
			int m_LastTradePrice = OMD_GET_VALUE(_pMsg, 16, int);
			unsigned short int m_TradeType = OMD_GET_VALUE(_pMsg, 33, unsigned short int);
			unsigned char omdd_side = OMD_GET_VALUE(_pMsg, 32, unsigned char);

			//auto side = OMD_GET_VALUE(_pMsg, 32, OmddFullTickBook::OrderSide);

			if(id > 0){
				if (2 == omdd_side)
				{

					rOrderBook.m_MsgType = MsgType::OMDD_TRADE;
					rOrderBook.m_LastTradeQuantity = m_LastTradeQuantity;
					rOrderBook.m_LastTradePrice = m_LastTradePrice;
					rOrderBook.m_TradeType = m_TradeType;
					rOrderBook.m_TradeSide = TradeSide::BUY_SIDE;
					rOrderBook.m_AccumulateBuyQuantity = rOrderBook.m_LastTradeQuantity;

					book.omdd_deduct_order(id,m_LastTradeQuantity, OmddFullTickBook::OrderSide::BID);

					//rOrderBook.m_MsgType = MsgType::OMDD_BOOK;

					ConvertFullBookToBookBid(book.Bids, rOrderBook);
					rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
					broadcastQueue.enqueue(rOrderBook);



					DEBUG("tm:%llu, OMDD Trade , code: %u, Price: %d, Side: BUY, Quantity : %llu, Type : %d  \n",
								dbp::tools::srv::current(),
								uSecurityCode,
								m_LastTradePrice,
								m_LastTradeQuantity,
								m_TradeType);
				}
				else if (3 == omdd_side)
				{

					rOrderBook.m_MsgType = MsgType::OMDD_TRADE;
					rOrderBook.m_LastTradeQuantity = m_LastTradeQuantity;
					rOrderBook.m_LastTradePrice = m_LastTradePrice;
					rOrderBook.m_TradeType = m_TradeType;
					rOrderBook.m_TradeSide = TradeSide::SELL_SIDE;
					rOrderBook.m_AccumulateSellQuantity = rOrderBook.m_LastTradeQuantity;
					broadcastQueue.enqueue(rOrderBook);

					book.omdd_deduct_order(id,m_LastTradeQuantity, OmddFullTickBook::OrderSide::ASK);
					//rOrderBook.m_MsgType = MsgType::OMDD_BOOK;

					ConvertFullBookToBookAsk(book.Asks, rOrderBook);
					rOrderBook.m_MsgType = MsgType::OMDD_BOOK;
					broadcastQueue.enqueue(rOrderBook);

					DEBUG("tm:%llu, OMDD Trade ID>0, code: %u, Price: %d, Side: SELL, Quantity : %llu, Type : %d  \n",
								dbp::tools::srv::current(),
								uSecurityCode,
								m_LastTradePrice,
								m_LastTradeQuantity,
								m_TradeType);
				}
			}else{
				DEBUG("tm:%llu, OMDD Trade ID=0, code: %u, Price: %d, Side: SELL, Quantity : %llu, Type : %d  \n",
							dbp::tools::srv::current(),
							uSecurityCode,
							m_LastTradePrice,
							m_LastTradeQuantity,
							m_TradeType);
			}

		}
#endif //DF
}

#endif


