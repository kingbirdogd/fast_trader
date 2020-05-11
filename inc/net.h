#ifndef __DBP_NET_SRV__
#define __DBP_NET_SRV__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <epoll.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>
#include <json_node.h>
#ifdef __APPLE__
#define TCP_QUICKACK TCP_NODELAY
#define SOL_TCP IPPROTO_TCP
#define TCP_KEEPIDLE TCP_KEEPALIVE
#endif
namespace dbp
{
	namespace net
	{
		namespace srv
		{
			typedef ssize_t (*PRecvFunc) (int sockfd, void *buf, size_t len, int flags);
			static PRecvFunc pfunc_recv = nullptr;
			static struct timeval tvSO_RCVTIMEO;
			static struct timeval tvSO_SNDTIMEO;
			static struct linger linSO_LINGER;
			static int iSO_KEEPALIVE = 1;
			static int iSO_RCVBUF =  131072;
			static int iSO_SNDBUF =  131072;
			static int iTCP_NODELAY =  1;
			static int iTCP_CORK = 1;
			static int iTCP_KEEPCNT =  5;
			static int iTCP_KEEPIDLE = 30;
			static int iTCP_KEEPINTVL = 120;
			inline static ssize_t recv_noquick_ack(int sockfd, void *buf, size_t len, int flags)
			{
				return recv(sockfd, buf, len, flags);
			}
			inline static ssize_t recv_quick_ack(int sockfd, void *buf, size_t len, int flags)
			{
				int iRt = recv(sockfd, buf, len, flags);
				if (0 >= iRt)
				{
					return iRt;
				}
				else
				{
					int iFlag = 1;
					int iOptRt = setsockopt(sockfd, IPPROTO_TCP, TCP_QUICKACK, (char*)(&iFlag), sizeof(int));
					if (0 != iOptRt)
					{
						return -1;
					}
					else
					{
						return iRt;
					}
				}
			}
			inline static bool initSOL_SOCKET(dbp::cfg::srv::json_node& _json)
			{
				dbp::cfg::srv::json_node* pSOL_SOCKET = _json.getMapNode("SOL_SOCKET");
				if (nullptr == pSOL_SOCKET)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::JSON != pSOL_SOCKET->getType())
				{
					return false;
				}
				dbp::cfg::srv::json_node* pSO_KEEPALIVE = pSOL_SOCKET->getMapNode("SO_KEEPALIVE");
				if (nullptr == pSO_KEEPALIVE)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::BOOL != pSO_KEEPALIVE->getType())
				{
					return false;
				}
				if (pSO_KEEPALIVE->getBool())
				{
					iSO_KEEPALIVE = 1;
				}
				else
				{
					iSO_KEEPALIVE = 0;
				}
				dbp::cfg::srv::json_node* pSO_RCVTIMEO = pSOL_SOCKET->getMapNode("SO_RCVTIMEO");
				if (nullptr == pSO_RCVTIMEO)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pSO_RCVTIMEO->getType())
				{
					return false;
				}
				long long llSO_RCVTIMEO = pSO_RCVTIMEO->getInt();
				if (llSO_RCVTIMEO <= 0)
				{
					return false;
				}
				unsigned long long uSO_RCVTIMEO = (int)llSO_RCVTIMEO;
				tvSO_RCVTIMEO.tv_sec = uSO_RCVTIMEO / 1000000;
				tvSO_RCVTIMEO.tv_usec = uSO_RCVTIMEO % 1000000;
				dbp::cfg::srv::json_node* pSO_SNDTIMEO = pSOL_SOCKET->getMapNode("SO_SNDTIMEO");
				if (nullptr == pSO_SNDTIMEO)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pSO_SNDTIMEO->getType())
				{
					return false;
				}
				long long llSO_SNDTIMEO = pSO_SNDTIMEO->getInt();
				if (llSO_SNDTIMEO <= 0)
				{
					return false;
				}
				unsigned long long uSO_SNDTIMEO = (int)llSO_SNDTIMEO;
				tvSO_SNDTIMEO.tv_sec = uSO_SNDTIMEO / 1000000;
				tvSO_SNDTIMEO.tv_usec = uSO_SNDTIMEO % 1000000;
				dbp::cfg::srv::json_node* pSO_LINGER = pSOL_SOCKET->getMapNode("SO_LINGER");
				if (nullptr == pSO_LINGER)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pSO_LINGER->getType())
				{
					return false;
				}
				long long llSO_LINGER = pSO_LINGER->getInt();
				if (llSO_LINGER < 0 || llSO_LINGER > std::numeric_limits<int>::max())
				{
					return false;
				}
				int iSO_LINGER = (int)llSO_LINGER;
				if (0 == iSO_LINGER)
				{
					linSO_LINGER.l_onoff = 0;
					linSO_LINGER.l_linger = 0;
				}
				else
				{
					linSO_LINGER.l_onoff = 1;
					linSO_LINGER.l_linger = iSO_LINGER;
				}
				dbp::cfg::srv::json_node* pSO_RCVBUF = pSOL_SOCKET->getMapNode("SO_RCVBUF");
				if (nullptr == pSO_RCVBUF)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pSO_RCVBUF->getType())
				{
					return false;
				}
				long long llSO_RCVBUF = pSO_RCVBUF->getInt();
				if (llSO_RCVBUF <= 0 || llSO_RCVBUF > std::numeric_limits<int>::max())
				{
					return false;
				}
				iSO_RCVBUF = (int)llSO_RCVBUF;
				dbp::cfg::srv::json_node* pSO_SNDBUF = pSOL_SOCKET->getMapNode("SO_SNDBUF");
				if (nullptr == pSO_SNDBUF)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pSO_SNDBUF->getType())
				{
					return false;
				}
				long long llSO_SNDBUF = pSO_SNDBUF->getInt();
				if (llSO_SNDBUF <= 0 || llSO_SNDBUF > std::numeric_limits<int>::max())
				{
					return false;
				}
				iSO_SNDBUF = (int)llSO_SNDBUF;
				return true;
			}
			inline static bool initSOL_TCP(dbp::cfg::srv::json_node& _json)
			{
				dbp::cfg::srv::json_node* pSOL_TCP = _json.getMapNode("SOL_TCP");
				if (nullptr == pSOL_TCP)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::JSON != pSOL_TCP->getType())
				{
					return false;
				}
				dbp::cfg::srv::json_node* pTCP_NODELAY = pSOL_TCP->getMapNode("TCP_NODELAY");
				if (nullptr == pTCP_NODELAY)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::BOOL != pTCP_NODELAY->getType())
				{
					return false;
				}
				if (pTCP_NODELAY->getBool())
				{
					iTCP_NODELAY = 1;
				}
				else
				{
					iTCP_NODELAY = 0;
				}
				dbp::cfg::srv::json_node* pTCP_CORK = pSOL_TCP->getMapNode("TCP_CORK");
				if (nullptr == pTCP_CORK)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::BOOL != pTCP_CORK->getType())
				{
					return false;
				}
				if (pTCP_CORK->getBool())
				{
					iTCP_CORK = 1;
				}
				else
				{
					iTCP_CORK = 0;
				}
				return true;
			}
			inline static bool initIPPROTO_TCP(dbp::cfg::srv::json_node& _json)
			{
				dbp::cfg::srv::json_node* pIPPROTO_TCP = _json.getMapNode("IPPROTO_TCP");
				if (nullptr == pIPPROTO_TCP)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::JSON != pIPPROTO_TCP->getType())
				{
					return false;
				}
				dbp::cfg::srv::json_node* pTCP_QUICKACK = pIPPROTO_TCP->getMapNode("TCP_QUICKACK");
				if (nullptr == pTCP_QUICKACK)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::BOOL != pTCP_QUICKACK->getType())
				{
					return false;
				}
				if (pTCP_QUICKACK->getBool())
				{
					pfunc_recv = recv_quick_ack;
				}
				else
				{
					pfunc_recv = recv_noquick_ack;
				}
				dbp::cfg::srv::json_node* pTCP_KEEPCNT = pIPPROTO_TCP->getMapNode("TCP_KEEPCNT");
				if (nullptr == pTCP_KEEPCNT)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pTCP_KEEPCNT->getType())
				{
					return false;
				}
				long long llTCP_KEEPCNT = pTCP_KEEPCNT->getInt();
				if (llTCP_KEEPCNT <= 0 || llTCP_KEEPCNT > std::numeric_limits<int>::max())
				{
					return false;
				}
				iTCP_KEEPCNT = (int)llTCP_KEEPCNT;
				dbp::cfg::srv::json_node* pTCP_KEEPIDLE = pIPPROTO_TCP->getMapNode("TCP_KEEPIDLE");
				if (nullptr == pTCP_KEEPIDLE)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pTCP_KEEPIDLE->getType())
				{
					return false;
				}
				long long llTCP_KEEPIDLE = pTCP_KEEPIDLE->getInt();
				if (llTCP_KEEPIDLE <= 0 || llTCP_KEEPIDLE > std::numeric_limits<int>::max())
				{
					return false;
				}
				iTCP_KEEPIDLE = (int)llTCP_KEEPIDLE;
				dbp::cfg::srv::json_node* pTCP_KEEPINTVL = pIPPROTO_TCP->getMapNode("TCP_KEEPINTVL");
				if (nullptr == pTCP_KEEPINTVL)
				{
					return false;
				}
				if (dbp::cfg::srv::json_node::INT != pTCP_KEEPINTVL->getType())
				{
					return false;
				}
				long long llTCP_KEEPINTVL = pTCP_KEEPINTVL->getInt();
				if (llTCP_KEEPINTVL <= 0 || llTCP_KEEPINTVL > std::numeric_limits<int>::max())
				{
					return false;
				}
				iTCP_KEEPINTVL = (int)llTCP_KEEPINTVL;
				return true;
			}
			inline static bool setSocketOpt(int sockfd)
			{
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tvSO_RCVTIMEO,  sizeof(struct timeval)))
				{
					return false;
				}
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&tvSO_SNDTIMEO,  sizeof(struct timeval)))
				{
					return false;
				}
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char*)&linSO_LINGER,  sizeof(struct linger)))
				{
					return false;
				}
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &iSO_KEEPALIVE, sizeof(int)))
				{
					return false;
				}
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &iSO_RCVBUF, sizeof(int)))
				{
					return false;
				}
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &iSO_SNDBUF, sizeof(int)))
				{
					return false;
				}
				return true;
			}
			inline static bool setTcpOpt(int sockfd)
			{
				if (0 != setsockopt(sockfd, SOL_TCP, TCP_NODELAY, &iTCP_NODELAY, sizeof(int)))
				{
					return false;
				}
				if (0 != setsockopt(sockfd, SOL_TCP, TCP_NODELAY, &iTCP_CORK, sizeof(int)))
				{
					return false;
				}
				if (0 != setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &iTCP_KEEPCNT, sizeof(int)))
				{
					return false;
				}
				if (0 != setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &iTCP_KEEPIDLE, sizeof(int)))
				{
					return false;
				}
				if (0 != setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &iTCP_KEEPINTVL, sizeof(int)))
				{
					return false;
				}
				return true;
			}
			inline static bool setAddressReuse(int sockfd)
			{
				int iFlag = 1;
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &iFlag, sizeof(int)))
				{
					return false;
				}
				return true;
			}
			inline static bool disableAddressReuse(int sockfd)
			{
				int iFlag = 0;
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &iFlag, sizeof(int)))
				{
					return false;
				}
				return true;
			}
			inline static bool setPortReuse(int sockfd)
			{
				int iFlag = 1;
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &iFlag, sizeof(int)))
				{
					return false;
				}
				return true;
			}
			inline static bool disablePortReuse(int sockfd)
			{
				int iFlag = 0;
				if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &iFlag, sizeof(int)))
				{
					return false;
				}
				return true;
			}
			inline static bool setTcpSocketOpt(int sockfd)
			{
				if (!setSocketOpt(sockfd))
				{
					return false;
				}
				return setTcpOpt(sockfd);
			}
			inline static bool setNoBlock(int iHandler)
			{
				int iOptions = fcntl(iHandler, F_GETFL);
				if (iOptions < 0)
				{
					return false;
				}
				iOptions |= O_NONBLOCK;
				if (fcntl(iHandler, F_SETFL, iOptions) < 0)
				{
					return false;
				}
				return true;
			}
			inline static bool setBlock(int iHandler)
			{
				int iOptions = fcntl(iHandler, F_GETFL);
				if (iOptions < 0)
				{
					return false;
				}
				iOptions &= (~O_NONBLOCK);
				if (fcntl(iHandler, F_SETFL, iOptions) < 0)
				{
					return false;
				}
				return true;
			}
			inline static int getNoBlockReuseTcpAccpetor(unsigned short int uPort, int iPoller, std::string strAddr = "")
			{
				int iRt = socket(AF_INET, SOCK_STREAM, 0);
				if (iRt <= 0)
				{
					return iRt;
				}
				if (!setSocketOpt(iRt))
				{
					close(iRt);
					return -1;
				}
				if (!setNoBlock(iRt))
				{
					close(iRt);
					return -1;
				}
				if (!setAddressReuse(iRt))
				{
					close(iRt);
					return -1;
				}
				struct sockaddr_in bindaddr;
				bindaddr.sin_family= AF_INET;
				bindaddr.sin_port = htons(uPort);
				if (strAddr == "")
				{
					bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
				}
				else
				{
					bindaddr.sin_addr.s_addr = inet_addr(strAddr.c_str());
				}
				if (0 != bind(iRt, (struct sockaddr*)&bindaddr, sizeof(struct sockaddr_in)))
				{
					return -1;
				}
				if (0 != listen(iRt, 32))
				{
					close(iRt);
					return -1;
				}
				struct epoll_event objEvent;
				memset(&objEvent, 0, sizeof(struct epoll_event));
				objEvent.data.fd = iRt;
				objEvent.events = EPOLLIN | EPOLLET;
				if (0 != epoll_ctl(iPoller, EPOLL_CTL_ADD, iRt, &objEvent))
				{
					close(iRt);
					return -1;
				}
				return iRt;
			}
			inline static int getNoBlockReuseUdpListener(unsigned short int uPort, std::string strGroupIp, std::string strInterfaceIp = "")
			{
				int iRt = socket(AF_INET, SOCK_DGRAM, 0);
				if (iRt <= 0)
				{
					return iRt;
				}
				if (!setSocketOpt(iRt))
				{
					close(iRt);
					return -1;
				}
				if (!setNoBlock(iRt))
				{
					close(iRt);
					return -1;
				}
				if (!setAddressReuse(iRt))
				{
					close(iRt);
					return -1;
				}
				/*
				if (!setPortReuse(iRt))
				{
					close(iRt);
					return -1;
				}*/
				sockaddr_in groupAddr;
				memset(&groupAddr, 0, sizeof(sockaddr_in));
				groupAddr.sin_family = AF_INET;
				groupAddr.sin_addr.s_addr = inet_addr(strGroupIp.c_str());
				groupAddr.sin_port = htons(uPort);
				if (0 != bind(iRt, (struct sockaddr*)(&groupAddr), sizeof(sockaddr_in)))
				{
					close(iRt);
					return -1;
				}
				if (strInterfaceIp != "")
				{
					struct ip_mreq interFace;
					interFace.imr_multiaddr.s_addr = inet_addr(strGroupIp.c_str());
					interFace.imr_interface.s_addr = inet_addr(strInterfaceIp.c_str());
					if (0 != setsockopt(iRt, IPPROTO_IP, IP_ADD_MEMBERSHIP, &interFace, sizeof(ip_mreq)))
					{
						close(iRt);
						return -1;
					}
				}
				return iRt;
			}
			inline static int getBlockReuseUdpListener(unsigned short int uPort, std::string strGroupIp, std::string strInterfaceIp = "")
			{
				int iRt = socket(AF_INET, SOCK_DGRAM, 0);
				if (iRt <= 0)
				{
					return iRt;
				}
				if (!setSocketOpt(iRt))
				{
					close(iRt);
					return -1;
				}
				if (!setAddressReuse(iRt))
				{
					close(iRt);
					return -1;
				}
				/*
				if (!setPortReuse(iRt))
				{
					close(iRt);
					return -1;
				}
				*/
				sockaddr_in groupAddr;
				memset(&groupAddr, 0, sizeof(sockaddr_in));
				groupAddr.sin_family = AF_INET;
				groupAddr.sin_addr.s_addr = inet_addr(strGroupIp.c_str());
				groupAddr.sin_port = htons(uPort);
				if (0 != bind(iRt, (struct sockaddr*)(&groupAddr), sizeof(sockaddr_in)))
				{
					close(iRt);
					return -1;
				}
				if (strInterfaceIp != "")
				{
					struct ip_mreq interFace;
					interFace.imr_multiaddr.s_addr = inet_addr(strGroupIp.c_str());
					interFace.imr_interface.s_addr = inet_addr(strInterfaceIp.c_str());
					if (0 != setsockopt(iRt, IPPROTO_IP, IP_ADD_MEMBERSHIP, &interFace, sizeof(ip_mreq)))
					{
						close(iRt);
						return -1;
					}
				}
				return iRt;
			}
		}
	}
}
#endif


