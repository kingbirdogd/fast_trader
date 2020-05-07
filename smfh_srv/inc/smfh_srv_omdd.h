#ifndef __SMFH_SRV_OMDD__
#define __SMFH_SRV_OMDD__
#include <srv/tools.h>
#include <srv/macro.h>
#include <spin.h>
#include "smfh_srv_orderbook.h"
inline static void handleOmdd(dbp::omd::COmdMsgHeader* _pMsg, unsigned int _uSeq, unsigned long long _uChannelIdx, unsigned long long _uPkgTm)
{
	if (353 == _pMsg->m_uMsgType)
	{
		unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
		COmdOrderMap::iterator it = omddMap.find(uSecurityCode);
		if (omddMap.end() == it)
		{
			return;
		}
		COmdOrderbook& rOrderBook = it->second;
		bool bBidFlag = false;
		bool bAskFlag = false;
		buildOmddOrderBook(_pMsg, rOrderBook, bBidFlag, bAskFlag);
		dbp::shm::Tradable& tradable = shm_Ref.m_pOmddTradable[rOrderBook.m_uIdx];
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
			CHECK_BID_REST_AND_SET(tradable.m_uOrderBookUpdateFlag, tradable.m_Bid, rOrderBook.m_BidOrder);
		}
		if (bAskFlag)
		{
			CHECK_FIRST_ASK_AND_SET(tradable.m_uOrderBookUpdateFlag, tradable.m_Ask[0], rOrderBook.m_AskOrder[0]);
			CHECK_ASK_REST_AND_SET(tradable.m_uOrderBookUpdateFlag, tradable.m_Ask, rOrderBook.m_AskOrder);
		}
		if (0 != (tradable.m_uOrderBookUpdateFlag & tradable.m_uSubscribeFlag))
		{
			dbp::shm::QueueHeader& queueHeader = (*(shm_Ref.m_vecOmddQueueHeader[_uChannelIdx]));
			dbp::shm::TradableQueueItem* pQueue = shm_Ref.m_vecOmddQueue[_uChannelIdx];
			dbp::shm::TradableQueueItem& rQueue = pQueue[(queueHeader.m_uIdx) % queueHeader.m_uQueueSize];
			dbp::spin::spin_unique_lock lock_queue(rQueue.m_Mutex);
			rQueue.m_uEventFlag = tradable.m_uOrderBookUpdateFlag;
			rQueue.m_uCode = uSecurityCode;
			rQueue.m_uTSequence = 0;
			rQueue.m_uOSequence = _uSeq;
			++queueHeader.m_uIdx;
		}
	}
	else if (350 == _pMsg->m_uMsgType)
	{
		unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);
		COmdOrderMap::iterator it = omddMap.find(uSecurityCode);
		if (omddMap.end() == it)
		{
			return;
		}
		COmdOrderbook& rOrderBook = it->second;
		dbp::shm::Tradable& tradable = shm_Ref.m_pOmddTradable[rOrderBook.m_uIdx];
		dbp::spin::spin_unique_lock lock(tradable.m_Mutex);
		tradable.m_uQueueIdx = _uChannelIdx;
		tradable.m_uTradeSeq = _uSeq;
		++tradable.m_uTradeCnt;
		tradable.m_uTm = _uPkgTm;
		tradable.m_uTradeTime = OMD_GET_VALUE(_pMsg, 48, unsigned long long);
		tradable.m_uTradeQuanty = OMD_GET_VALUE(_pMsg, 40, unsigned long long);
		tradable.m_iTradePrice = OMD_GET_VALUE(_pMsg, 16, int);
		tradable.m_iTradeType = OMD_GET_VALUE(_pMsg, 33, unsigned char);
		unsigned char omdd_side = OMD_GET_VALUE(_pMsg, 32, unsigned char);
		unsigned long long uAccumulatedQuantity = 0;
		if (2 == omdd_side)
		{
			tradable.m_iTradeSide = 1;
			tradable.m_uAccumulateSellQuantity += tradable.m_uTradeQuanty;
			uAccumulatedQuantity = tradable.m_uAccumulateSellQuantity;
		}
		else if (3 == omdd_side)
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
			dbp::shm::QueueHeader& queueHeader = (*(shm_Ref.m_vecOmddQueueHeader[_uChannelIdx]));
			dbp::shm::TradableQueueItem* pQueue = shm_Ref.m_vecOmddQueue[_uChannelIdx];
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


