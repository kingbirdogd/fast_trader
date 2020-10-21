#ifndef __SMFH_SRV_CHANNEL__
#define __SMFH_SRV_CHANNEL__
#include <stdio.h>
#include <pthread.h>
#include <thread>
#include <vector>
#include <omd.h>
#include <tools.h>
#include <global_memory.hpp>
#include <unordered_set>
#include "smfh_srv_cfg.h"
#include "smfh_srv_omdc.h"
#include "smfh_srv_omdd.h"
#include "smfh_srv_omddft.h"
#include "smfh_srv_pricetable.h"
#include "smfh_srv_s1signal.h"
#include "smfh_srv_slimomdc.h"
#include "smfh_srv_swomdc.h"

#ifdef FULLTICK
#define HANDLE(MSG, TM, CHANNEL_ID) _codes.insert(OMD_GET_VALUE(MSG, 4, unsigned int)); \
	_Handler(MSG, TM);\
	WRITE_CHANNEL_DATA(MSG, TM, CHANNEL_ID)
#define CLEAR() clearBook()
#else
#define HANDLE(MSG, TM, CHANNEL_ID) _Handler(MSG, TM);\
	WRITE_CHANNEL_DATA(MSG, TM, CHANNEL_ID)
#define CLEAR()
#endif

typedef void (*PFuncOmdMsgHandler)(dbp::omd::COmdMsgHeader* _pMsg, unsigned long long _uPkgTm);
template <const PFuncOmdMsgHandler _Handler>
class ChannelHandler
{
#ifdef FULLTICK
private:
	static thread_local bool _is_omdd;
	static thread_local std::unordered_set<unsigned int> _codes;
	inline static void clearBook()
	{
		if (_is_omdd)
		{
			for (auto it = _codes.begin(); it != _codes.end(); ++it)
			{
				omddFullTickBook[*it].clear();
			}
		}
		else
		{
			for (auto it = _codes.begin(); it != _codes.end(); ++it)
			{
				omdcFullTickBook[*it].clear();
			}
		}
	}
#endif //FULLTICK
public:
	inline static bool handleChannel(const CStreamChannel& _channel)
	{
		std::cout << "handleChannel Channel:" << _channel.m_uChannelId << std::endl;
		INIT_CAHNNEL(_channel.m_uChannelId);
		struct epoll_event objEvent;
		memset(&objEvent, 0, sizeof(struct epoll_event));
		objEvent.events = EPOLLIN | EPOLLET;
		objEvent.data.fd = _channel.m_iHot;
		if (0 != epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_ADD, objEvent.data.fd, &objEvent))
		{
			return false;
		}
		objEvent.data.fd = _channel.m_iRefresh;
		if (0 != epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_ADD, objEvent.data.fd, &objEvent))
		{
			return false;
		}
		std::thread* pThread = new std::thread
		(
			[&]
			()
			{
#ifdef FULLTICK
				if (_channel.m_uChannelId > 100)
				{
					_is_omdd = true;
				}
				else
				{
					_is_omdd = false;
				}
#endif //FULLTICK
				std::cout << "Start Channel Thread:" << _channel.m_uChannelId << std::endl;
				dbp::tools::srv::blockSigPipe();
				unsigned int uCnt = 0;
				unsigned long long uSum = 0;
				while (true)
				{
					std::vector<std::vector<char>> Vec;
					char szBuffer[2048];
					epoll_event events[8192];
					unsigned int uHotSeq = 0;
					unsigned int uRefreshSeq = 0;
					unsigned int uOnRefreshStartIdx = 0;
					bool bStartRefresh = false;
					bool bAgain = false;
					bool bComplete = false;
					memset(&events, 0, sizeof(epoll_event) * 8192);
					memset(szBuffer, 0, 2048);
					while (true)
					{
						int iCnt = epoll_wait(_channel.m_iEpoll, events, 8192, -1);
						if (iCnt > 0)
						{
							for (int i = 0; i < iCnt; ++i)
							{
								if (events[i].events & EPOLLIN)
								{
									if (events[i].data.fd == _channel.m_iHot)
									{
										while (true)
										{
											if (recvfrom(events[i].data.fd, szBuffer, sizeof(szBuffer), 0, 0, 0) <= 0)
											{
												break;
											}
											else if (bAgain)
											{
												continue;
											}
											else
											{
												char* pszBuffer = &szBuffer[0];
												dbp::omd::COmdPkgHeader* pPkg = OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdPkgHeader);
												if (0 == uHotSeq)
												{
													if (0 == pPkg->m_uMsgCnt)
													{
														uOnRefreshStartIdx = pPkg->m_uSeq;
														uHotSeq = pPkg->m_uSeq;
													}
													else
													{
														uOnRefreshStartIdx = pPkg->m_uSeq;
														uHotSeq = pPkg->m_uSeq + pPkg->m_uMsgCnt - 1;
														pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
														for (unsigned char i = 0; i < pPkg->m_uMsgCnt; ++i)
														{
															unsigned short int uSize = OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
															std::vector<char> buffer;
															buffer.resize(uSize);
															memcpy (&buffer[0], pszBuffer, uSize);
															Vec.push_back(buffer);
															pszBuffer += uSize;
														}
													}
												}
												else
												{
													if (0 == pPkg->m_uMsgCnt)
													{
														if (uHotSeq < pPkg->m_uSeq)
														{
															uHotSeq = pPkg->m_uSeq;
															uOnRefreshStartIdx = pPkg->m_uSeq;
															Vec.resize(0);
														}
													}
													else
													{
														if (uHotSeq + 1 < pPkg->m_uSeq)
														{
															Vec.resize(0);
															uOnRefreshStartIdx = pPkg->m_uSeq;
															uHotSeq = pPkg->m_uSeq + pPkg->m_uMsgCnt - 1;
															pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
															for (unsigned char i = 0; i < pPkg->m_uMsgCnt; ++i)
															{
																unsigned short int uSize = OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
																std::vector<char> buffer;
																buffer.resize(uSize);
																memcpy (&buffer[0], pszBuffer, uSize);
																Vec.push_back(buffer);
																pszBuffer += uSize;
															}
														}
														else if (uHotSeq + 1 == pPkg->m_uSeq)
														{
															uHotSeq = pPkg->m_uSeq + pPkg->m_uMsgCnt - 1;
															pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
															for (unsigned char i = 0; i < pPkg->m_uMsgCnt; ++i)
															{
																unsigned short int uSize = OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
																std::vector<char> buffer;
																buffer.resize(uSize);
																memcpy (&buffer[0], pszBuffer, uSize);
																Vec.push_back(buffer);
																pszBuffer += uSize;
															}
														}
													}
												}
											}
										}
									}
									else
									{
										while (true)
										{
											if (recvfrom(events[i].data.fd, szBuffer, sizeof(szBuffer), 0, 0, 0) <= 0)
											{
												break;
											}
											else if (bAgain)
											{
												continue;
											}
											else
											{
												char* pszBuffer = &szBuffer[0];
												dbp::omd::COmdPkgHeader* pPkg = OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdPkgHeader);
												if (0 == pPkg->m_uMsgCnt)
												{
													if (bStartRefresh && uRefreshSeq != pPkg->m_uSeq)
													{
														uCnt = 0;
														uSum = 0;
														bAgain = true;
														CLEAR();
														flush_printf("tm:%llu, Refresh Fail 0, Retry, ChannelId:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId);
													}
												}
												else
												{
													if (!bStartRefresh)
													{
														pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
														for (unsigned short int i = 0; i < pPkg->m_uMsgCnt; ++i)
														{
															unsigned short int mType = OMD_GET_VALUE(pszBuffer, 2, unsigned short int);
															if (203 == mType)
															{
																if (!bStartRefresh)
																{
																	bStartRefresh = true;
																	uRefreshSeq = pPkg->m_uSeq + pPkg->m_uMsgCnt - 1;
																	flush_printf("tm:%llu, Refresh Start, ChannelId:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId);
																}
																else
																{
																	unsigned int uStartSeq = OMD_GET_VALUE(pszBuffer, 4, unsigned int) + 1;
																	if (uStartSeq < uOnRefreshStartIdx)
																	{
																		uCnt = 0;
																		uSum = 0;
																		bAgain = true;
																		CLEAR();
																		flush_printf("tm:%llu, Refresh Fail 2, Retry, ChannelId:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId);
																	}
																	else
																	{
																		flush_printf("tm:%llu, Refresh Complete 1, ChannelId:%u, uStartSeq:%u, uOnRefreshStartIdx:%u, Vec.size:%lu\n", dbp::tools::srv::current(), _channel.m_uChannelId, uStartSeq, uOnRefreshStartIdx, Vec.size());
																		for (unsigned int i =  uStartSeq - uOnRefreshStartIdx; i < Vec.size(); ++i)
																		{
																			HANDLE(OMD_GET_POINTER(&Vec[i][0], 0, dbp::omd::COmdMsgHeader), 0, _channel.m_uChannelId);
																		}
																		struct epoll_event objEvent;
																		memset(&objEvent, 0, sizeof(struct epoll_event));
																		objEvent.data.fd = _channel.m_iHot;
																		epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_DEL, objEvent.data.fd, &objEvent);
																		objEvent.data.fd = _channel.m_iRefresh;
																		epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_DEL, objEvent.data.fd, &objEvent);
																		bComplete = true;
																	}
																	break;
																}
															}
															else if (bStartRefresh)
															{
																unsigned int ucode = OMD_GET_VALUE(pszBuffer, 4, unsigned int);
																flush_printf("tm:%llu, Handle Refresh 1 , ChannelId:%u , mType: %u Code:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId, mType, ucode);
																HANDLE(OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdMsgHeader), 0, _channel.m_uChannelId);
															}
															else
															{
																unsigned int ucode = OMD_GET_VALUE(pszBuffer, 4, unsigned int);
																flush_printf("tm:%llu, ignore Refresh 1 , ChannelId:%u , mType: %u Code:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId, mType, ucode);
															}
															pszBuffer += OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
														}
													}
													else
													{
														if (uRefreshSeq + 1 < pPkg->m_uSeq)
														{
															uCnt = 0;
															uSum = 0;
															bAgain = true;
															CLEAR();
															flush_printf("tm:%llu, Refresh Fail 20, ChannelId:%u, uRefreshSeq:%u, pPkg->m_uSeq:%u, uOnRefreshStartIdx:%u, Vec.size:%lu\n", dbp::tools::srv::current(), _channel.m_uChannelId, uRefreshSeq, pPkg->m_uSeq, uOnRefreshStartIdx, Vec.size());
														}
														else if (uRefreshSeq + 1 == pPkg->m_uSeq)
														{
															pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
															uRefreshSeq = pPkg->m_uSeq + pPkg->m_uMsgCnt - 1;
															for (unsigned short int i = 0; i < pPkg->m_uMsgCnt; ++i)
															{
																unsigned short int mType = OMD_GET_VALUE(pszBuffer, 2, unsigned short int);
																//if (203 == OMD_GET_VALUE(pszBuffer, 2, unsigned short int))
																if (203 == mType)
																{
																	unsigned int uStartSeq = OMD_GET_VALUE(pszBuffer, 4, unsigned int) + 1;
																	if (uStartSeq < uOnRefreshStartIdx)
																	{
																		uCnt = 0;
																		uSum = 0;
																		bAgain = true;
																		CLEAR();
																		flush_printf("tm:%llu, Refresh Fail 2, ChannelId:%u, uStartSeq:%u, uOnRefreshStartIdx:%u, Vec.size:%lu\n", dbp::tools::srv::current(), _channel.m_uChannelId, uStartSeq, uOnRefreshStartIdx, Vec.size());
																	}
																	else
																	{
																		flush_printf("tm:%llu, Refresh Complete 2, ChannelId:%u, uStartSeq:%u, uOnRefreshStartIdx:%u, Vec.size:%lu\n", dbp::tools::srv::current(), _channel.m_uChannelId, uStartSeq, uOnRefreshStartIdx, Vec.size());
																		for (unsigned int i =  uStartSeq - uOnRefreshStartIdx; i < Vec.size(); ++i)
																		{
																			HANDLE(OMD_GET_POINTER(&Vec[i][0], 0, dbp::omd::COmdMsgHeader), 0, _channel.m_uChannelId);
																		}
																		struct epoll_event objEvent;
																		memset(&objEvent, 0, sizeof(struct epoll_event));
																		objEvent.data.fd = _channel.m_iHot;
																		epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_DEL, objEvent.data.fd, &objEvent);
																		objEvent.data.fd = _channel.m_iRefresh;
																		epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_DEL, objEvent.data.fd, &objEvent);

																		bComplete = true;
																	}
																	break;
																}
																else
																{
																	//unsigned int ucode = OMD_GET_VALUE(pszBuffer, 4, unsigned int);
																	//flush_printf("tm:%llu, Handle Refresh 2 , ChannelId:%u , mType: %u Code:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId, mType, ucode);
																	HANDLE(OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdMsgHeader), 0, _channel.m_uChannelId);
																}
																pszBuffer += OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
															}
														}
													}
												}
											}
											if (bComplete)
											{
												break;
											}
										}
									}
								}
								if (bComplete)
								{
									break;
								}
							}
						}
						if (bComplete || bAgain)
						{
							break;
						}
					}
					if (bAgain)
					{
						continue;
					}
					while (true)
					{
						if (recvfrom(_channel.m_iHot, szBuffer, sizeof(szBuffer), 0, 0, 0) <= 0)
						{
							continue;
						}
						unsigned long long uTimeStart = dbp::tools::srv::current();
						char* pszBuffer = &szBuffer[0];
						dbp::omd::COmdPkgHeader* pPkg = OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdPkgHeader);
						if (0 == uHotSeq)
						{
							if (0 == pPkg->m_uMsgCnt)
							{
								uHotSeq = pPkg->m_uSeq;
							}
							else
							{
								uHotSeq = pPkg->m_uSeq + pPkg->m_uMsgCnt - 1;
								pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
								for (unsigned char i = 0; i < pPkg->m_uMsgCnt; ++i)
								{
									HANDLE(OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdMsgHeader), pPkg->m_uTimeStamp, _channel.m_uChannelId);
									pszBuffer += OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
									unsigned long long uTimeDiff = dbp::tools::srv::current() - uTimeStart;
									uSum += uTimeDiff;
									if (uTimeDiff > 100000)
									{
										flush_printf("tm:%llu, more than 100 microsecond, ChannelId:%u, delay:%llu\n", dbp::tools::srv::current(), _channel.m_uChannelId, uTimeDiff);
									}
									++uCnt;
									if (100000 == uCnt)
									{
										flush_printf("tm:%llu, Average latency, ChannelId:%u, Average:%Lf\n", dbp::tools::srv::current(), _channel.m_uChannelId, (long double)uSum / (long double)uCnt);
										uSum = 0;
										uCnt = 0;
									}
								}
							}
						}
						else
						{
							if (0 == pPkg->m_uMsgCnt)
							{
								if (uHotSeq < pPkg->m_uSeq)
								{
									flush_printf("tm:%llu, Gap, Heart Beart ChannelId:%u, Start:%u, End:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId, uHotSeq + 1, pPkg->m_uSeq);
									std::vector<unsigned char> buffer;
									if
									(
										dbp::omd::requestRetran
										(
											retranVec[_channel.m_uRetranProxyIdx].m_strIp.c_str(),
											retranVec[_channel.m_uRetranProxyIdx].m_uPort,
											_channel.m_uChannelId,
											uHotSeq + 1,
											pPkg->m_uSeq,
											buffer
										)
									)
									{
										unsigned int uCnt = pPkg->m_uSeq - uHotSeq;
										unsigned char* retranBuffer = &buffer[0];
										for (unsigned int i = 0; i < uCnt; ++i)
										{
											HANDLE(OMD_GET_POINTER(retranBuffer, 0, dbp::omd::COmdMsgHeader), 0, _channel.m_uChannelId);
											retranBuffer += OMD_GET_VALUE(retranBuffer, 0, unsigned short int);
										}
										uHotSeq = pPkg->m_uSeq;
									}
									else
									{
										flush_printf("tm:%llu, Retran Fail, Heart Beat Hot, ChannelId:%u, Start:%u, End:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId, uHotSeq + 1, pPkg->m_uSeq);
										struct epoll_event objEvent;
										memset(&objEvent, 0, sizeof(struct epoll_event));
										objEvent.events = EPOLLIN | EPOLLET;
										objEvent.data.fd = _channel.m_iHot;
										epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_ADD, objEvent.data.fd, &objEvent);
										objEvent.data.fd = _channel.m_iRefresh;
										epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_ADD, objEvent.data.fd, &objEvent);
										uCnt = 0;
										uSum = 0;
										CLEAR();
										break;
									}
								}
							}
							else
							{
								if (uHotSeq + 1 < pPkg->m_uSeq)
								{
									flush_printf("tm:%llu, Gap, ChannelId:%u, Start:%u, End:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId, uHotSeq + 1, pPkg->m_uSeq - 1);
									std::vector<unsigned char> buffer;
									if
									(
										dbp::omd::requestRetran
										(
											retranVec[_channel.m_uRetranProxyIdx].m_strIp.c_str(),
											retranVec[_channel.m_uRetranProxyIdx].m_uPort,
											_channel.m_uChannelId,
											uHotSeq + 1,
											pPkg->m_uSeq - 1,
											buffer
										)
									)
									{
										unsigned int uCnt = pPkg->m_uSeq - 1 - uHotSeq;
										unsigned char* retranBuffer = &buffer[0];
										for (unsigned int i = 0; i < uCnt; ++i)
										{
											HANDLE(OMD_GET_POINTER(retranBuffer, 0, dbp::omd::COmdMsgHeader), 0, _channel.m_uChannelId);
											retranBuffer += OMD_GET_VALUE(retranBuffer, 0, unsigned short int);
										}
										uHotSeq = pPkg->m_uSeq + pPkg->m_uMsgCnt - 1;
										pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
										for (unsigned char i = 0; i < pPkg->m_uMsgCnt; ++i)
										{
											HANDLE(OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdMsgHeader), pPkg->m_uTimeStamp, _channel.m_uChannelId);
											pszBuffer += OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
										}
									}
									else
									{
										flush_printf("tm:%llu, Retran Fail, ChannelId:%u, Start:%u, End:%u\n", dbp::tools::srv::current(), _channel.m_uChannelId, uHotSeq + 1, pPkg->m_uSeq - 1);
										struct epoll_event objEvent;
										memset(&objEvent, 0, sizeof(struct epoll_event));
										objEvent.events = EPOLLIN | EPOLLET;
										objEvent.data.fd = _channel.m_iHot;
										epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_ADD, objEvent.data.fd, &objEvent);
										objEvent.data.fd = _channel.m_iRefresh;
										epoll_ctl(_channel.m_iEpoll, EPOLL_CTL_ADD, objEvent.data.fd, &objEvent);
										uCnt = 0;
										uSum = 0;
										CLEAR();
										break;
									}
								}
								else if (uHotSeq + 1 == pPkg->m_uSeq)
								{
									uHotSeq = pPkg->m_uSeq + pPkg->m_uMsgCnt - 1;
									pszBuffer += sizeof(dbp::omd::COmdPkgHeader);
									for (unsigned char i = 0; i < pPkg->m_uMsgCnt; ++i)
									{
										HANDLE(OMD_GET_POINTER(pszBuffer, 0, dbp::omd::COmdMsgHeader), pPkg->m_uTimeStamp, _channel.m_uChannelId);
										pszBuffer += OMD_GET_VALUE(pszBuffer, 0, unsigned short int);
										unsigned long long uTimeDiff = dbp::tools::srv::current() - uTimeStart;
										uSum += uTimeDiff;
										if (uTimeDiff > 100000)
										{
											flush_printf("tm:%llu, more than 100 microsecond, ChannelId:%u, delay:%llu\n", dbp::tools::srv::current(), _channel.m_uChannelId, uTimeDiff);
										}
										++uCnt;
										//100000 >> 1000000
										if (100000 == uCnt)
										{
											flush_printf("tm:%llu, Average latency, ChannelId:%u, Average:%Lf\n", dbp::tools::srv::current(), _channel.m_uChannelId, (long double)uSum / (long double)uCnt);
											uSum = 0;
											uCnt = 0;
										}
									}
								}
							}
						}
					}
				}
			}
		);

		pthread_t iThread = pThread->native_handle();
