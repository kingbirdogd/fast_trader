#ifndef __DBP_SHM_CLI__
#define __DBP_SHM_CLI__
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <dbp_shm.h>
namespace dbp
{
	namespace shm
	{
		namespace cli 
		{
			inline static bool getCliShm
			(
			 	MemoryRef& ref,
			 	const char* pszPath
			)
			{
				key_t iKey = ftok(pszPath, 1);
				if (-1 == iKey)
				{
					return false;
				}
				int iShmId = shmget (iKey, 0, 0);
				if (-1 == iShmId)
				{
					return false;
				}
				unsigned char* pszAddr = (unsigned char*)(shmat(iShmId, nullptr, 0));
				if (((unsigned char*)-1) == pszAddr)
				{
					return false;
				}
				ref.m_pHeader = (MemoryHeader*)pszAddr;
				pszAddr += sizeof(MemoryHeader);
				ref.m_vecOmdcQueueHeader.resize(ref.m_pHeader->m_uOmdcChannelCnt);
				ref.m_vecOmddQueueHeader.resize(ref.m_pHeader->m_uOmddChannelCnt);
				/*
				* Before Change 20161104
				* ref.m_vecOmdcQueue.resize(ref.m_pHeader->m_uOmdcTradableCnt);
				* ref.m_vecOmddQueue.resize(ref.m_pHeader->m_uOmddTradableCnt);
				*/
				ref.m_vecOmdcQueue.resize(ref.m_pHeader->m_uOmdcChannelCnt);
				ref.m_vecOmddQueue.resize(ref.m_pHeader->m_uOmddChannelCnt);
				for (unsigned int i = 0; i < ref.m_pHeader->m_uOmdcChannelCnt; ++i)
				{
					ref.m_vecOmdcQueueHeader[i] = (QueueHeader*)pszAddr;
					pszAddr += sizeof(QueueHeader);
				}
				for (unsigned int i = 0; i < ref.m_pHeader->m_uOmddChannelCnt; ++i)
				{
					ref.m_vecOmddQueueHeader[i] = (QueueHeader*)pszAddr;
					pszAddr += sizeof(QueueHeader);
				}
				ref.m_pBrokerPositionQueueHeader = (QueueHeader*)pszAddr;
				pszAddr += sizeof(QueueHeader);
				for (unsigned int i = 0; i < ref.m_pHeader->m_uOmdcChannelCnt; ++i)
				{
					ref.m_vecOmdcQueue[i] = (TradableQueueItem*)pszAddr;
					pszAddr += (sizeof(TradableQueueItem) * ref.m_vecOmdcQueueHeader[i]->m_uQueueSize);
				}
				for (unsigned int i = 0; i < ref.m_pHeader->m_uOmddChannelCnt; ++i)
				{
					ref.m_vecOmddQueue[i] = (TradableQueueItem*)pszAddr;
					pszAddr += (sizeof(TradableQueueItem) * ref.m_vecOmddQueueHeader[i]->m_uQueueSize);
				}
				ref.m_pPositionQueue = (PositionQueueItem*)pszAddr;
				pszAddr += (sizeof(PositionQueueItem) * ref.m_pBrokerPositionQueueHeader->m_uQueueSize);
				ref.m_pOmdcTradable = (OmdcTradable*)pszAddr;
				pszAddr += (sizeof(OmdcTradable) * ref.m_pHeader->m_uOmdcTradableCnt);
				ref.m_pOmddTradable = (Tradable*)pszAddr;
				pszAddr += (sizeof(Tradable) * ref.m_pHeader->m_uOmddTradableCnt);
				return true;
			}
		}
	}
}
#endif


