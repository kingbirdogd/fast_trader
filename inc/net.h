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
#include <json.hpp>
#include <iostream>
#ifdef __APPLE__
#define TCP_QUICKACK TCP_NODELAY
#define SOL_TCP IPPROTO_TCP
#define TCP_KEEPIDLE TCP_KEEPALIVE
#endif

using json = nlohmann::json;

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
			inline static bool initSOL_SOCKET(json& _json)
			{
				try
				{
					auto& SOL_SOCKET_Node = _json["SOL_SOCKET"];
					auto bSO_KEEPALIVE = SOL_SOCKET_Node["SO_KEEPALIVE"].get<bool>();
					if (bSO_KEEPALIVE)
					{
						iSO_KEEPALIVE = 1;
					}
					else
					{
						iSO_KEEPALIVE = 0;
					}
					unsigned long long uSO_RCVTIMEO =
							SOL_SOCKET_Node["SO_RCVTIMEO"].get<unsigned long long>();
					tvSO_RCVTIMEO.tv_sec = uSO_RCVTIMEO / 1000000;
					tvSO_RCVTIMEO.tv_usec = uSO_RCVTIMEO % 1000000;
					unsigned long long uSO_SNDTIMEO =
							SOL_SOCKET_Node["SO_SNDTIMEO"].get<unsigned long long>();
					tvSO_SNDTIMEO.tv_sec = uSO_SNDTIMEO / 1000000;
					tvSO_SNDTIMEO.tv_usec = uSO_SNDTIMEO % 1000000;
					int iSO_LINGER = SOL_SOCKET_Node["SO_LINGER"].get<int>();
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
					iSO_RCVBUF = SOL_SOCKET_Node["SO_RCVBUF"].get<int>();
					iSO_SNDBUF = SOL_SOCKET_Node["SO_SNDBUF"].get<int>();
				}
				catch(...)
				{
					std::cerr << "load initSOL_SOCKET fail" <<  std::endl;
					return false;
				}
				return true;
			}
			inline static bool initSOL_TCP(json& _json)
			{
				try
				{
					auto& SOL_TCP_Node = _json["SOL_TCP"];
					bool bTCP_NODELAY = SOL_TCP_Node["TCP_NODELAY"].get<bool>();
					if (bTCP_NODELAY)
					{
						iTCP_NODELAY = 1;
					}
					else
					{
						iTCP_NODELAY = 0;
					}
					bool bTCP_CORK = SOL_TCP_Node["TCP_CORK"].get<bool>();
					if (bTCP_CORK)
					{
						iTCP_CORK = 1;
					}
					else
					{
						iTCP_CORK = 0;
					}
				}
				catch(...)
				{
					std::cerr << "load initSOL_TCP fail" <<  std::endl;
					return false;
				}
				return true;
			}
			inline static bool initIPPROTO_TCP(json& _json)
			{
				try
				{
					auto& IPPROTO_TCP_Node = _json["IPPROTO_TCP"];
					bool bTCP_QUICKACK = IPPROTO_TCP_Node["TCP_QUICKACK"].get<bool>();
					if (bTCP_QUICKACK)
					{
						pfunc_recv = recv_quick_ack;
					}
					else
					{
						pfunc_recv = recv_noquick_ack;
					}
					iTCP_KEEPCNT = IPPROTO_TCP_Node["TCP_KEEPCNT"].get<int>();
					iTCP_KEEPIDLE = IPPROTO_TCP_Node["TCP_KEEPIDLE"].get<int>();
					iTCP_KEEPINTVL = IPPROTO_TCP_Node["TCP_KEEPINTVL"].get<int>();
				}
				catch (...)
				{
					std::cerr << "load initIPPROTO_TCP fail" <<  std::endl;
					return false;
				}
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
				if (!setPortReuse(iRt))
				{
					close(iRt);
					return -1;
				}
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
						std::cout << "IP_ADD_MEMBERSHIP error:" << strerror(errno) << std::endl;
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


