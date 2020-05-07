#ifndef __DBP_SHM_SRV__
#define __DBP_SHM_SRV__
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
		namespace srv
		{
			const static unsigned long long g_uShmMinSize = 0x4000000;
			inline static unsigned long long getMemSize
			(
				unsigned int _uOmdcTradableCnt,
				unsigned int _uOmddTradableCnt,
				unsigned int _uOmdcChannelCnt,
				unsigned int _uOmddChannelCnt,
				unsigned long long _uPositionQueueSize,
				const std::vector<unsigned long long>& _omdcQueueSize,
				const std::vector<unsigned long long>& _omddQueueSize
			)
			{
				unsigned long long uRt = sizeof(MemoryHeader);
				uRt += (sizeof(QueueHeader) * (_uOmdcChannelCnt + _uOmddChannelCnt + 1));
				uRt += (sizeof(OmdcTradable) * _uOmdcTradableCnt);
				uRt += (sizeof(Tradable) * _uOmddTradableCnt);
				for (size_t i = 0; i < _omdcQueueSize.size(); ++i)
				{
					uRt += (sizeof(TradableQueueItem) * _omdcQueueSize[i]);
				}
				for (size_t i = 0; i < _omddQueueSize.size(); ++i)
				{
					uRt += (sizeof(TradableQueueItem) * _omddQueueSize[i]);
				}
				uRt += (sizeof(PositionQueueItem) * _uPositionQueueSize);
				unsigned long long uMod = uRt % g_uShmMinSize;
				if (0 != uMod)
				{
					uRt -= uMod;
					uRt += g_uShmMinSize;
				}
				return uRt;
			}
			inline static bool cleanShm(const char* pszPath)
			{
				key_t iKey = ftok(pszPath, 1);
				if (-1 != iKey)
				{
					int iShmId = shmget (iKey, 0, 0);
					if (-1 != iShmId)
					{
						if (-1 == shmctl(iShmId, IPC_RMID, nullptr))
						{
							return false;
						}
					}
					else
					{
						if (errno != ENOENT)
						{
							return false;
						}
					}
					unlink(pszPath);
				}
				return true;
			}
			inline static bool getSrvShm
			(
			 	MemoryRef& ref,
			 	const char* pszPath,
				unsigned int _uOmdcTradableCnt,
				unsigned int _uOmddTradableCnt,
				unsigned int _uOmdcChannelCnt,
				unsigned int _uOmddChannelCnt,
				unsigned long long _uPositionQueueSize,
				const std::vector<unsigned long long>& _omdcQueueSize,
				const std::vector<unsigned long long>& _omddQueueSize
			)
			{
				if (_omdcQueueSize.size() != _uOmdcChannelCnt)
				{
					return false;
				}
				if (_omddQueueSize.size() != _uOmddChannelCnt)
				{
					return false;
				}
				if (!cleanShm(pszPath))
				{
					return false;
				}
				int iKeyFile = open(pszPath, O_CREAT | O_EXCL, 00666);
				if (-1 == iKeyFile)
				{
					return false;
				}
				close (iKeyFile);
				key_t iKey = ftok(pszPath, 1);
				if (-1 == iKey)
				{
					return false;
				}
				unsigned long long uSize = 
				getMemSize
				(
				 	_uOmdcTradableCnt, 
					_uOmddTradableCnt, 
					_uOmdcChannelCnt, 
					_uOmddChannelCnt, 
					_uPositionQueueSize, 
					_omdcQueueSize, 
					_omddQueueSize
				);
				int iShmId = shmget (iKey, uSize, IPC_CREAT | IPC_EXCL | 00666);
				if (-1 == iShmId)
				{
					return false;
				}
				unsigned char* pszAddr = (unsigned char*)(shmat(iShmId, nullptr, 0));
				if (((unsigned char*)-1) == pszAddr)
				{
					return false;
				}
				ref.m_pHeader = new ((void*)pszAddr) MemoryHeader();
				pszAddr += sizeof(MemoryHeader);
				ref.m_pHeader->m_uOmdcTradableCnt = _uOmdcTradableCnt;
				ref.m_pHeader->m_uOmddTradableCnt = _uOmddTradableCnt;
				ref.m_pHeader->m_uOmdcChannelCnt = _uOmdcChannelCnt;
				ref.m_pHeader->m_uOmddChannelCnt = _uOmddChannelCnt;
				ref.m_vecOmdcQueueHeader.resize(_uOmdcChannelCnt);
				ref.m_vecOmddQueueHeader.resize(_uOmddChannelCnt);
				ref.m_vecOmdcQueue.resize(_uOmdcChannelCnt);
				ref.m_vecOmddQueue.resize(_uOmddChannelCnt);
				for (unsigned int i = 0; i < _uOmdcChannelCnt; ++i)
				{
					ref.m_vecOmdcQueueHeader[i] = new ((void*)pszAddr) QueueHeader();
					pszAddr += sizeof(QueueHeader);
					ref.m_vecOmdcQueueHeader[i]->m_uQueueSize = _omdcQueueSize[i];
				}
				for (unsigned int i = 0; i < _uOmddChannelCnt; ++i)
				{
					ref.m_vecOmddQueueHeader[i] = new ((void*)pszAddr) QueueHeader();
					pszAddr += sizeof(QueueHeader);
					ref.m_vecOmddQueueHeader[i]->m_uQueueSize = _omddQueueSize[i];
				}
				ref.m_pBrokerPositionQueueHeader = new ((void*)pszAddr) QueueHeader();
				pszAddr += sizeof(QueueHeader);
				ref.m_pBrokerPositionQueueHeader->m_uQueueSize = _uPositionQueueSize;
				for (unsigned int i = 0; i < _uOmdcChannelCnt; ++i)
				{
					ref.m_vecOmdcQueue[i] = new ((void*)pszAddr) TradableQueueItem[_omdcQueueSize[i]];
					pszAddr += (sizeof(TradableQueueItem) * _omdcQueueSize[i]);
				}
				for (unsigned int i = 0; i < _uOmddChannelCnt; ++i)
				{
					ref.m_vecOmddQueue[i] = new ((void*)pszAddr) TradableQueueItem[_omddQueueSize[i]];
					pszAddr += (sizeof(TradableQueueItem) * _omddQueueSize[i]);
				}
				ref.m_pPositionQueue = new ((void*)pszAddr) PositionQueueItem[_uPositionQueueSize];
				pszAddr += (sizeof(PositionQueueItem) * _uPositionQueueSize);
				ref.m_pOmdcTradable = new ((void*)pszAddr) OmdcTradable[_uOmdcTradableCnt];
				pszAddr += (sizeof(OmdcTradable) * _uOmdcTradableCnt);
				ref.m_pOmddTradable = new ((void*)pszAddr) Tradable[_uOmddTradableCnt];
				pszAddr += (sizeof(Tradable) * _uOmddTradableCnt);
				return true;
			}
		}
	}
}
#endif


