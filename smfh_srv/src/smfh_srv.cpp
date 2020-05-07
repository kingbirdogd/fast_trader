#include <sys/time.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <dbp_tcp.h>
#include "smfh_srv_channel.h"
using namespace std;
using namespace dbp;
using namespace dbp::tools::srv;
using namespace dbp::omd;
using namespace dbp::omd::srv;
using namespace dbp::net::srv;
using namespace dbp::tcp;
using namespace dbp::shm;
class CTcpHandler
{
private:
	const static unsigned int m_uReadBufferSize = sizeof(SubscribeMessage) * 128;
	const static unsigned int m_uStoreBufferSize = sizeof(SubscribeMessage) * 256;
private:
	struct Buffer
	{
		unsigned char m_ReadBuffer[m_uReadBufferSize];
		unsigned char m_StoreBuffer[m_uStoreBufferSize];
		unsigned long long m_uRemainSize;
		Buffer():
			m_ReadBuffer{0},
			m_StoreBuffer{0},
			m_uRemainSize(0)
		{
		}
		~Buffer()
		{
		}
	};
private:
	typedef unordered_map <int, Buffer> CIoMap;
private:
	CIoMap m_Map;

public:
	CTcpHandler():
		m_Map()
	{
	}
	~CTcpHandler()
	{
	}
	CTcpHandler(const CTcpHandler&) = delete;
	CTcpHandler(CTcpHandler&&) = delete;
	CTcpHandler& operator= (const CTcpHandler&) = delete;
	CTcpHandler& operator= (CTcpHandler&&) = delete;
private:
	bool handlerMsg(SubscribeMessage* _pMsg, int _iClient)
	{
		SubscribeMessageReturn rtMsg;
		memcpy(static_cast<void*>(&rtMsg), static_cast<void*>(_pMsg), sizeof(SubscribeMessage));
		if (FlagOmdcOrderBook == (_pMsg->m_uFlag & maskFlagOmdcOrderBook))
		{
			flush_printf("tm:%llu, Do Omdc Orderbook : uCode = %u : mFlag = %llu, Sub = %d\n", dbp::tools::srv::current(), _pMsg->m_uCode, _pMsg->m_uFlag, int(_pMsg->m_bSubscribeFlag));
			COmdOmdcOrderMap::iterator it = omdcMap.find(_pMsg->m_uCode);
			if (omdcMap.end() == it)
			{
				rtMsg.m_SubscribeKey.invalidCode();
				flush_printf("tm:%llu, Invalid Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
			}
			else
			{

				OmdcTradable& tradable = shm_Ref.m_pOmdcTradable[it->second.m_SPOrderBook.m_uIdx];
				/*
				if (0xFFFFFFFF == tradable.m_Tradable.m_uQueueIdx)
				{
					rtMsg.m_SubscribeKey.notReady();
					flush_printf("tm:%llu, notReady  \n", dbp::tools::srv::current());
				}
				else
				{*/
					rtMsg.m_uQueueIdx = tradable.m_Tradable.m_uQueueIdx;
					rtMsg.m_uTradableIdx = it->second.m_SPOrderBook.m_uIdx;
					spin::spin_unique_lock lock(tradable.m_Tradable.m_Mutex);
					if (_pMsg->m_bSubscribeFlag)
					{
						SET_BIT(tradable.m_Tradable.m_uSubscribeFlag, _pMsg->m_uFlag & FlagOrderBookMask);
						flush_printf("tm:%llu, Subscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
					}
					else
					{
						UNSET_BIT(tradable.m_Tradable.m_uSubscribeFlag, _pMsg->m_uFlag & FlagOrderBookMask);
						flush_printf("tm:%llu, Unsubscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
					}
				//}
			}
		}
		else if (FlagOmddOrderBook == (_pMsg->m_uFlag & maskFlagOmddOrderBook))
		{
			unsigned long long mInstrumentGroup = 10000000000000;
			unsigned long long mCommodityCode = 1000000000;
			unsigned long long mExpdate = 100000;

			flush_printf("tm:%llu, Do Omdd Orderbook : Commodity Code = %d\n", dbp::tools::srv::current(), _pMsg->m_commoditycode);
			flush_printf("tm:%llu, Do Omdd Orderbook : Instrument Group = %d\n", dbp::tools::srv::current(), _pMsg->m_instrumentcode);
			flush_printf("tm:%llu, Do Omdd Orderbook : Expiry Date = %d\n", dbp::tools::srv::current(), _pMsg->m_expirydate);
			flush_printf("tm:%llu, Do Omdd Orderbook : Strike = %d\n", dbp::tools::srv::current(), _pMsg->m_strike);

			unsigned long long m_strike = _pMsg->m_strike;
			if(m_strike > 10000){

			}else if(m_strike > 1000){
				m_strike = m_strike * 10;
			}else if(m_strike > 100){
				m_strike = m_strike * 100;
			}


			unsigned long long mKey =
			_pMsg->m_instrumentcode*mInstrumentGroup+
			_pMsg->m_commoditycode*mCommodityCode+
			_pMsg->m_expirydate*mExpdate+
			m_strike;

			unsigned int m_ucode = 0;

			flush_printf("tm:%llu, Do Omdd Orderbook : uKey = %llu\n", dbp::tools::srv::current(), mKey);

			COmddOrderidMap::iterator itkeys = omdcOrderodMap.find(mKey);
			if(omdcOrderodMap.end() != itkeys){
				m_ucode = itkeys->second;
			}

			flush_printf("tm:%llu, Do Omdd Orderbook : uKey = %llu,  uCode = %u : mFlag = %llu, Sub = %d\n", dbp::tools::srv::current(),mKey, m_ucode, _pMsg->m_uFlag, int(_pMsg->m_bSubscribeFlag));


			COmdOrderMap::iterator it = omddMap.find(m_ucode);
			//COmdOrderMap::iterator it = omddMap.find(_pMsg->m_uCode);

			if (omddMap.end() == it)
			{
				rtMsg.m_SubscribeKey.invalidCode();
				//flush_printf("tm:%llu, Invalid Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
				flush_printf("tm:%llu, Invalid Code : %llu\n", dbp::tools::srv::current(), mKey );
			}
			else
			{

				Tradable& tradable = shm_Ref.m_pOmddTradable[it->second.m_uIdx];
				/*
				if (0xFFFFFFFF == tradable.m_uQueueIdx)
				{
					rtMsg.m_SubscribeKey.notReady();
					flush_printf("tm:%llu, notReady  \n", current());
				}
				else
				{*/
					rtMsg.m_uQueueIdx = tradable.m_uQueueIdx;
					rtMsg.m_uTradableIdx = it->second.m_uIdx;
					spin::spin_unique_lock lock(tradable.m_Mutex);
					if (_pMsg->m_bSubscribeFlag)
					{
						SET_BIT(tradable.m_uSubscribeFlag, _pMsg->m_uFlag & FlagOrderBookMask);
						//flush_printf("tm:%llu, Subscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
						flush_printf("tm:%llu, Subscribe Code : %u\n", dbp::tools::srv::current(),m_ucode );
					}
					else
					{
						UNSET_BIT(tradable.m_uSubscribeFlag, _pMsg->m_uFlag & FlagOrderBookMask);
						//flush_printf("tm:%llu, Unsubscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
						flush_printf("tm:%llu, Unsubscribe Code : %u\n", dbp::tools::srv::current(), m_ucode );
					}
				//}
			}
		}
		else if (FlagOmdcTrade == _pMsg->m_uFlag)
		{
			flush_printf("tm:%llu, Do Omdc Trade : \n", current());
			flush_printf("tm:%llu, Do Omdc Trade : uCode = %u : mFlag = %llu, Sub = %d\n", dbp::tools::srv::current(), _pMsg->m_uCode, _pMsg->m_uFlag, int(_pMsg->m_bSubscribeFlag));
			COmdOmdcOrderMap::iterator it = omdcMap.find(_pMsg->m_uCode);
			if (omdcMap.end() == it)
			{
				rtMsg.m_SubscribeKey.invalidCode();
				flush_printf("tm:%llu, Invalid Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
			}
			else
			{
				OmdcTradable& tradable = shm_Ref.m_pOmdcTradable[it->second.m_SPOrderBook.m_uIdx];
				/*
				if (0xFFFFFFFF == tradable.m_Tradable.m_uQueueIdx)
				{
					rtMsg.m_SubscribeKey.notReady();
					flush_printf("tm:%llu, notReady  \n", dbp::tools::srv::current());
				}
				else
				{*/
					rtMsg.m_uQueueIdx = tradable.m_Tradable.m_uQueueIdx;
					rtMsg.m_uTradableIdx = it->second.m_SPOrderBook.m_uIdx;
					spin::spin_unique_lock lock(tradable.m_Tradable.m_Mutex);
					if (_pMsg->m_bSubscribeFlag)
					{
						SET_BIT(tradable.m_Tradable.m_uSubscribeFlag, FlagTradeMask);
						flush_printf("tm:%llu, Subscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
					}
					else
					{
						UNSET_BIT(tradable.m_Tradable.m_uSubscribeFlag, FlagTradeMask);
						flush_printf("tm:%llu, Unsubscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
					}
				//}
			}
		}
		else if (FlagOmddTrade == _pMsg->m_uFlag)
		{

			flush_printf("tm:%llu, Do Omdd Trade : Commodity Code = %d\n", dbp::tools::srv::current(), _pMsg->m_commoditycode);
			flush_printf("tm:%llu, Do Omdd Trade : Instrument Group = %d\n", dbp::tools::srv::current(), _pMsg->m_instrumentcode);
			flush_printf("tm:%llu, Do Omdd Trade : Expiry Date = %d\n", dbp::tools::srv::current(), _pMsg->m_expirydate);
			flush_printf("tm:%llu, Do Omdd Trade : Strike = %d\n", dbp::tools::srv::current(), _pMsg->m_strike);

			//flush_printf("tm:%llu, Do Omdd Trade : \n", dbp::tools::srv::current());
			//flush_printf("tm:%llu, Do Omdd Trade : uCode = %u : mFlag = %llu, Sub = %d\n", dbp::tools::srv::current(), _pMsg->m_uCode, _pMsg->m_uFlag, int(_pMsg->m_bSubscribeFlag));
			unsigned long long mInstrumentGroup = 10000000000000;
			unsigned long long mCommodityCode = 1000000000;
			unsigned long long mExpdate = 100000;

			unsigned long long m_strike = _pMsg->m_strike;
			if(m_strike > 10000){

			}else if(m_strike > 1000){
				m_strike = m_strike * 10;
			}else if(m_strike > 100){
				m_strike = m_strike * 100;
			}

			unsigned long long mKey =
			_pMsg->m_instrumentcode*mInstrumentGroup+
			_pMsg->m_commoditycode*mCommodityCode+
			_pMsg->m_expirydate*mExpdate+
			m_strike;

			unsigned int m_ucode = 0;

			flush_printf("tm:%llu, Do Omdd Trade : uKey = %llu\n", dbp::tools::srv::current(), mKey);

			COmddOrderidMap::iterator itkeys = omdcOrderodMap.find(mKey);
			if(omdcOrderodMap.end() != itkeys){
				m_ucode = itkeys->second;
			}

			//flush_printf("tm:%llu, Do Omdd Trade : uCode = %u : mFlag = %llu, Sub = %d\n", dbp::tools::srv::current(), _pMsg->m_uCode, _pMsg->m_uFlag, int(_pMsg->m_bSubscribeFlag));
			flush_printf("tm:%llu, Do Omdd Trade : uKey = %llu,  uCode = %u : mFlag = %llu, Sub = %d\n", dbp::tools::srv::current(),mKey, m_ucode, _pMsg->m_uFlag, int(_pMsg->m_bSubscribeFlag));

						//COmdOrderMap::iterator it = omddMap.find(_pMsg->m_uCode);


			//COmdOrderMap::iterator it = omddMap.find(_pMsg->m_uCode);
			COmdOrderMap::iterator it = omddMap.find(m_ucode);
			if (omddMap.end() == it)
			{
				rtMsg.m_SubscribeKey.invalidCode();
				//flush_printf("tm:%llu, Invalid Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
				flush_printf("tm:%llu, Invalid Code : %llu %u\n", dbp::tools::srv::current(),mKey, m_ucode );
			}
			else
			{

				Tradable& tradable = shm_Ref.m_pOmddTradable[it->second.m_uIdx];
				/*
				if (0xFFFFFFFF == tradable.m_uQueueIdx)
				{
					rtMsg.m_SubscribeKey.notReady();
					flush_printf("tm:%llu, notReady  \n", dbp::tools::srv::current());
				}
				else
				{*/
					rtMsg.m_uQueueIdx = tradable.m_uQueueIdx;
					rtMsg.m_uTradableIdx = it->second.m_uIdx;
					spin::spin_unique_lock lock(tradable.m_Mutex);
					if (_pMsg->m_bSubscribeFlag)
					{
						SET_BIT(tradable.m_uSubscribeFlag, FlagTradeMask);
						//flush_printf("tm:%llu, Subscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
						flush_printf("tm:%llu, Subscribe Code : %u\n", dbp::tools::srv::current(), m_ucode );
					}
					else
					{
						UNSET_BIT(tradable.m_uSubscribeFlag, FlagTradeMask);
						//flush_printf("tm:%llu, Unsubscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
						flush_printf("tm:%llu, Unsubscribe Code : %u\n", dbp::tools::srv::current(), m_ucode);
					}
				//}
			}
		}
		else if (FlagBrokerQueue == _pMsg->m_uFlag)
		{
			flush_printf("tm:%llu, Do BrokerQueue : uCode = %u : mFlag = %llu, Sub = %d\n", dbp::tools::srv::current(), _pMsg->m_uCode, _pMsg->m_uFlag, int(_pMsg->m_bSubscribeFlag));
			COmdOmdcOrderMap::iterator it = omdcMap.find(_pMsg->m_uCode);
			if (omdcMap.end() == it)
			{
				rtMsg.m_SubscribeKey.invalidCode();
				flush_printf("tm:%llu, Invalid Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
			}
			else
			{
				OmdcTradable& tradable = shm_Ref.m_pOmdcTradable[it->second.m_SPOrderBook.m_uIdx];
				/*
				if (0 == tradable.m_Position.m_uFirstCome)
				{
					rtMsg.m_SubscribeKey.notReady();
					flush_printf("tm:%llu, notReady  \n", dbp::tools::srv::current());
				}
				else
				{*/
					rtMsg.m_uQueueIdx = 0;
					rtMsg.m_uTradableIdx = it->second.m_SPOrderBook.m_uIdx;
					spin::spin_unique_lock lock(tradable.m_Position.m_Mutex);
					if (_pMsg->m_bSubscribeFlag)
					{
						tradable.m_Position.m_uSbuscribeFlag = 1;
						flush_printf("tm:%llu, Subscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
					}
					else
					{
						tradable.m_Position.m_uSbuscribeFlag = 0;
						flush_printf("tm:%llu, Unsubscribe Code : %u\n", dbp::tools::srv::current(), _pMsg->m_uCode );
					}
				//}
			}
		}
		else
		{
			rtMsg.m_SubscribeKey.invalidMsg();
			flush_printf("tm:%llu, Invalid Msg  \n", dbp::tools::srv::current());
		}
		ssize_t iSend = ::send(_iClient, &rtMsg, sizeof(SubscribeMessageReturn), 0);
		if (iSend <= 0)
		{
			return false;
		}
		if (((size_t)iSend) != sizeof(SubscribeMessageReturn))
		{
			return false;
		}
		return true;
	}
public:
	void onAccept(int _iClient)
	{
		m_Map[_iClient];
	}
	void onIoStop(int _iClient)
	{
		m_Map.erase(_iClient);
		::close(_iClient);
	}
	bool onIoIn(int _iClient)
	{
		CIoMap::iterator it = m_Map.find(_iClient);
		if (m_Map.end() == it)
		{
			return false;
		}
		Buffer& buffer = it->second;
		while (true)
		{
			ssize_t iRcv = pfunc_recv(_iClient, buffer.m_ReadBuffer, m_uReadBufferSize, 0);
			if (0 >= iRcv)
			{
				break;
			}
			if (0 == buffer.m_uRemainSize)
			{
				unsigned long long uTotalSize =  iRcv;
				unsigned long long uCnt = uTotalSize / sizeof(SubscribeMessage);
				unsigned long long uRemain = uTotalSize % sizeof(SubscribeMessage);
				SubscribeMessage* pMsg = (SubscribeMessage*)(&buffer.m_ReadBuffer[0]);
				for (unsigned long long i = 0; i < uCnt; ++i)
				{
					if (!handlerMsg(pMsg, _iClient))
					{
						return false;
					}
					++pMsg;
				}
				if (0 != uRemain)
				{
					memcpy (&buffer.m_StoreBuffer[0], &buffer.m_ReadBuffer[uTotalSize - uRemain], uRemain);
					buffer.m_uRemainSize = uRemain;
				}
				else
				{
					buffer.m_uRemainSize = 0;
				}
			}
			else
			{
				memcpy(&buffer.m_StoreBuffer[buffer.m_uRemainSize], &buffer.m_ReadBuffer[0], iRcv);
				unsigned long long uTotalSize = buffer.m_uRemainSize + iRcv;
				unsigned long long uCnt = uTotalSize / sizeof(SubscribeMessage);
				unsigned long long uRemain = uTotalSize % sizeof(SubscribeMessage);
				SubscribeMessage* pMsg = (SubscribeMessage*)(&buffer.m_StoreBuffer[0]);
				for (unsigned long long i = 0; i < uCnt; ++i)
				{
					if (!handlerMsg(pMsg, _iClient))
					{
						return false;
					}
					++pMsg;
				}
				if (0 != uRemain)
				{
					memmove(&buffer.m_StoreBuffer[0], &buffer.m_ReadBuffer[uTotalSize - uRemain], uRemain);
					buffer.m_uRemainSize = uRemain;
				}
				else
				{
					buffer.m_uRemainSize = 0;
				}
			}
		}
		return true;
	}
};
static char stdOutBuffer[65536] = {0};
static int iTcpEpoll = 0;
static int iTcpAcceptor = 0;
static CTcpHandler tcpHandler;
inline void startTcpServer()
{
	flush_printf("tm:%llu, Start startTcpServer \n", dbp::tools::srv::current());
	blockSigPipe();
	epoll_event events[8192];
	memset(&events, 0, sizeof(epoll_event) * 8192);
	while (true)
	{
		int iCnt = epoll_wait(iTcpEpoll, events, 8192, -1);
		if (0 < iCnt)
		{
			for (int i = 0; i < iCnt; ++i)
			{
				if (events[i].events & EPOLLIN)
				{
					if (events[i].data.fd == iTcpAcceptor)
					{
						while (true)
						{
							int iClient = accept(iTcpAcceptor, nullptr, 0);
							if (iClient > 0)
							{
								struct epoll_event objEvent;
								memset(&objEvent, 0, sizeof(struct epoll_event));
								objEvent.data.fd = iClient;
								objEvent.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
								epoll_ctl(iTcpEpoll, EPOLL_CTL_ADD, iClient, &objEvent);
								tcpHandler.onAccept(iClient);
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						if (!tcpHandler.onIoIn(events[i].data.fd))
						{
							struct epoll_event objEvent;
							memset(&objEvent, 0, sizeof(struct epoll_event));
							objEvent.data.fd = events[i].data.fd;
							epoll_ctl(iTcpEpoll, EPOLL_CTL_DEL, events[i].data.fd, &objEvent);
							::close(events[i].data.fd);
						}
					}
				}
				else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
				{
					struct epoll_event objEvent;
					memset(&objEvent, 0, sizeof(struct epoll_event));
					objEvent.data.fd = events[i].data.fd;
					epoll_ctl(iTcpEpoll, EPOLL_CTL_DEL, events[i].data.fd, &objEvent);
					if (events[i].data.fd == iTcpAcceptor)
					{
						close(iTcpAcceptor);
						iTcpAcceptor = getNoBlockReuseTcpAccpetor(uTcpPort, iTcpEpoll, strTcpIp);
					}
					else
					{
						tcpHandler.onIoStop(events[i].data.fd);
					}
				}
			}
		}
	}
}
inline static bool start()
{
	auto itActivate = mActivateChannel.find("OmdcChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, Start OmdcChannel \n", dbp::tools::srv::current());
		if (!startOmdcChannel())
		{
			return false;
		}
	}
	itActivate = mActivateChannel.find("OmddChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, Start OmddChannel \n", dbp::tools::srv::current());
		if (!startOmddChannel())
		{
			return false;
		}
	}

	itActivate = mActivateChannel.find("BrokerQueueChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, Start BrokerQueueChannel \n", dbp::tools::srv::current());
		if (!startBrokerQueueChannel())
		{
			return false;
		}
	}
	startTcpServer();
	return true;
}
inline static void closeAll()
{
	if (0 < iTcpEpoll)
	{
		close(iTcpEpoll);
	}
	if (0 < iTcpAcceptor)
	{
		close(iTcpAcceptor);
	}
	for (size_t i = 0; i < omdcStreams.size(); ++i)
	{
		if (0 != omdcStreams[i].m_iHot)
		{
			close(omdcStreams[i].m_iHot);
			omdcStreams[i].m_iHot = 0;
		}
		if (0 != omdcStreams[i].m_iRefresh)
		{
			close(omdcStreams[i].m_iRefresh);
			omdcStreams[i].m_iRefresh = 0;
		}
		if (0 != omdcStreams[i].m_iEpoll)
		{
			close(omdcStreams[i].m_iEpoll);
			omdcStreams[i].m_iEpoll = 0;
		}
	}
	for (size_t i = 0; i < omddStreams.size(); ++i)
	{
		if (0 != omddStreams[i].m_iHot)
		{
			close(omddStreams[i].m_iHot);
			omddStreams[i].m_iHot = 0;
		}
		if (0 != omddStreams[i].m_iRefresh)
		{
			close(omdcStreams[i].m_iRefresh);
			omdcStreams[i].m_iRefresh = 0;
		}
		if (0 != omdcStreams[i].m_iEpoll)
		{
			close(omddStreams[i].m_iEpoll);
			omddStreams[i].m_iEpoll = 0;
		}
	}
	if (0 != brokerStream.m_Channel.m_iHot)
	{
		close(brokerStream.m_Channel.m_iHot);
		brokerStream.m_Channel.m_iHot = 0;
	}
	if (0 != brokerStream.m_Channel.m_iRefresh)
	{
		close(brokerStream.m_Channel.m_iRefresh);
		brokerStream.m_Channel.m_iRefresh = 0;
	}
	if (0 != brokerStream.m_Channel.m_iEpoll)
	{
		close(brokerStream.m_Channel.m_iEpoll);
		brokerStream.m_Channel.m_iEpoll = 0;
	}
}
inline static bool initGlobal()
{
	iTcpEpoll = epoll_create(16);
	if (iTcpEpoll <= 0)
	{
		cerr << "Create Tcp Epoll Error:" << strerror(errno) << endl;
		closeAll();
		return false;
	}
	iTcpAcceptor = getNoBlockReuseTcpAccpetor(uTcpPort, iTcpEpoll, strTcpIp);
	if (iTcpAcceptor <= 0)
	{
		cerr << "Start up Tcp Acceptor:" <<  strerror(errno) << endl;
		closeAll();
		return false;
	}
	return true;
}
inline static bool init(const char* _pszJsonPath)
{
/*
	time_t iTime = time(0);
	struct tm tmRlt;
	localtime_r(&iTime, &tmRlt);
	unsigned int uTime = tmRlt.tm_hour * 3600 + tmRlt.tm_min * 60 + tmRlt.tm_sec;
	if (uTime > 65700 || uTime < 8100)
	{
		return false;
	}
*/
	if (!initJson(_pszJsonPath))
	{
		return false;
	}
	flush_printf("tm:%llu, initGlobal \n", dbp::tools::srv::current());
	return initGlobal();
}
int main(int _iArgc, char** _pszArgv)
{
	blockSigPipe();
	setbuffer(stdout, stdOutBuffer, 65536);
	pid_t iPid = fork();
	if (0 == iPid)
	{
		iPid = fork();
		if (0 > iPid)
		{
			cerr << "fork error" << endl;
			return -1;
		}
		else if (0 < iPid)
		{
			return 0;
		}
		else
		{
			blockSigPipe();
			setbuffer(stdout, stdOutBuffer, 65536);
		}
	}
	else if (0 > iPid)
	{
		cerr << "fork error" << endl;
		return -1;
	}
	else
	{
		int iStatus = 0;
		waitpid(iPid, &iStatus, 0);
		return 0;
	}
	if (2 != _iArgc)
	{
		cerr << "Usage: smfh_srv <cfg json>" << endl;
		return -1;
	}
	if (!init(_pszArgv[1]))
	{
		cerr << "init error:" << strerror(errno) << endl;
		closeAll();
		return -1;
	}
	if (!start())
	{
		cerr << "start error:" << strerror(errno) << endl;
		closeAll();
		return -1;
	}
	return 0;
}



