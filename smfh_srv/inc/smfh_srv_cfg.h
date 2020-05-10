#ifndef __SMFH_SRV_CFG__
#define __SMFH_SRV_CFG__
#include <sys/types.h>
#include <sys/socket.h>
#include <epoll.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <dbp_cpu.h>
#include <srv/omd.h>
#include <srv/json_node.h>
#include <srv/net.h>
#include <srv/dbp_shm.h>
typedef std::unordered_set<int> CISBrokerIDSet;
class COmdMemOrderbook
{
public:
	dbp::shm::OrderItem m_BidOrder[11];
	dbp::shm::OrderItem m_AskOrder[11];
public:
	COmdMemOrderbook(const COmdMemOrderbook& book) = delete;
	COmdMemOrderbook(COmdMemOrderbook&& book) = delete;
	COmdMemOrderbook():
		m_BidOrder{dbp::shm::OrderItem()},
		m_AskOrder{dbp::shm::OrderItem()}
	{
	}
	~COmdMemOrderbook()
	{
	}
};
class COmdOrderbook: public COmdMemOrderbook
{
public:
	unsigned int m_uIdx;
	unsigned int m_uLotSize;
public:
	COmdOrderbook(const COmdOrderbook& book) = delete;
	COmdOrderbook(COmdOrderbook&& book) = delete;
	COmdOrderbook():
		COmdMemOrderbook(),
		m_uIdx(0),
		m_uLotSize(0)
	{
	}
	~COmdOrderbook()
	{
	}
};
class COmdOmdcOrderbook
{
public:
	COmdMemOrderbook m_SSOrderBook;
	COmdOrderbook m_SPOrderBook;
public:
	COmdOmdcOrderbook(const COmdOmdcOrderbook& book) = delete;
	COmdOmdcOrderbook(COmdOmdcOrderbook&& book) = delete;
	COmdOmdcOrderbook():
		m_SSOrderBook(),
		m_SPOrderBook()
	{
	}
	~COmdOmdcOrderbook()
	{
	}
};
class CStreamChannel
{
public:
	unsigned long long m_uQueueSize;
	int m_iHot;
	int m_iRefresh;
	int m_iEpoll;
	unsigned long long m_uChannelIdx;
	size_t m_uRetranProxyIdx;
	unsigned short int m_uChannelId;
public:
	CStreamChannel():
		m_uQueueSize(0),
		m_iHot(0),
		m_iRefresh(0),
		m_iEpoll(0),
		m_uChannelIdx(0),
		m_uRetranProxyIdx(0),
		m_uChannelId(0)
	{
	}
	~CStreamChannel()
	{
	}
};
class CBroker
{
public:
	CStreamChannel m_Channel;
	CISBrokerIDSet m_CISBrokerIDSet;
	unsigned int m_uLastWaitOrderBookCode;
	bool m_bCodeChange;
	bool m_bDataUpdate;
	unsigned int m_iLastSPOrderBookIdx;
public:
	CBroker():
		m_Channel(),
		m_CISBrokerIDSet(),
		m_uLastWaitOrderBookCode(0),
		m_bCodeChange(false),
		m_bDataUpdate(false),
		m_iLastSPOrderBookIdx(0)
	{
	}
	CBroker(const CBroker& book) = delete;
	CBroker(CBroker&& book) = delete;
	~CBroker()
	{
	}
};
class CDefChannel
{
public:
	enum EStatus
	{
		NO_READY = 0,
		READY = 1,
		FINISHED = 2
	};
public:
	EStatus m_Status;
	unsigned int m_uSeq;
	bool m_bAgain;
	CDefChannel():
		m_Status(CDefChannel::NO_READY),
		m_uSeq(0),
		m_bAgain(false)
	{
	}
	~CDefChannel()
	{
	}
};
class CRetranProxy
{
public:
	std::string m_strIp;
	unsigned short int m_uPort;
public:
	CRetranProxy():
		m_strIp(""),
		m_uPort(0)
	{
	}
	~CRetranProxy()
	{
	}
};

//CUP core
static dbp::cpu::CpuInfo cpuInfo;
//CUP core end

typedef std::unordered_map<std::string, std::string> CActivateChannel;
typedef void (*PFuncOmdMsgHandler)(dbp::omd::COmdMsgHeader* _pMsg, unsigned int _uSeq, unsigned long long _uChannelIdx, unsigned long long _uPkgTm);
typedef std::vector<CRetranProxy> CRetranVec;
typedef std::unordered_set<unsigned int> CSpecialSpreadTableSet;
typedef std::unordered_map<unsigned int, COmdOrderbook> COmdOrderMap;
typedef std::unordered_map<unsigned int, COmdOmdcOrderbook> COmdOmdcOrderMap;
typedef std::unordered_map<unsigned long long, unsigned int> COmddOrderidMap;
typedef std::unordered_map<int, CDefChannel> CDefMap;
typedef std::vector<CStreamChannel> CStreamVec;
static COmdOmdcOrderMap omdcMap;
static COmdOrderMap omddMap;
static CSpecialSpreadTableSet specialSpreadTableSet;
static CRetranVec retranVec;
static CStreamVec omdcStreams;
static CStreamVec omddStreams;
static CBroker brokerStream;
static COmddOrderidMap omdcOrderodMap;
static dbp::shm::MemoryRef shm_Ref;
static CActivateChannel mActivateChannel;

inline std::string getString(char* _pszBuffer, unsigned int _uOfferSet, unsigned int _uSize)
{
	return std::string(_pszBuffer+_uOfferSet,_uSize);
}

