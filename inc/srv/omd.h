#ifndef __DBP_OMD_SRV__
#define __DBP_OMD_SRV__
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <string>
#include <vector>
namespace dbp
{
	namespace omd
	{
		namespace srv
		{
			#pragma pack(push, 1)
			class COmdRetranTcpProxyRequestMsg
			{
			public:
				unsigned short int m_uChannelId;
				unsigned char m_szFiller2[2];
				unsigned int m_uStartSeq;
				unsigned int m_uEndSeq;
				unsigned char m_szFiller4[4];
				unsigned long long m_uRandom1;
				unsigned long long m_uRandom2;
			};
			class COmdRetranTcpProxyReponseMsgHeader
			{
			public:
				unsigned long long m_uMsgSize;
				unsigned long long m_uRandom1;
				unsigned long long m_uRandom2;
				unsigned short int m_uChannelId;
				unsigned char m_szFiller2[2];
				unsigned int m_uStartSeq;
				unsigned int m_uEndSeq;
				unsigned char m_szFiller4[4];
			};
			#pragma pack(pop)
			static inline bool requestRetran
			(
				const char* _pszIp,
				unsigned short int _uPort,
				unsigned short int _uChannelId,
				unsigned int _uStart,
				unsigned int _uEnd,
				std::vector<unsigned char>& rtVec
			)
			{
				int iSockFd = socket(AF_INET,SOCK_STREAM,0);
				if (iSockFd <= 0)
				{
					return false;
				}
				struct sockaddr_in sockAddr;
				memset(&sockAddr, 0, sizeof(struct sockaddr_in));
				sockAddr.sin_family = AF_INET;
				sockAddr.sin_addr.s_addr = inet_addr(_pszIp);
				sockAddr.sin_port = htons(_uPort);
				if (0 != connect(iSockFd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)))
				{
					close(iSockFd);
					return false;
				}
				COmdRetranTcpProxyRequestMsg requestMsg;
				requestMsg.m_uChannelId = _uChannelId;
				requestMsg.m_uStartSeq = _uStart;
				requestMsg.m_uEndSeq = _uEnd;
				struct timeval tv;
				gettimeofday(&tv, 0);
				unsigned int iSeed = tv.tv_usec;
				requestMsg.m_uRandom1 = rand_r(&iSeed);
				gettimeofday(&tv, 0);
				iSeed = tv.tv_usec;
				requestMsg.m_uRandom2 = rand_r(&iSeed);
				char* pszBuffer = (char*)&requestMsg;
				unsigned int uSize = sizeof(COmdRetranTcpProxyRequestMsg);
				while (uSize > 0)
				{
					unsigned int uSendSize = send(iSockFd, pszBuffer, uSize, 0);
					if (uSendSize <= 0)
					{
						close(iSockFd);
						return false;
					}
					uSize -= uSendSize;
					pszBuffer += uSendSize;
				}
				COmdRetranTcpProxyReponseMsgHeader responseMsg;
				pszBuffer = (char*)&responseMsg;
				uSize = sizeof(COmdRetranTcpProxyReponseMsgHeader);
				while (uSize > 0)
				{
					unsigned int uRecvSize = recv(iSockFd, pszBuffer, uSize, 0);
					if (uRecvSize <= 0)
					{
						close(iSockFd);
						return false;
					}
					uSize -= uRecvSize;
					pszBuffer += uRecvSize;
				}
				if (responseMsg.m_uMsgSize < sizeof (COmdRetranTcpProxyReponseMsgHeader))
				{
					close(iSockFd);
					return false;
				}
				if (responseMsg.m_uRandom1 != requestMsg.m_uRandom1 ||
						responseMsg.m_uRandom2 != requestMsg.m_uRandom2)
				{
					close(iSockFd);
					return false;
				}
				if (responseMsg.m_uMsgSize == sizeof (COmdRetranTcpProxyReponseMsgHeader))
				{
					close(iSockFd);
					return false;
				}
				uSize = responseMsg.m_uMsgSize - sizeof (COmdRetranTcpProxyReponseMsgHeader);
				char recvBuffer[2048];
				rtVec.resize(4096);
				rtVec.resize(0);
				while (uSize > 0)
				{
					unsigned int uRecvSize = recv(iSockFd, recvBuffer, 2048, 0);
					if (uRecvSize <= 0)
					{
						close(iSockFd);
						return false;
					}
					uSize -= uRecvSize;
					size_t uPost = rtVec.size();
					rtVec.resize(uPost + uRecvSize);
					memcpy(&rtVec[uPost], recvBuffer, uRecvSize);
				}
				close(iSockFd);
				return true;
			}
		}
	}
}
#endif


