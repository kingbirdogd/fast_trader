#ifndef __SMFH_SRV_OMDC__
#define __SMFH_SRV_OMDC__
#include <srv/tools.h>
#include <srv/macro.h>
#include <spin.h>
#include "smfh_srv_orderbook.h"
inline static void handleOmdc(dbp::omd::COmdMsgHeader* _pMsg, unsigned int _uSeq, unsigned long long _uChannelIdx, unsigned long long _uPkgTm)
{
	if (53 == _pMsg->m_uMsgType)
	{
		unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
		COmdOmdcOrderMap::iterator it = omdcMap.find(uSecurityCode);
		if (omdcMap.end() == it)
		{
			return;
		}
		COmdOrderbook& rOrderBook = it->second.m_SPOrderBook;
		bool bBidFlag = false;
		bool bAskFlag = false;
		buildOmdcOrderBook(_pMsg, rOrderBook, bBidFlag, bAskFlag);
		bool bSpecialSpread = (specialSpreadTableSet.end() != specialSpreadTableSet.find(uSecurityCode));
		dbp::shm::Tradable& tradable = shm_Ref.m_pOmdcTradable[rOrderBook.m_uIdx].m_Tradable;
		dbp::spin::spin_unique_lock lock(tradable.m_Mutex);
		tradable.m_uQueueIdx = _uChannelIdx;
		tradable.m_uOrderSeq = _uSeq;
		++tradable.m_uOrderCnt;
		tradable.m_uTm = _uPkgTm;
		tradable.m_uOrderBookUpdateFlag = 0;
		tradable.m_uAccumulateSellQuantity = 0;
		tradable.m_uAccumulateBuyQuantity = 0;
		tradable.m_uAccumulateBlankQuantity = 0;
		if (bBidFlag)
		{
			CHECK_FIRST_BID_AND_SET(tradable.m_uOrderBookUpdateFlag, tradable.m_Bid[0], rOrderBook.m_BidOrder[0]);
			int iNextPrice = rOrderBook.m_BidOrder[0].m_iPrice;
			if (bSpecialSpread)
			{
				if (500 == iNextPrice)
				{
					iNextPrice = 0;
				}
				else
				{
					iNextPrice -= ((0 == iNextPrice) ? 0 : 50);
				}
			}
			else
			{
				if (10 == iNextPrice)
				{
					iNextPrice = 0;
				}
				else
				{
					iNextPrice -= dbp::tools::srv::NextBidDiff(iNextPrice);
				}
			}
			unsigned int uIdx = 1;
			for (unsigned int i = 1; i < 10; ++i)
			{
				dbp::shm::OrderItem item;
				item.m_iPrice = iNextPrice;
				if (rOrderBook.m_BidOrder[uIdx].m_iPrice != iNextPrice)
				{
					item.m_uQuantity = 0;
					item.m_uNumberOfOrder = 0;
				}
				else
				{
					item.m_uQuantity = rOrderBook.m_BidOrder[uIdx].m_uQuantity;
					item.m_uNumberOfOrder = rOrderBook.m_BidOrder[uIdx].m_uNumberOfOrder;
					++uIdx;
				}
				if (bSpecialSpread)
				{
					if (500 == iNextPrice)
					{
						iNextPrice = 0;
					}
					else
					{
						iNextPrice -= ((0 == iNextPrice) ? 0 : 50);
					}
				}
				else
				{
					if (10 == iNextPrice)
					{
						iNextPrice = 0;
					}
					else
					{
						iNextPrice -= dbp::tools::srv::NextBidDiff(iNextPrice);
					}
				}
				CHECK_BID_AND_SET(tradable.m_uOrderBookUpdateFlag, tradable.m_Bid[i], item, i);
			}
		}
		if (bAskFlag)
		{
			CHECK_FIRST_ASK_AND_SET(tradable.m_uOrderBookUpdateFlag, tradable.m_Ask[0], rOrderBook.m_AskOrder[0]);
			int iNextPrice = rOrderBook.m_AskOrder[0].m_iPrice;
			if (9995000 == iNextPrice)
			{
				iNextPrice = 0;
			}
			else
			{
				iNextPrice += (bSpecialSpread) ? ((0 == iNextPrice) ? 0 : 50) : dbp::tools::srv::NextAskDiff(iNextPrice);
			}
			unsigned int uIdx = 1;
			for (unsigned int i = 1; i < 10; ++i)
			{
				dbp::shm::OrderItem item;
				item.m_iPrice = iNextPrice;
				if (rOrderBook.m_AskOrder[uIdx].m_iPrice != iNextPrice)
				{
					item.m_uQuantity = 0;
					item.m_uNumberOfOrder = 0;
				}
				else
				{
					item.m_uQuantity = rOrderBook.m_AskOrder[uIdx].m_uQuantity;
					item.m_uNumberOfOrder = rOrderBook.m_AskOrder[uIdx].m_uNumberOfOrder;
					++uIdx;
				}
				if (9995000 == iNextPrice)
				{
					iNextPrice = 0;
				}
				else
				{
					iNextPrice += (bSpecialSpread) ? ((0 == iNextPrice) ? 0 : 50) : dbp::tools::srv::NextAskDiff(iNextPrice);
				}
				CHECK_ASK_AND_SET(tradable.m_uOrderBookUpdateFlag, tradable.m_Ask[i], item, i);
			}
		}
		if (0 != (tradable.m_uOrderBookUpdateFlag & tradable.m_uSubscribeFlag))
		{
			dbp::shm::QueueHeader& queueHeader = (*(shm_Ref.m_vecOmdcQueueHeader[_uChannelIdx]));
			dbp::shm::TradableQueueItem* pQueue = shm_Ref.m_vecOmdcQueue[_uChannelIdx];
			dbp::shm::TradableQueueItem& rQueue = pQueue[queueHeader.m_uIdx % queueHeader.m_uQueueSize];
			dbp::spin::spin_unique_lock lock_queue(rQueue.m_Mutex);
			rQueue.m_uEventFlag = tradable.m_uOrderBookUpdateFlag;
			rQueue.m_uCode = uSecurityCode;
			rQueue.m_uTSequence = 0;
			rQueue.m_uOSequence = _uSeq;
			++queueHeader.m_uIdx;
		}
	}
	else if (50 == _pMsg->m_uMsgType)
	{
		unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
		COmdOmdcOrderMap::iterator it = omdcMap.find(uSecurityCode);
		if (omdcMap.end() == it)
		{
			return;
		}
		COmdOrderbook& rOrderBook = it->second.m_SPOrderBook;
		dbp::shm::Tradable& tradable = shm_Ref.m_pOmdcTradable[rOrderBook.m_uIdx].m_Tradable;
		dbp::spin::spin_unique_lock lock(tradable.m_Mutex);
		tradable.m_uQueueIdx = _uChannelIdx;
		tradable.m_uTradeSeq = _uSeq;
		++tradable.m_uTradeCnt;
		tradable.m_uTm = _uPkgTm;
		tradable.m_uTradeTime = OMD_GET_VALUE(_pMsg, 24, unsigned long long);
		tradable.m_uTradeQuanty = (unsigned long long)(OMD_GET_VALUE(_pMsg, 16, unsigned int));
		tradable.m_iTradePrice = OMD_GET_VALUE(_pMsg, 12, int);
		tradable.m_iTradeType = OMD_GET_VALUE(_pMsg, 20, unsigned short int);
		unsigned long long uAccumulatedQuantity = 0;
		if (tradable.m_iTradePrice <= tradable.m_Bid[0].m_iPrice)
		{
			tradable.m_iTradeSide = 1;
			tradable.m_uAccumulateSellQuantity += tradable.m_uTradeQuanty;
			uAccumulatedQuantity = tradable.m_uAccumulateSellQuantity;
		}
		else if (tradable.m_iTradePrice >= tradable.m_Ask[0].m_iPrice)
		{
			tradable.m_iTradeSide = -1;
			tradable.m_uAccumulateBuyQuantity += tradable.m_uTradeQuanty;
			uAccumulatedQuantity = tradable.m_uAccumulateBuyQuantity;
		}
		else
		{
			tradable.m_iTradeSide = 0;
			tradable.m_uAccumulateBlankQuantity += tradable.m_uTradeQuanty;
			uAccumulatedQuantity = tradable.m_uAccumulateBlankQuantity;
		}
		auto idx = tradable.m_uLastTradeIdx % tradable.m_TradeRecord.size();
		++tradable.m_uLastTradeIdx;
		dbp::shm::TradeItem& item = tradable.m_TradeRecord[idx];
		item.m_Bid = tradable.m_Bid[0];
		item.m_Ask = tradable.m_Ask[0];
		item.m_uTm = tradable.m_uTradeTime;
		item.m_uQuantity = tradable.m_uTradeQuanty;
		item.m_uAccumulatedQuantity = uAccumulatedQuantity;
		item.m_uSeq = tradable.m_uTradeSeq;
		item.m_uTradeCnt = tradable.m_uTradeCnt;
		item.m_iTradeType = tradable.m_iTradeType;
		item.m_iTradeSide = tradable.m_iTradeSide;
		item.m_iPrice = tradable.m_iTradePrice;
		//if (CHECK_TRADE_FLAG(tradable))
		if (0 != (dbp::shm::FlagTradeMask & tradable.m_uSubscribeFlag))
		{
			dbp::shm::QueueHeader& queueHeader = (*(shm_Ref.m_vecOmdcQueueHeader[_uChannelIdx]));
			dbp::shm::TradableQueueItem* pQueue = shm_Ref.m_vecOmdcQueue[_uChannelIdx];
			dbp::shm::TradableQueueItem& rQueue = pQueue[(queueHeader.m_uIdx) % queueHeader.m_uQueueSize];
			dbp::spin::spin_unique_lock lock_queue(rQueue.m_Mutex);
			rQueue.m_uEventFlag = dbp::shm::FlagTradeMask;
			rQueue.m_uCode = uSecurityCode;
			rQueue.m_uTSequence = _uSeq;
			rQueue.m_uOSequence = 0;
			rQueue.m_uTradeIdx = idx;
			++queueHeader.m_uIdx;
		}
	}
}
#endif


