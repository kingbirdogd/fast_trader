#ifndef __SMFH_SRV_BROKER__
#define __SMFH_SRV_BROKER__
#include <srv/tools.h>
#include <srv/macro.h>
#include <spin.h>
#include "smfh_srv_orderbook.h"
inline static void handleBroker(dbp::omd::COmdMsgHeader* _pMsg, unsigned int _uSeq, unsigned long long, unsigned long long _uPkgTm)
{
//	flush_printf("----> _uSeq = %u\n", _uSeq);

	unsigned int uSecurityCode = OMD_GET_VALUE(_pMsg, 4, unsigned int);

	COmdOmdcOrderMap::iterator it = omdcMap.find(uSecurityCode);
	if (omdcMap.end() == it)
	{
		return;
	}

	if (0 != brokerStream.m_uLastWaitOrderBookCode)
	{
		if (brokerStream.m_uLastWaitOrderBookCode != uSecurityCode)
		{
//			flush_printf("----> Change b4 uSecurityCode = %u\n", brokerStream.m_uLastWaitOrderBookCode);
//			flush_printf("----> Change af uSecurityCode = %u\n", uSecurityCode);
			brokerStream.m_bCodeChange = true;
			brokerStream.m_uLastWaitOrderBookCode = uSecurityCode;
		}
	}
	else
	{
//		flush_printf("----> New uSecurityCode = %u\n", uSecurityCode);
		brokerStream.m_uLastWaitOrderBookCode = uSecurityCode;
	}

//	flush_printf("----> m_bCodeChange = %d\n", (int)brokerStream.m_bCodeChange);
//	flush_printf("----> m_bDataUpdate = %d\n", (int)brokerStream.m_bDataUpdate);

	if (brokerStream.m_bCodeChange)
	{
		if(brokerStream.m_bDataUpdate)
		{
			dbp::shm::TradablePosition& position = shm_Ref.m_pOmdcTradable[brokerStream.m_iLastSPOrderBookIdx].m_Position;

//			flush_printf("----> m_uSbuscribeFlag = %d\n", (int)position.m_uSbuscribeFlag);

			if(1 == position.m_uSbuscribeFlag)
			{
				dbp::shm::QueueHeader& queueHeader = (*(shm_Ref.m_pBrokerPositionQueueHeader));
				dbp::shm::PositionQueueItem& rQueue = shm_Ref.m_pPositionQueue[(queueHeader.m_uIdx) % queueHeader.m_uQueueSize];
				dbp::spin::spin_unique_lock lock_queue(rQueue.m_Mutex);
				rQueue.m_uCode = brokerStream.m_uLastWaitOrderBookCode;
				rQueue.m_uOrderBookSequence = position.m_uOrderBookSeq;
				rQueue.m_uBrokerQueueSequence = position.m_uBrokerQueueSeq;
				++queueHeader.m_uIdx;
			}
		}

		brokerStream.m_bCodeChange = false;
		brokerStream.m_bDataUpdate = false;
	}

	if((uSecurityCode >= 10000 && uSecurityCode < 30000) || (uSecurityCode >= 57000 && uSecurityCode < 70000) || (uSecurityCode >= 89000 && uSecurityCode < 90000))
	{
		// store last SP Idx
		brokerStream.m_iLastSPOrderBookIdx = it->second.m_SPOrderBook.m_uIdx;
//		flush_printf("----> brokerStream.m_iLastSPOrderBookIdx = %u\n", brokerStream.m_iLastSPOrderBookIdx);
	}
	else
	{
		return;
	}

	COmdMemOrderbook& rOrderBook = it->second.m_SSOrderBook;
	dbp::shm::TradablePosition& position = shm_Ref.m_pOmdcTradable[it->second.m_SPOrderBook.m_uIdx].m_Position;

	if (53 == _pMsg->m_uMsgType)
	{
		buildOmdcOrderBookNoFlag(_pMsg, rOrderBook);

		dbp::spin::spin_unique_lock lock(position.m_Mutex);
		position.m_uOrderBookTimeStamp = _uPkgTm;
		position.m_uOrderBookSeq = _uSeq;
		position.m_uLastSeq = _uSeq;
		position.m_iBestBid = rOrderBook.m_BidOrder[0].m_iPrice;
		position.m_iBestAsk = rOrderBook.m_AskOrder[0].m_iPrice;

//		flush_printf("----> in 53, position.m_uOrderBookTimeStamp = %llu\n", position.m_uOrderBookTimeStamp);
//		flush_printf("----> in 53, position.m_uOrderBookSeq = %u\n", position.m_uOrderBookSeq);
//		flush_printf("----> in 53, position.m_uLastSeq = %u\n", position.m_uLastSeq);
//		flush_printf("----> in 53, position.m_iBestBid = %u\n", position.m_iBestBid);
//		flush_printf("----> in 53, position.m_iBestAsk = %u\n", position.m_iBestAsk);

		brokerStream.m_bDataUpdate = true;
	}
	else if (54 == _pMsg->m_uMsgType)
	{
		dbp::spin::spin_unique_lock lock(position.m_Mutex);
		position.m_uBrokerQueueTimeStmap = _uPkgTm;
		position.m_uFirstCome = 1;
		position.m_uBrokerQueueSeq = _uSeq;
		position.m_uLastSeq = _uSeq;

//		flush_printf("----> in 54, position.m_uBrokerQueueTimeStamp=%llu\n", position.m_uBrokerQueueTimeStmap);
//		flush_printf("----> in 54, position.m_uBrokerQueueSeq=%u\n", position.m_uBrokerQueueSeq);
//		flush_printf("----> in 54, position.m_uLastSeq=%u\n", position.m_uLastSeq);

		unsigned char uCnt = OMD_GET_VALUE(_pMsg, 8, unsigned char);
		unsigned int uSide = OMD_GET_VALUE(_pMsg, 9, unsigned short int);

//		flush_printf("----> in 54, uCnt=%d\n", uCnt);
//		flush_printf("----> in 54, uSide=%d\n", uSide);

		bool bLPFind = false;
		bool bCISFind = false;
		bool bRequireReset = false;

		if (1 == uSide)
		{
			memcpy (&position.m_BidBQArray, ((unsigned char*)((void*)_pMsg)) + 12, sizeof(dbp::shm::BrokerQueue) * uCnt);
			char* pszMsgPointer = (char*)_pMsg + 12;

			int brokerPosition = 1;
			int border = 0;

			for (unsigned char i = 0; i < uCnt; ++i)
			{
				int iItem = (int)(position.m_BidBQArray[i].m_uItem);

//				flush_printf("----> in 54.1, iItem=%u\n", iItem);
//				flush_printf("----> in 54.1, bLPFind=%d\n", (int)bLPFind);
//				flush_printf("----> in 54.1, bCISFind=%d\n", (int)bCISFind);

				if ('B' == position.m_BidBQArray[i].m_cType)
				{
					border++;
					if (!bLPFind && iItem >= 9500 && iItem < 9800)
					{
						position.m_LPBestBid.m_iBrokerId = iItem;
						position.m_LPBestBid.m_iBorkerPosition = brokerPosition;
						position.m_LPBestBid.m_iBrokerOrder = border;
						bLPFind = true;
					}

					if (!bCISFind && (brokerStream.m_CISBrokerIDSet.end() != brokerStream.m_CISBrokerIDSet.find(iItem)))
					{
//						flush_printf("----> in 54.1, insideCIS iItem=%u\n", iItem);
//						flush_printf("----> in 54.1, insideCIS i=%u\n", i);

						position.m_CISBestBid.m_iBrokerId = iItem;
						position.m_CISBestBid.m_iBorkerPosition = brokerPosition;
						position.m_CISBestBid.m_iBrokerOrder = border;
						bCISFind = true;
					}

					if (bLPFind && bCISFind)
					{
						break;
					}
				}
				else if ('S' == position.m_BidBQArray[i].m_cType)
				{
					brokerPosition = iItem + 1;
					border=0;
				}

				pszMsgPointer += 4;
			}

			if(!bLPFind)
			{
				position.m_LPBestBid.m_iBrokerId = -1;
				position.m_LPBestBid.m_iBorkerPosition = 0;
				position.m_LPBestBid.m_iBrokerOrder = 0;
				bRequireReset = true;
			}

			if(!bCISFind)
			{
				position.m_CISBestBid.m_iBrokerId = -1;
				position.m_CISBestBid.m_iBorkerPosition = 0;
				position.m_CISBestBid.m_iBrokerOrder = 0;
				bRequireReset = true;
			}

//			flush_printf("----> in 54.1a, bLPFind=%d\n", (int)bLPFind);
//			flush_printf("----> in 54.1a, bCISFind=%d\n", (int)bCISFind);
//			flush_printf("----> in 54.1a, bRequireReset=%d\n", (int)bRequireReset);
//			flush_printf("----> in 54.1a, m_LPBestBid.m_iBrokerId=%d\n", position.m_LPBestBid.m_iBrokerId);
//			flush_printf("----> in 54.1a, m_LPBestBid.m_iBorkerPosition=%u\n", position.m_LPBestBid.m_iBorkerPosition);
//			flush_printf("----> in 54.1a, m_CISBestBid.m_iBrokerId=%d\n", position.m_CISBestBid.m_iBrokerId);
//			flush_printf("----> in 54.1a, m_CISBestBid.m_iBorkerPosition=%u\n", position.m_CISBestBid.m_iBorkerPosition);
		}
		else if (2 == uSide)
		{
			memcpy (&position.m_AskBQArray, ((unsigned char*)((void*)_pMsg)) + 12, sizeof(dbp::shm::BrokerQueue) * uCnt);
			char* pszMsgPointer = (char*)_pMsg + 12;

			int brokerPosition = 1;
			int border = 0;
			for (unsigned char i = 0; i < uCnt; ++i)
			{
				int iItem = (int)(position.m_AskBQArray[i].m_uItem);

//				flush_printf("----> in 54.2, iItem=%u\n", iItem);
//				flush_printf("----> in 54.2, bLPFind=%d\n", (int)bLPFind);
//				flush_printf("----> in 54.2, bCISFind=%d\n", (int)bCISFind);

				if ('B' == position.m_AskBQArray[i].m_cType)
				{
					border++;
					if (!bLPFind && iItem >= 9500 && iItem < 9800)
					{
						position.m_LPBestAsk.m_iBrokerId = iItem;
						position.m_LPBestAsk.m_iBorkerPosition = brokerPosition;
						position.m_LPBestAsk.m_iBrokerOrder = border;
						bLPFind = true;
					}

					if (!bCISFind && (brokerStream.m_CISBrokerIDSet.end() != brokerStream.m_CISBrokerIDSet.find(iItem)))
					{
//						flush_printf("----> in 54.2, insideCIS iItem=%u\n", iItem);
//						flush_printf("----> in 54.2, insideCIS i=%u\n", i);

						position.m_CISBestAsk.m_iBrokerId = iItem;
						position.m_CISBestAsk.m_iBorkerPosition = brokerPosition;
						position.m_CISBestAsk.m_iBrokerOrder = border;
						bCISFind = true;
					}

					if (bLPFind && bCISFind)
					{
						break;
					}
				}
				else if ('S' == position.m_AskBQArray[i].m_cType)
				{
					brokerPosition = iItem + 1;
					border=0;
				}

				pszMsgPointer += 4;
			}

			if(!bLPFind)
			{
				position.m_LPBestAsk.m_iBrokerId = -1;
				position.m_LPBestAsk.m_iBorkerPosition = 0;
				position.m_LPBestAsk.m_iBrokerOrder = 0;
				bRequireReset = true;
			}

			if(!bCISFind)
			{
				position.m_CISBestAsk.m_iBrokerId = -1;
				position.m_CISBestAsk.m_iBorkerPosition = 0;
				position.m_CISBestAsk.m_iBrokerOrder = 0;
				bRequireReset = true;
			}

//			flush_printf("----> in 54.2a, bLPFind=%d\n", (int)bLPFind);
//			flush_printf("----> in 54.2a, bCISFind=%d\n", (int)bCISFind);
//			flush_printf("----> in 54.2a, bRequireReset = %d\n", (int)bRequireReset);
//			flush_printf("----> in 54.2a, m_LPBestAsk.m_iBrokerId=%d\n", position.m_LPBestAsk.m_iBrokerId);
//			flush_printf("----> in 54.2a, m_LPBestAsk.m_iBorkerPosition=%u\n", position.m_LPBestAsk.m_iBorkerPosition);
//			flush_printf("----> in 54.2a, m_CISBestAsk.m_iBrokerId=%d\n", position.m_CISBestAsk.m_iBrokerId);
//			flush_printf("----> in 54.2a, m_CISBestAsk.m_iBorkerPosition=%u\n", position.m_CISBestAsk.m_iBorkerPosition);
		}

		if(bLPFind || bCISFind || bRequireReset)
		{
			brokerStream.m_bDataUpdate = true;
		}
	}
}

#endif