//Running Data Loading
inline static bool loadCpu(dbp::cfg::srv::json_node& _json)
{
	dbp::cfg::srv::json_node* pCPU = _json.getMapNode("CPU");
	if (nullptr == pCPU)
	{
		std::cerr << "Can not found CPU node" << std::endl;
		return false;
	}
	if (dbp::cfg::srv::json_node::VECTOR != pCPU->getType())
	{
		std::cerr << "CPU node is not a json" << std::endl;
		return false;
	}
	size_t uCpuSize = pCPU->getArraySize();
	for (size_t i = 0; i < uCpuSize; ++i)
	{
		dbp::cfg::srv::json_node* pValue = pCPU->getArrayNode(i);
		if (dbp::cfg::srv::json_node::INT != pValue->getType())
		{
			std::cerr << "CPU[" << i << "] is not a INT" << std::endl;
			return false;
		}
		long long llValue = pValue->getInt();
		if (llValue < 0 || llValue >= std::numeric_limits<int>::max())
		{
			std::cerr << "CPU[" << i << "] is not in Range" << std::endl;
			return false;
		}
		int iVavlue = (int)llValue;
		if (!cpuInfo.setCpu(iVavlue))
		{
			std::cerr << "CPU[" << i << "] init fail" << std::endl;
			return false;
		}
	}
	return true;
}
inline static bool loadDefinition(dbp::cfg::srv::json_node& _json, const std::string& strPath)
{
	std::ofstream ofs_symbolmap(strPath + ".symbolmap.csv", std::ios_base::out | std::ios_base::trunc);
	std::ofstream ofs_warrantmap(strPath + ".warrantmap.csv", std::ios_base::out | std::ios_base::trunc);
	dbp::cfg::srv::json_node* pDefinition = _json.getMapNode("Definition");
	if (nullptr == pDefinition)
	{
		std::cerr << "Can not found Definition node" << std::endl;
		return false;
	}
	if (dbp::cfg::srv::json_node::JSON != pDefinition->getType())
	{
		std::cerr << "Definition node is not a json" << std::endl;
		return false;
	}
	std::string strCatchPath = "";
	dbp::cfg::srv::json_node* pCatchPath = pDefinition->getMapNode("CatchPath");
	if (nullptr != pCatchPath)
	{
		if (dbp::cfg::srv::json_node::STRING != pCatchPath->getType())
		{
			std::cerr << "CatchPath node is not a STRING" << std::endl;
			return false;
		}
		pCatchPath->getString(strCatchPath);
	}
	else
	{
		const char* pszHome = getenv("HOME");
		if (nullptr == pszHome)
		{
			std::cerr << "CatchPath node not found and HOME not found" << std::endl;
			return false;
		}
		else
		{
			strCatchPath = pszHome;
		}
	}
	char szCacheFileName[512] = {0};
	struct tm tmResult;
	memset(szCacheFileName, 0, 64);
	memset(&tmResult, 0, sizeof(struct tm));
	time_t iNowTime = time(0);
	localtime_r(&iNowTime, &tmResult);
	snprintf(szCacheFileName, 512, "%s/smfh_def_catch_%04d%02d%02d", strCatchPath.c_str(), tmResult.tm_year + 1900, tmResult.tm_mon + 1, tmResult.tm_mday);
	std::ifstream ifs(szCacheFileName, std::ios::binary);
	bool bReload = true;
	if (ifs)
	{
		bool bFail = false;
		unsigned int uCodeCnt = 0;
		unsigned int uCode = 0;
		unsigned long long uKeys = 0;
		unsigned long uMCode = 0;
		ifs.read((char*)((void*)(&uCodeCnt)), sizeof(unsigned int));
		if (ifs)
		{
			if(!bFail){
				for (unsigned int i = 0; i < uCodeCnt; ++i)
				{
					ifs.read((char*)((void*)(&uCode)), sizeof(unsigned int));
					if (!ifs)
					{
						bFail = true;
						break;
					}
					ifs.read((char*)((void*)(&omdcMap[uCode].m_SPOrderBook.m_uLotSize)), sizeof(unsigned int));
					if (!ifs)
					{
						bFail = true;
						break;
					}
					omdcMap[uCode].m_SPOrderBook.m_uIdx = i;
				}
			}

			if (!bFail)
			{
				ifs.read((char*)((void*)(&uCodeCnt)), sizeof(unsigned int));
				if (ifs)
				{
					for (unsigned int i = 0; i < uCodeCnt; ++i)
					{
						ifs.read((char*)((void*)(&uCode)), sizeof(unsigned int));
						if (!ifs)
						{
							bFail = true;
							break;
						}
						omddMap[uCode].m_uIdx = i;
						omddMap[uCode].m_uLotSize = 1;
					}

				}
				else
				{
					bFail = true;
				}
			}


			if (!bFail)
			{
				ifs.read((char*)((void*)(&uCodeCnt)), sizeof(unsigned int));
				if (ifs)
				{
					for (unsigned int i = 0; i < uCodeCnt; ++i)
					{
						ifs.read((char*)((void*)(&uKeys)), sizeof(unsigned long long));
						ifs.read((char*)((void*)(&uMCode)), sizeof(unsigned long));
						if (!ifs)
						{
							bFail = true;
							break;
						}
						omdcOrderodMap[uKeys] = uMCode;
					}
				}
				else
				{
					bFail = true;
				}
			}


			if (!bFail)
			{
				ifs.read((char*)((void*)(&uCodeCnt)), sizeof(unsigned int));
				if (ifs)
				{
					for (unsigned int i = 0; i < uCodeCnt; ++i)
					{
						ifs.read((char*)((void*)(&uCode)), sizeof(unsigned int));
						if (!ifs)
						{
							bFail = true;
							break;
						}
						specialSpreadTableSet.insert(uCode);
					}
				}
				else
				{
					bFail = true;
				}
			}
		}
		else
		{
			bFail = true;
		}
		if (bFail)
		{
			omdcMap.clear();
			omddMap.clear();
			specialSpreadTableSet.clear();
		}
		else
		{
			bReload = false;
		}
	}else{
          flush_printf("tm:%llu, No Cache File \n", dbp::tools::srv::current());
	}


	if(bReload)
	{
		int iDefEopll = epoll_create(4);
		if (-1 == iDefEopll)
		{
			std::cerr << "create epoll fail" << std::endl;
			return false;
		}

		CDefMap omdcStatus;
		auto itActivate = mActivateChannel.find("OmdcChannel");
		if(itActivate != mActivateChannel.end())
		{

			dbp::cfg::srv::json_node* pOmdc = pDefinition->getMapNode("OMDC");
			if (nullptr == pOmdc)
			{
				std::cerr << "OMDC node not found" << std::endl;
				::close(iDefEopll);
				return false;
			}
			if (dbp::cfg::srv::json_node::VECTOR != pOmdc->getType())
			{
				std::cerr << "OMDC node is not a std::vector" << std::endl;
				::close(iDefEopll);
				return false;
			}
			size_t uOmdcNodeSize = pOmdc->getArraySize();
			if (0 == uOmdcNodeSize)
			{
				std::cerr << "OMDC node size is 0" << std::endl;
				::close(iDefEopll);
				return false;
			}
			for (size_t i = 0; i < uOmdcNodeSize; ++i)
			{
				std::string strInterfaceIp = "";
				std::string strMulticastIp = "";
				unsigned short int uMulticastPort = 0;
				dbp::cfg::srv::json_node* pUdpNode = pOmdc->getArrayNode(i);
				if (dbp::cfg::srv::json_node::JSON != pUdpNode->getType())
				{
					std::cerr << "OMDC node[" << i << "] is not a json" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				dbp::cfg::srv::json_node* pInterfaceIp = pUdpNode->getMapNode("InterfaceIp");
				if (nullptr == pInterfaceIp)
				{
					std::cerr << "OMDC node[" << i << "] InterfaceIp not found" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				if (dbp::cfg::srv::json_node::STRING != pInterfaceIp->getType())
				{
					std::cerr << "OMDC node[" << i << "] InterfaceIp not STRING" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				pInterfaceIp->getString(strInterfaceIp);
				dbp::cfg::srv::json_node* pMulticastIp = pUdpNode->getMapNode("MulticastIp");
				if (nullptr == pMulticastIp)
				{
					std::cerr << "OMDC node[" << i << "] MulticastIp not found" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				if (dbp::cfg::srv::json_node::STRING != pMulticastIp->getType())
				{
					std::cerr << "OMDC node[" << i << "] MulticastIp not STRING" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				pMulticastIp->getString(strMulticastIp);
				dbp::cfg::srv::json_node* pMulticastPort = pUdpNode->getMapNode("MulticastPort");
				if (nullptr == pMulticastPort)
				{
					std::cerr << "OMDC node[" << i << "] MulticastPort not found" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pMulticastPort->getType())
				{
					std::cerr << "OMDC node[" << i << "] MulticastPort not INT" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				long long llMulticastPort = pMulticastPort->getInt();
				if (llMulticastPort <= 0 || llMulticastPort > 0xFFFF)
				{
					std::cerr << "OMDC node[" << i << "] MulticastPort not in Range" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				uMulticastPort = (unsigned short int)llMulticastPort;
				int iUdpHandler = dbp::net::srv::getNoBlockReuseUdpListener(uMulticastPort, strMulticastIp, strInterfaceIp);
				if (iUdpHandler <= 0)
				{
					std::cerr << "OMDC node[" << i << "] Create Socket Error, MulticastIp:" << strMulticastIp <<
						", MulticastPort:" << uMulticastPort <<
						", InterfaceIp:" << strInterfaceIp << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				struct epoll_event objEvent;
				memset(&objEvent, 0, sizeof(struct epoll_event));
				objEvent.events = EPOLLIN | EPOLLET;
				objEvent.data.fd = iUdpHandler;
				if (0 != epoll_ctl(iDefEopll, EPOLL_CTL_ADD, objEvent.data.fd, &objEvent))
				{
					std::cerr << "OMDC node[" << i << "] Add to Epoll Error, MulticastIp:" << strMulticastIp <<
						", MulticastPort:" << uMulticastPort <<
						", InterfaceIp:" << strInterfaceIp << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				omdcStatus[iUdpHandler].m_Status = CDefChannel::NO_READY;
				omdcStatus[iUdpHandler].m_uSeq = 0;
			}
		}


		CDefMap omddStatus;
		itActivate = mActivateChannel.find("OmddChannel");
		if(itActivate != mActivateChannel.end())
		{
			flush_printf("tm:%llu, Load Omdd Definition Config \n", dbp::tools::srv::current());
			dbp::cfg::srv::json_node* pOmdd = pDefinition->getMapNode("OMDD");
			if (nullptr == pOmdd)
			{
				std::cerr << "OMDD node not found" << std::endl;
				::close(iDefEopll);
				for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
				{
					::close(it->first);
				}
				return false;
			}
			if (dbp::cfg::srv::json_node::VECTOR != pOmdd->getType())
			{
				std::cerr << "OMDD node is not a std::vector" << std::endl;
				::close(iDefEopll);
				for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
				{
					::close(it->first);
				}
				return false;
			}
			size_t uOmddNodeSize = pOmdd->getArraySize();
			if (0 == uOmddNodeSize)
			{
				std::cerr << "OMDD node size is 0" << std::endl;
				::close(iDefEopll);
				for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
				{
					::close(it->first);
				}
				return false;
			}
			for (size_t i = 0; i < uOmddNodeSize; ++i)
			{
				std::string strInterfaceIp = "";
				std::string strMulticastIp = "";
				unsigned short int uMulticastPort = 0;
				dbp::cfg::srv::json_node* pUdpNode = pOmdd->getArrayNode(i);
				if (dbp::cfg::srv::json_node::JSON != pUdpNode->getType())
				{
					std::cerr << "OMDD node[" << i << "] is not a json" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				dbp::cfg::srv::json_node* pInterfaceIp = pUdpNode->getMapNode("InterfaceIp");
				if (nullptr == pInterfaceIp)
				{
					std::cerr << "OMDD node[" << i << "] InterfaceIp not found" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				if (dbp::cfg::srv::json_node::STRING != pInterfaceIp->getType())
				{
					std::cerr << "OMDD node[" << i << "] InterfaceIp not STRING" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				pInterfaceIp->getString(strInterfaceIp);
				dbp::cfg::srv::json_node* pMulticastIp = pUdpNode->getMapNode("MulticastIp");
				if (nullptr == pMulticastIp)
				{
					std::cerr << "OMDD node[" << i << "] MulticastIp not found" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				if (dbp::cfg::srv::json_node::STRING != pMulticastIp->getType())
				{
					std::cerr << "OMDD node[" << i << "] MulticastIp not STRING" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				pMulticastIp->getString(strMulticastIp);
				dbp::cfg::srv::json_node* pMulticastPort = pUdpNode->getMapNode("MulticastPort");
				if (nullptr == pMulticastPort)
				{
					std::cerr << "OMDD node[" << i << "] MulticastPort not found" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pMulticastPort->getType())
				{
					std::cerr << "OMDD node[" << i << "] MulticastPort not INT" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				long long llMulticastPort = pMulticastPort->getInt();
				if (llMulticastPort <= 0 || llMulticastPort > 0xFFFF)
				{
					std::cerr << "OMDD node[" << i << "] MulticastPort not in Range" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}

				uMulticastPort = (unsigned short int)llMulticastPort;
				int iUdpHandler = dbp::net::srv::getNoBlockReuseUdpListener(uMulticastPort, strMulticastIp, strInterfaceIp);
				if (iUdpHandler <= 0)
				{
					std::cerr << "OMDD node[" << i << "] Create Socket Error, MulticastIp:" << strMulticastIp <<
						", MulticastPort:" << uMulticastPort <<
						", InterfaceIp:" << strInterfaceIp << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				struct epoll_event objEvent;
				memset(&objEvent, 0, sizeof(struct epoll_event));
				objEvent.events = EPOLLIN | EPOLLET;
				objEvent.data.fd = iUdpHandler;
				if (0 != epoll_ctl(iDefEopll, EPOLL_CTL_ADD, objEvent.data.fd, &objEvent))
				{
					std::cerr << "OMDD node[" << i << "] Add to Epoll Error, MulticastIp:" << strMulticastIp <<
						", MulticastPort:" << uMulticastPort <<
						", InterfaceIp:" << strInterfaceIp << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				omddStatus[iUdpHandler].m_Status = CDefChannel::NO_READY;
				omddStatus[iUdpHandler].m_uSeq = 0;
			}
		}

		unsigned int uFinished = 0;
		epoll_event events[8192];
		char szBuffer[2048];
		while (true)
		{
			int iCnt = epoll_wait(iDefEopll, events, 8192, -1);
			if (iCnt > 0)
			{
				for (int i = 0; i < iCnt; ++i)
				{
					if (events[i].events & EPOLLIN)
					{
						int iFd = events[i].data.fd;
						if (omdcStatus.end() != omdcStatus.find(iFd))
						{
							CDefChannel& channel = omdcStatus[iFd];
							while (true)
							{
								if (recvfrom(iFd, szBuffer, sizeof(szBuffer), 0, 0, 0) <= 0)
								{
									break;
								}
								else if (channel.m_bAgain)
								{
									continue;
								}
								else
								{
									char* pszBuffer = &szBuffer[0];
									dbp::omd::COmdPkgHeader* pPkg = OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdPkgHeader);
									if (0 == pPkg->m_uMsgCnt)
									{
										continue;
									}
									pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
									if (CDefChannel::READY == channel.m_Status)
									{
										if (channel.m_uSeq + 1 != pPkg->m_uSeq)
										{
											channel.m_bAgain = true;
											channel.m_Status = CDefChannel::NO_READY;
											channel.m_uSeq = 0;
											continue;
										}
									}
									for (unsigned char i = 0; i < pPkg->m_uMsgCnt; ++i)
									{
										unsigned short int uMsgType = OMD_GET_VALUE(pszBuffer, 2, unsigned short int);
										switch (channel.m_Status)
										{
											case CDefChannel::NO_READY:
											{
												if (203 == uMsgType)
												{
													channel.m_Status = CDefChannel::READY;
													channel.m_uSeq = pPkg->m_uSeq + i;
													flush_printf("tm:%llu, Start Omdc Definition Refresh \n\n", dbp::tools::srv::current());
												}
												break;
											}
											case CDefChannel::READY:
											{
												channel.m_uSeq = pPkg->m_uSeq + i;
												if (203 == uMsgType)
												{
													channel.m_Status = CDefChannel::FINISHED;
													channel.m_bAgain = true;
													++uFinished;
													struct epoll_event objEvent;
													memset(&objEvent, 0, sizeof(struct epoll_event));
													objEvent.data.fd = iFd;
													epoll_ctl(iDefEopll, EPOLL_CTL_DEL, iFd, &objEvent);

													flush_printf("tm:%llu, Finish Omdc Definition Refresh \n\n", dbp::tools::srv::current());
												}
												else if (11 == uMsgType)
												{
													unsigned int uSecurityCode = OMD_GET_VALUE(pszBuffer, 4, unsigned int);
													omdcMap[uSecurityCode].m_SPOrderBook.m_uIdx = 0;
													omdcMap[uSecurityCode].m_SPOrderBook.m_uLotSize = OMD_GET_VALUE(pszBuffer, 195, unsigned int);
													if ('3' == OMD_GET_VALUE(pszBuffer, 31, char))
													{
														specialSpreadTableSet.insert(uSecurityCode);
													}
													auto instrument_Type = OMD_GET_STR(pszBuffer, 24, 4);
													if (instrument_Type == "WRNT")
													{
														ofs_warrantmap << OMD_GET_VALUE(pszBuffer, 4, unsigned int) << ","
																<< OMD_GET_VALUE(pszBuffer, 464, unsigned int) << std::endl;
													}
												}
												break;
											}
											case CDefChannel::FINISHED:
											{
												channel.m_bAgain = true;
												break;
											}
											default:
											{
												break;
											}
										}
										pszBuffer += OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
										if (channel.m_bAgain)
										{
											break;
										}
									}
								}
							}
							channel.m_bAgain = false;
						}
						else if (omddStatus.end() != omddStatus.find(iFd))
						{
							CDefChannel& channel = omddStatus[iFd];
							while (true)
							{
								if (recvfrom(iFd, szBuffer, sizeof(szBuffer), 0, 0, 0) <= 0)
								{
									break;
								}
								else if (channel.m_bAgain)
								{
									continue;
								}
								else
								{
									char* pszBuffer = &szBuffer[0];
									dbp::omd::COmdPkgHeader* pPkg = OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdPkgHeader);
									if (0 == pPkg->m_uMsgCnt)
									{
										continue;
									}
									pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
									if (CDefChannel::READY == channel.m_Status)
									{
										if (channel.m_uSeq + 1 != pPkg->m_uSeq)
										{
											channel.m_bAgain = true;
											channel.m_Status = CDefChannel::NO_READY;
											channel.m_uSeq = 0;
											continue;
										}
									}
									for (unsigned char i = 0; i < pPkg->m_uMsgCnt; ++i)
									{
										unsigned short int uMsgType = OMD_GET_VALUE(pszBuffer, 2, unsigned short int);
										switch (channel.m_Status)
										{
											case CDefChannel::NO_READY:
											{
												if (203 == uMsgType)
												{
													channel.m_Status = CDefChannel::READY;
													channel.m_uSeq = pPkg->m_uSeq + i;
													flush_printf("tm:%llu, Start Omdd Definition Refresh \n\n", dbp::tools::srv::current());
												}
												break;
											}
											case CDefChannel::READY:
											{
												channel.m_uSeq = pPkg->m_uSeq + i;
												if (203 == uMsgType)
												{
													channel.m_Status = CDefChannel::FINISHED;
													channel.m_bAgain = true;
													++uFinished;
													struct epoll_event objEvent;
													memset(&objEvent, 0, sizeof(struct epoll_event));
													objEvent.data.fd = iFd;
													epoll_ctl(iDefEopll, EPOLL_CTL_DEL, iFd, &objEvent);

													flush_printf("tm:%llu, Finish Omdd Definition Refresh \n\n", dbp::tools::srv::current());
												}
												else if (304 == uMsgType)
												{
													auto symbol = OMD_GET_STR(pszBuffer, 8, 32);
													auto idx = symbol.find(' ');
													if (std::string::npos != idx)
													{
														symbol = symbol.substr(0, idx);
													}
													ofs_symbolmap << symbol << ","
															<< OMD_GET_VALUE(pszBuffer, 4, unsigned int) << std::endl;
													unsigned int instrumentGroup = OMD_GET_VALUE(pszBuffer, 42, unsigned short);
													unsigned int commodityCode = OMD_GET_VALUE(pszBuffer, 44, unsigned short);
													unsigned int strike = OMD_GET_VALUE(pszBuffer, 48, unsigned int);
													std::string expdate = OMD_GET_STR(pszBuffer, 82, 4);

													if(expdate.compare("    ") == 0){
														break;
													}

													unsigned long long mInstrumentGroup = 10000000000000;
													unsigned long long mCommodityCode = 1000000000;
													unsigned long long mExpdate = 100000;
													unsigned long long mstrike = strike;
													if(mstrike > 10000){

													}else if(mstrike > 1000){
														mstrike = mstrike * 10;
													}else if(mstrike > 100){
														mstrike = mstrike * 100;
													}

													unsigned long long mykey = instrumentGroup * mInstrumentGroup +
															commodityCode * mCommodityCode + std::stoi(expdate) * mExpdate + mstrike;

													unsigned long orderbookid = OMD_GET_VALUE(pszBuffer, 4, unsigned int);
													omdcOrderodMap[mykey] = orderbookid;
													omddMap[orderbookid].m_uIdx = 0;
												}
												break;
											}
											case CDefChannel::FINISHED:
											{
												channel.m_bAgain = true;
												break;
											}
											default:
											{
												break;
											}
										}
										pszBuffer += OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
										if (channel.m_bAgain)
										{
											break;
										}
									}
								}
							}
							channel.m_bAgain = false;
						}
					}
				}
			}
			if (uFinished == omdcStatus.size() + omddStatus.size())
			{
				break;
			}
		}
		::close(iDefEopll);
		ofs_symbolmap.close();
		ofs_warrantmap.close();
		itActivate = mActivateChannel.find("OmdcChannel");
		if(itActivate != mActivateChannel.end())
		{
			for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
			{
				::close(it->first);
			}
		}
		itActivate = mActivateChannel.find("OmddChannel");
		if(itActivate != mActivateChannel.end())
		{
			for (CDefMap::iterator it = omddStatus.begin(); it != omddStatus.end(); ++it)
			{
				::close(it->first);
			}
		}
		unsigned int uCnt = 0;
		itActivate = mActivateChannel.find("OmdcChannel");
		if(itActivate != mActivateChannel.end())
		{
			for (COmdOmdcOrderMap::iterator it = omdcMap.begin(); it != omdcMap.end(); ++it)
			{
				it->second.m_SPOrderBook.m_uIdx = uCnt;
				++uCnt;
			}
		}
		uCnt = 0;
		itActivate = mActivateChannel.find("OmddChannel");
		if(itActivate != mActivateChannel.end())
		{
			for (COmdOrderMap::iterator it = omddMap.begin(); it != omddMap.end(); ++it)
			{
				it->second.m_uIdx = uCnt;
				++uCnt;
			}
		}
	}
	return true;
}
inline static bool loadRetran(dbp::cfg::srv::json_node& _json)
{
	dbp::cfg::srv::json_node* pRetranProxy = _json.getMapNode("RetranProxy");
	if (nullptr == pRetranProxy)
	{
		return false;
	}
	if (dbp::cfg::srv::json_node::VECTOR != pRetranProxy->getType())
	{
		return false;
	}
	size_t uSize = pRetranProxy->getArraySize();
	for (size_t i = 0; i < uSize; ++i)
	{
		dbp::cfg::srv::json_node* pNode = pRetranProxy->getArrayNode(i);
		if (dbp::cfg::srv::json_node::JSON != pNode->getType())
		{
			return false;
		}
		dbp::cfg::srv::json_node* pIp = pNode->getMapNode("Ip");
		if (nullptr == pIp)
		{
			return false;
		}
		if (dbp::cfg::srv::json_node::STRING != pIp->getType())
		{
			return false;
		}
		dbp::cfg::srv::json_node* pPort = pNode->getMapNode("Port");
		if (nullptr == pPort)
		{
			return false;
		}
		if (dbp::cfg::srv::json_node::INT != pPort->getType())
		{
			return false;
		}
		std::string strIp = "";
		long long llPort = pPort->getInt();
		if (llPort <= 0 || llPort > std::numeric_limits<unsigned short int>::max())
		{
			return false;
		}
		unsigned short int uPort = (unsigned short int)llPort;
		pIp->getString(strIp);
		CRetranProxy proxy;
		proxy.m_strIp = strIp;
		proxy.m_uPort = uPort;
		retranVec.push_back(proxy);
	}
	return true;
}
inline static bool loadChannel(dbp::cfg::srv::json_node& _json, const char* pszName, CStreamVec& vec)
{
	dbp::cfg::srv::json_node* pChannel = _json.getMapNode(pszName);
	if (nullptr == pChannel)
	{
		return false;
	}
	if (dbp::cfg::srv::json_node::VECTOR != pChannel->getType())
	{
		return false;
	}
	size_t uChannelSize = pChannel->getArraySize();
	for (size_t i = 0; i < uChannelSize; ++i)
	{
		dbp::cfg::srv::json_node* pInterfaceIp = 0;
		dbp::cfg::srv::json_node* pMulticastIp = 0;
		dbp::cfg::srv::json_node* pMulticastPort = 0;
		std::string strInterfaceIp = "";
		std::string strMulticastIp = "";
		long long llPort = 0;
		unsigned short int uMulticastPort = 0;
		dbp::cfg::srv::json_node* pChannelNode = pChannel->getArrayNode(i);
		if (dbp::cfg::srv::json_node::JSON != pChannelNode->getType())
		{
			return false;
		}
		dbp::cfg::srv::json_node* pChannelId = pChannelNode->getMapNode("ChannelId");
		if (nullptr == pChannelId)
		{
			return false;
		}
		if (dbp::cfg::srv::json_node::INT != pChannelId->getType())
		{
			return false;
		}
		long long llChannelId = pChannelId->getInt();
		if (llChannelId <= 0 || llChannelId > std::numeric_limits<unsigned short int>::max())
		{
			return false;
		}
		unsigned short int uChannelId = (unsigned short int)llChannelId;
		dbp::cfg::srv::json_node* pHot = pChannelNode->getMapNode("Hot");
		if (nullptr == pHot)
		{
			return false;
		}
		if (dbp::cfg::srv::json_node::JSON != pHot->getType())
		{
			return false;
		}
		pInterfaceIp = pHot->getMapNode("InterfaceIp");
		if (nullptr == pInterfaceIp)
		{
			return false;
		}
		if (dbp::cfg::srv::json_node::STRING != pInterfaceIp->getType())
		{
			return false;
		}
		pInterfaceIp->getString(strInterfaceIp);
		pMulticastIp = pHot->getMapNode("MulticastIp");
		if (nullptr == pMulticastIp)
		{
			return false;
		}
		if (dbp::cfg::srv::json_node::STRING != pMulticastIp->getType())
		{
			return false;
		}
		pMulticastIp->getString(strMulticastIp);
		pMulticastPort = pHot->getMapNode("MulticastPort");
		if (nullptr == pMulticastPort)
		{
			return false;
		}
		if (dbp::cfg::srv::json_node::INT != pMulticastPort->getType())
		{
			return false;
		}
		llPort = pMulticastPort->getInt();
		if (llPort <= 0 || llPort > std::numeric_limits<unsigned short int>::max())
		{
			return false;
		}
		uMulticastPort = (unsigned short int)llPort;
		int iHot = dbp::net::srv::getNoBlockReuseUdpListener(uMulticastPort, strMulticastIp, strInterfaceIp);
		if (-1 == iHot)
		{
			return false;
		}
		dbp::cfg::srv::json_node* pRefresh = pChannelNode->getMapNode("Refresh");
		if (nullptr == pRefresh)
		{
			close(iHot);
			return false;
		}
		if (dbp::cfg::srv::json_node::JSON != pRefresh->getType())
		{
			close(iHot);
			return false;
		}
		pInterfaceIp = pRefresh->getMapNode("InterfaceIp");
		if (nullptr == pInterfaceIp)
		{
			close(iHot);
			return false;
		}
		if (dbp::cfg::srv::json_node::STRING != pInterfaceIp->getType())
		{
			close(iHot);
			return false;
		}
		pInterfaceIp->getString(strInterfaceIp);
		pMulticastIp = pRefresh->getMapNode("MulticastIp");
		if (nullptr == pMulticastIp)
		{
			close(iHot);
			return false;
		}
		if (dbp::cfg::srv::json_node::STRING != pMulticastIp->getType())
		{
			close(iHot);
			return false;
		}
		pMulticastIp->getString(strMulticastIp);
		pMulticastPort = pRefresh->getMapNode("MulticastPort");
		if (nullptr == pMulticastPort)
		{
			close(iHot);
			return false;
		}
		if (dbp::cfg::srv::json_node::INT != pMulticastPort->getType())
		{
			close(iHot);
			return false;
		}
		llPort = pMulticastPort->getInt();
		if (llPort <= 0 || llPort > std::numeric_limits<unsigned short int>::max())
		{
			close(iHot);
			return false;
		}
		uMulticastPort = (unsigned short int)llPort;
		int iRefresh = dbp::net::srv::getNoBlockReuseUdpListener(uMulticastPort, strMulticastIp, strInterfaceIp);
		if (-1 == iRefresh)
		{
			close(iHot);
			return false;
		}
		dbp::cfg::srv::json_node* pRetranProxyIndex = pChannelNode->getMapNode("RetranProxyIndex");
		if (nullptr == pRetranProxyIndex)
		{
			close(iHot);
			close(iRefresh);
			return false;
		}
		if (dbp::cfg::srv::json_node::INT != pRetranProxyIndex->getType())
		{
			close(iHot);
			close(iRefresh);
			return false;
		}
		long long llRetranProxyIndex = pRetranProxyIndex->getInt();
		if (llRetranProxyIndex < 0 || (unsigned long long)llRetranProxyIndex > std::numeric_limits<size_t>::max())
		{
			close(iHot);
			close(iRefresh);
			return false;
		}
		unsigned long long ullQueueSize = 1000000;
		dbp::cfg::srv::json_node* pQueueSize = pChannelNode->getMapNode("QueueSize");
		if (nullptr != pQueueSize)
		{
			if (dbp::cfg::srv::json_node::INT != pQueueSize->getType())
			{
				close(iHot);
				close(iRefresh);
				return false;
			}
			long long llQueueSize = pQueueSize->getInt();
			if (llQueueSize <= 1000)
			{
				close(iHot);
				close(iRefresh);
				return false;
			}
			ullQueueSize = (unsigned long long)llQueueSize;
		}
		CStreamChannel channel;
		channel.m_iHot = iHot;
		channel.m_iRefresh = iRefresh;
		channel.m_uRetranProxyIdx = (size_t)llRetranProxyIndex;
		channel.m_uQueueSize = ullQueueSize;
		if (channel.m_uRetranProxyIdx >= retranVec.size())
		{
			close(iHot);
			close(iRefresh);
			return false;
		}
		channel.m_iEpoll = epoll_create(2);
		if (channel.m_iEpoll <= 0)
		{
			close(iHot);
			close(iRefresh);
			return false;
		}
		channel.m_uChannelId = uChannelId;
		channel.m_uChannelIdx = i;
		vec.push_back(channel);
	}
	return true;
}
inline static bool loadBrokerChannel(dbp::cfg::srv::json_node& _json)
{
	dbp::cfg::srv::json_node* pChannelNode = _json.getMapNode("BrokerQueueChannel");
	if (nullptr == pChannelNode)
	{
		return false;
	}
	if (dbp::cfg::srv::json_node::JSON != pChannelNode->getType())
	{
		return false;
	}
	dbp::cfg::srv::json_node* pInterfaceIp = 0;
	dbp::cfg::srv::json_node* pMulticastIp = 0;
	dbp::cfg::srv::json_node* pMulticastPort = 0;
	std::string strInterfaceIp = "";
	std::string strMulticastIp = "";
	long long llPort = 0;
	unsigned short int uMulticastPort = 0;
	dbp::cfg::srv::json_node* pChannelId = pChannelNode->getMapNode("ChannelId");
	if (nullptr == pChannelId)
	{
		return false;
	}
	if (dbp::cfg::srv::json_node::INT != pChannelId->getType())
	{
		return false;
	}
	long long llChannelId = pChannelId->getInt();
	if (llChannelId <= 0 || llChannelId > std::numeric_limits<unsigned short int>::max())
	{
		return false;
	}
	unsigned short int uChannelId = (unsigned short int)llChannelId;
	dbp::cfg::srv::json_node* pHot = pChannelNode->getMapNode("Hot");
	if (nullptr == pHot)
	{
		return false;
	}
	if (dbp::cfg::srv::json_node::JSON != pHot->getType())
	{
		return false;
	}
	pInterfaceIp = pHot->getMapNode("InterfaceIp");
	if (nullptr == pInterfaceIp)
	{
		return false;
	}
	if (dbp::cfg::srv::json_node::STRING != pInterfaceIp->getType())
	{
		return false;
	}
	pInterfaceIp->getString(strInterfaceIp);
	pMulticastIp = pHot->getMapNode("MulticastIp");
	if (nullptr == pMulticastIp)
	{
		return false;
	}
	if (dbp::cfg::srv::json_node::STRING != pMulticastIp->getType())
	{
		return false;
	}
	pMulticastIp->getString(strMulticastIp);
	pMulticastPort = pHot->getMapNode("MulticastPort");
	if (nullptr == pMulticastPort)
	{
		return false;
	}
	if (dbp::cfg::srv::json_node::INT != pMulticastPort->getType())
	{
		return false;
	}
	llPort = pMulticastPort->getInt();
	if (llPort <= 0 || llPort > std::numeric_limits<unsigned short int>::max())
	{
		return false;
	}
	uMulticastPort = (unsigned short int)llPort;
	int iHot = dbp::net::srv::getNoBlockReuseUdpListener(uMulticastPort, strMulticastIp, strInterfaceIp);
	if (-1 == iHot)
	{
		return false;
	}
	dbp::cfg::srv::json_node* pRefresh = pChannelNode->getMapNode("Refresh");
	if (nullptr == pRefresh)
	{
		close(iHot);
		return false;
	}
	if (dbp::cfg::srv::json_node::JSON != pRefresh->getType())
	{
		close(iHot);
		return false;
	}
	pInterfaceIp = pRefresh->getMapNode("InterfaceIp");
	if (nullptr == pInterfaceIp)
	{
		close(iHot);
		return false;
	}
	if (dbp::cfg::srv::json_node::STRING != pInterfaceIp->getType())
	{
		close(iHot);
		return false;
	}
	pInterfaceIp->getString(strInterfaceIp);
	pMulticastIp = pRefresh->getMapNode("MulticastIp");
	if (nullptr == pMulticastIp)
	{
		close(iHot);
		return false;
	}
	if (dbp::cfg::srv::json_node::STRING != pMulticastIp->getType())
	{
		close(iHot);
		return false;
	}
	pMulticastIp->getString(strMulticastIp);
	pMulticastPort = pRefresh->getMapNode("MulticastPort");
	if (nullptr == pMulticastPort)
	{
		close(iHot);
		return false;
	}
	if (dbp::cfg::srv::json_node::INT != pMulticastPort->getType())
	{
		close(iHot);
		return false;
	}
	llPort = pMulticastPort->getInt();
	if (llPort <= 0 || llPort > std::numeric_limits<unsigned short int>::max())
	{
		close(iHot);
		return false;
	}
	uMulticastPort = (unsigned short int)llPort;
	int iRefresh = dbp::net::srv::getNoBlockReuseUdpListener(uMulticastPort, strMulticastIp, strInterfaceIp);
	if (-1 == iRefresh)
	{
		close(iHot);
		return false;
	}
	dbp::cfg::srv::json_node* pRetranProxyIndex = pChannelNode->getMapNode("RetranProxyIndex");
	if (nullptr == pRetranProxyIndex)
	{
		close(iHot);
		close(iRefresh);
		return false;
	}
	if (dbp::cfg::srv::json_node::INT != pRetranProxyIndex->getType())
	{
		close(iHot);
		close(iRefresh);
		return false;
	}
	long long llRetranProxyIndex = pRetranProxyIndex->getInt();
	if (llRetranProxyIndex < 0 || (unsigned long long)llRetranProxyIndex > std::numeric_limits<size_t>::max())
	{
		close(iHot);
		close(iRefresh);
		return false;
	}

	dbp::cfg::srv::json_node* pCISBrokerIDs = pChannelNode->getMapNode("CISBrokerIDs");
	if (nullptr == pCISBrokerIDs)
	{
		close(iHot);
		close(iRefresh);
		return false;
	}
	if (dbp::cfg::srv::json_node::VECTOR != pCISBrokerIDs->getType())
	{
		close(iHot);
		close(iRefresh);
		return false;
	}
	size_t uIdCnt = pCISBrokerIDs->getArraySize();
	if (0 == uIdCnt)
	{
		close(iHot);
		close(iRefresh);
		return false;
	}

	flush_printf("CISBrokerIDs uIdCnt=%zu\n", uIdCnt);

	for (size_t i = 0; i < uIdCnt; ++i)
	{
		dbp::cfg::srv::json_node* pCISID = pCISBrokerIDs->getArrayNode(i);
		if (dbp::cfg::srv::json_node::INT != pCISID->getType())
		{
			close(iHot);
			close(iRefresh);
			return false;
		}
		long long llCISID = pCISID->getInt();
		if (llCISID <= 0 || llCISID >= std::numeric_limits<int>::max())
		{
			close(iHot);
			close(iRefresh);
			return false;
		}
		int iCISID = (int)llCISID;
		if (brokerStream.m_CISBrokerIDSet.end() != brokerStream.m_CISBrokerIDSet.find(iCISID))
		{
			return false;
		}

		brokerStream.m_CISBrokerIDSet.insert(iCISID);
	}

	for(std::unordered_set<int>::iterator a = brokerStream.m_CISBrokerIDSet.begin(); a != brokerStream.m_CISBrokerIDSet.end(); ++a)
	{
		flush_printf("loop CISBrokerID=%d\n", *a);
	}

	unsigned long long ullQueueSize = 1000000;
	dbp::cfg::srv::json_node* pQueueSize = pChannelNode->getMapNode("QueueSize");
	if (nullptr != pQueueSize)
	{
		if (dbp::cfg::srv::json_node::INT != pQueueSize->getType())
		{
			close(iHot);
			close(iRefresh);
			return false;
		}
		long long llQueueSize = pQueueSize->getInt();
		if (llQueueSize <= 1000)
		{
			close(iHot);
			close(iRefresh);
			return false;
		}
		ullQueueSize = (unsigned long long)llQueueSize;
	}
	brokerStream.m_Channel.m_iHot = iHot;
	brokerStream.m_Channel.m_iRefresh = iRefresh;
	brokerStream.m_Channel.m_uRetranProxyIdx = (size_t)llRetranProxyIndex;
	brokerStream.m_Channel.m_uQueueSize = ullQueueSize;
	if (brokerStream.m_Channel.m_uRetranProxyIdx >= retranVec.size())
	{
		close(iHot);
		close(iRefresh);
		return false;
	}
	brokerStream.m_Channel.m_iEpoll = epoll_create(2);
	if (brokerStream.m_Channel.m_iEpoll <= 0)
	{
		close(iHot);
		close(iRefresh);
		return false;
	}
	brokerStream.m_Channel.m_uChannelId = uChannelId;
	return true;
}
inline static bool loadActivateChannel(dbp::cfg::srv::json_node& _json)
{
	auto * pActivateChannel = _json.getMapNode("ActivateChannel");
	if (nullptr == pActivateChannel)
	{
		return false;
	}
	if (dbp::cfg::srv::json_node::VECTOR != pActivateChannel->getType())
	{
		return false;
	}

	size_t uSize = pActivateChannel->getArraySize();
	for (size_t i = 0; i < uSize; ++i)
	{
		auto* pNode = pActivateChannel->getArrayNode(i);
		if (dbp::cfg::srv::json_node::JSON != pNode->getType())
		{
			return false;
		}
                auto* pChannel = pNode->getMapNode("Channel");
		if (nullptr == pChannel)
		{
			return false;
		}
		if (dbp::cfg::srv::json_node::STRING != pChannel->getType())
		{
			return false;
		}

                std::string strChannel = "";
		pChannel->getString(strChannel);

		mActivateChannel[strChannel] = "";

		flush_printf("tm:%llu, Activate Channel = %s \n", dbp::tools::srv::current(), strChannel.c_str());

	}
	return true;
}

inline static bool initJson(const char* _pszJsonPath)
{
	dbp::cfg::srv::json_node json;
	std::ifstream ifs(_pszJsonPath);
	if (!ifs)
	{
		std::cerr << "Load Binary Json Error" << std::endl;
		return false;
	}
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	if (!json.fromString(strJson.c_str()))
	{
		std::cerr << "Parse Json Error" << std::endl;
		return false;
	}
	ifs.close();
	flush_printf("tm:%llu, initSOL_SOCKET \n", dbp::tools::srv::current());
	if (!dbp::net::srv::initSOL_SOCKET(json))
	{
		std::cerr << "initSOL_SOCKET fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, initSOL_TCP \n", dbp::tools::srv::current());
	if (!dbp::net::srv::initSOL_TCP(json))
	{
		std::cerr << "initSOL_TCP fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, initIPPROTO_TCP \n", dbp::tools::srv::current());
	if (!dbp::net::srv::initIPPROTO_TCP(json))
	{
		std::cerr << "initIPPROTO_TCP fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, loadCpu \n", dbp::tools::srv::current());
	if (!loadCpu(json))
	{
		std::cerr << "loadCpu fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, loadRetran \n", dbp::tools::srv::current());
	if (!loadRetran(json))
	{
		std::cerr << "loadRetran fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, loadActivateChannel \n", dbp::tools::srv::current());
	if (!loadActivateChannel(json))
	{
		std::cerr << "loadActivateChannel fail" << std::endl;
		return false;
	}
	flush_printf("tm:%llu, loadDefinition \n", dbp::tools::srv::current());

	std::string strPath = "/tmp/smfh.shm";
	dbp::cfg::srv::json_node* pShmPath = json.getMapNode("SHM_PATH");
	if (nullptr != pShmPath)
	{
		if (dbp::cfg::srv::json_node::STRING != pShmPath->getType())
		{
			std::cerr << "SHM_PATH is not a STRING" << std::endl;
			return false;
		}
		pShmPath->getString(strPath);
	}

	if (!loadDefinition(json, strPath))
	{
		std::cerr << "loadDefinition fail" << std::endl;
		return false;
	}

	auto itActivate = mActivateChannel.find("OmdcChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, loadChannel = OmdcChannel \n", dbp::tools::srv::current());
		if (!loadChannel(json, "OmdcChannel", omdcStreams))
		{
			std::cerr << "loadChannel OmdcChannel" << std::endl;
			return false;
		}
	}

	itActivate = mActivateChannel.find("OmddChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, loadChannel = OmddChannel \n", dbp::tools::srv::current());
		if (!loadChannel(json, "OmddChannel", omddStreams))
		{
			std::cerr << "loadChannel OmddChannel" << std::endl;
			return false;
		}
	}
	itActivate = mActivateChannel.find("BrokerQueueChannel");
	if(itActivate != mActivateChannel.end())
	{
		flush_printf("tm:%llu, loadChannel = BrokerQueueChannel \n", dbp::tools::srv::current());
		if (!loadBrokerChannel(json))
		{
			std::cerr << "loadChannel BrokerQueueChannel" << std::endl;
			return false;
		}
	}
	std::vector<unsigned long long> cvec;
	std::vector<unsigned long long> dvec;
	for (size_t i = 0; i < omdcStreams.size(); ++i)
	{
		cvec.push_back(omdcStreams[i].m_uQueueSize);
	}
	for (size_t i = 0; i < omddStreams.size(); ++i)
	{
		dvec.push_back(omddStreams[i].m_uQueueSize);
	}
	if(!dbp::shm::srv::getSrvShm
	(
		shm_Ref,
		strPath.c_str(),
		(unsigned long long)omdcMap.size(),
		(unsigned long long)omddMap.size(),
		(unsigned long long)omdcStreams.size(),
		(unsigned long long)omddStreams.size(),
		brokerStream.m_Channel.m_uQueueSize,
		cvec,
		dvec
	))
	{
		std::cerr << "Create SHM Error, Path:" << strPath << std::endl;
		return false;
	}
	for (COmdOmdcOrderMap::iterator it = omdcMap.begin(); it != omdcMap.end(); ++it)
	{
		shm_Ref.m_pOmdcTradable[it->second.m_SPOrderBook.m_uIdx].m_Tradable.m_uCode = it->first;
		shm_Ref.m_pOmdcTradable[it->second.m_SPOrderBook.m_uIdx].m_Tradable.m_uLotSize = it->second.m_SPOrderBook.m_uLotSize;
		shm_Ref.m_pOmdcTradable[it->second.m_SPOrderBook.m_uIdx].m_Position.m_uCode = it->first;
	}
	for (COmdOrderMap::iterator it = omddMap.begin(); it != omddMap.end(); ++it)
	{
		shm_Ref.m_pOmddTradable[it->second.m_uIdx].m_uCode = it->first;
		shm_Ref.m_pOmddTradable[it->second.m_uIdx].m_uLotSize = it->second.m_uLotSize;
	}
	return true;
}
#endif