#ifndef __APPLE__
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(cpuInfo.getCore(), &cpuset);
		pthread_setaffinity_np(iThread, sizeof(cpu_set_t), &cpuset);
#endif
		struct sched_param sch;
		memset (&sch, 0, sizeof(struct sched_param));
		int iPolicy = 0;
		if (0 != pthread_getschedparam(iThread, &iPolicy, &sch))
		{
			return false;
		}
		sch.sched_priority = SCHED_PRIORITY;
		if (0 != pthread_setschedparam(iThread, SCHED_TYPE, &sch))
		{
			return false;
		}
		return true;
	}
	inline static bool handleChannelVec(const CStreamVec& _Vec)
	{
		for (size_t i = 0; i < _Vec.size(); ++i)
		{
			if (!handleChannel(_Vec[i]))
			{
				return false;
			}
		}
		return true;
	}
};
inline static bool startStockWarrantOmdcChannel()
{
	return ChannelHandler<handleStockWarrantOmdc>::handleChannelVec(omdcStreams);
}
inline static bool startSlimOmdcChannel()
{
	return ChannelHandler<handleSlimOmdc>::handleChannelVec(omdcStreams);
}
inline static bool startOmdcChannel()
{
	return ChannelHandler<handleOmdc>::handleChannelVec(omdcStreams);
}
inline static bool startOmddChannel()
{
	return ChannelHandler<handleOmdd>::handleChannelVec(omddStreams);
}
inline static bool startOmddftChannel()
{
	return ChannelHandler<handleOmddFt>::handleChannelVec(omddStreams);
}
inline static bool startPriceTableChannel()
{
	return ChannelHandler<handlePricetable>::handleChannelVec(pricetableStreams);
}
inline static bool startS1Channel()
{
	return ChannelHandler<handleS1Signal>::handleChannelVec(s1Streams);
}

#ifdef FULLTICK
template <const PFuncOmdMsgHandler _Handler>
thread_local bool ChannelHandler<_Handler>::_is_omdd = false;

template <const PFuncOmdMsgHandler _Handler>
thread_local std::unordered_set<unsigned int> ChannelHandler<_Handler>::_codes;
#endif //FULLTICK

#endif



