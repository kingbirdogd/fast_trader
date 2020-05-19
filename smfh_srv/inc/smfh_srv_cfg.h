#ifndef __SMFH_SRV_CFG__
#define __SMFH_SRV_CFG__
#include <sys/types.h>
#include <sys/socket.h>
#include <epoll.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <json.hpp>
#include <tools.h>
#include <net.h>
#include <global_memory.hpp>
#include <user.hpp>
#include <limits>

using json = nlohmann::json;

inline static bool loadCpu(json& _json)
{
	try
	{
		auto& cpu = _json["CPU"];
		for (std::size_t i = 0; i < cpu.size(); ++i)
		{
			int iVavlue = cpu[i].get<int>();
			if (!cpuInfo.setCpu(iVavlue))
			{
				std::cerr << "CPU[" << i << "] init fail" << std::endl;
				return false;
			}
		}
	}
	catch(...)
	{
		std::cerr << "loadCpu fail" << std::endl;
		return false;
	}
	return true;
}

inline static bool loadInOut(json& _json)
{
	try
	{
		auto command = _json["IN_OUT_PROCESS"].get<std::string>();
		auto convert_command = dbp::tools::srv::replace_env(command);
		auto pid = dbp::tools::srv::sub_process(convert_command.c_str(), input_stream, output_stream);
		if (pid < 0)
			return false;
	}
	catch(...)
	{
		std::cerr << "loadInOut fail" << std::endl;
		return false;
	}
	return true;
}

inline static bool loadUsers(json& _json)
{
	user* pUser = nullptr;
	try
	{
		auto& users = _json["USERS"];
		auto cfg = broadcastQueue.get_configure();
		for (std::size_t i = 0; i < users.size(); ++i)
		{
			pUser = nullptr;
			auto& u = users[i];
			if (u.end() != u.find("DISABLE"))
			{
				bool disable = u["DISABLE"].get<bool>();
				if (disable)
				{
					continue;
				}
			}
			auto id = u["ID"].get<unsigned long long>();
			if (userMap.end() != userMap.find(id))
			{
				return false;
			}
			auto user_name = u["USER"].get<std::string>();
			auto password = u["PASS"].get<std::string>();
			unsigned long long buy_power = std::numeric_limits<unsigned long long>::max();
			if (u.end() != u.find("BUY_POWER"))
			{
				buy_power = u["BUY_POWER"].get<unsigned long long>();
			}
			std::string ip = "";
			unsigned short int port = 0;
			if (u.end() != u.find("IP"))
			{
				ip = u["IP"].get<std::string>();
				port = u["PORT"].get<unsigned short int>();
			}
			if (ip == "")
			{
				pUser = new user(cfg, id, user_name, password, buy_power);
			}
			else
			{
				pUser = new user(cfg, id, ip, port, user_name, password, buy_power);
			}
			auto& algos = u["ALGOS"];
			for (auto it = algos.begin(); it != algos.end(); ++it)
			{
				auto name = it.key();
				auto& algo = it.value();
				if (algo.end() != algo.find("DISABLE"))
				{
					bool disable = algo["DISABLE"].get<bool>();
					if (disable)
					{
						continue;
					}
				}
				auto lib = algo["lib"].get<std::string>();
				auto& params = algo["params"];
				if (!pUser->add_algo(name, lib, params))
				{
					delete pUser;
					return false;
				}
			}
			userMap[id] = pUser;
		}
		cfg.commit();
	}
	catch(...)
	{
		std::cerr << "loadUser fail" << std::endl;
		if (pUser)
			delete pUser;
		return false;
	}
	return true;
}

inline static bool loadDefinition(json& _json)
{
	json cache;
	auto itCache = _json.find("CatchPath");
	std::string strCatchPath = "";
	if (itCache != _json.end())
	{
		if (itCache->type() != json::value_t::string)
		{
			strCatchPath = itCache->get<std::string>();
		}
	}
	if (strCatchPath == "")
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
	std::memset(szCacheFileName, 0, 64);
	std::memset(&tmResult, 0, sizeof(struct tm));
	time_t iNowTime = time(0);
	localtime_r(&iNowTime, &tmResult);
	snprintf(szCacheFileName, 512, "%s/smfh_def_catch_%04d%02d%02d.json", strCatchPath.c_str(),
			tmResult.tm_year + 1900,
			tmResult.tm_mon + 1,
			tmResult.tm_mday);
	std::ifstream ifs(szCacheFileName);
	bool bReload = true;
	if (ifs)
	{
		std::string strDefJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
		try
		{
			cache = json::parse(strDefJson);
			const auto& omdc = cache["omdc"].get<json::array_t>();
			const auto& omdd = cache["omdd"].get<json::array_t>();
			const auto& warrent = cache["warrent_map"].get<json::object_t>();
			const auto& omdd_name = cache["omdd_name"].get<json::object_t>();
			for (std::size_t i = 0; i < omdc.size(); ++i)
			{
				const auto& code = omdc[i].get<unsigned int>();
				omdcMap[code].m_Code = code;
			}
			for (std::size_t i = 0; i < omdd.size(); ++i)
			{
				const auto& code = omdd[i].get<unsigned int>();
				omddMap[code].m_Code = code;
			}
			for (auto it = warrent.begin(); it != warrent.end(); ++it)
			{
				const auto& key = it->first;
				const auto& underlying = it->second.get<unsigned int>();
				unsigned int warrent = static_cast<unsigned int>(std::stoul(key));
				warrantToUnderlying[warrent] = underlying;
				underlyingToWarrant[underlying] = warrent;
			}
			for (auto it = omdd_name.begin(); it != omdd_name.end(); ++it)
			{
				const auto& key = it->first;
				const auto& name = it->second.get<std::string>();
				unsigned int code = static_cast<unsigned int>(std::stoul(key));
				codeToName[code] = name;
				nameToCode[name] = code;
			}
			bReload = false;
		}
		catch(...)
		{
			cache = json();
		}
	}
	else
	{
          flush_printf("tm:%llu, No Cache File \n", dbp::tools::srv::current());
	}
	if(bReload)
	{
		cache["omdc"] = json::array();
		cache["omdd"] = json::array();
		cache["warrent_map"] = json();
		cache["omdd_name"] = json();
		auto& Definition = _json["Definition"];
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
			try
			{
				flush_printf("tm:%llu, Load Omdc Definition Config \n", dbp::tools::srv::current());
				auto& Omdc = Definition["OMDC"];
				if (0 == Omdc.size())
				{
					std::cerr << "OMDC node size is 0" << std::endl;
					::close(iDefEopll);
					return false;
				}
				for (std::size_t i = 0; i < Omdc.size(); ++i)
				{
					auto& UdpNode = Omdc[i];
					std::string strInterfaceIp = UdpNode["InterfaceIp"].get<std::string>();
					std::string strMulticastIp = UdpNode["MulticastIp"].get<std::string>();
					unsigned short int uMulticastPort = UdpNode["MulticastPort"].get<unsigned short int>();
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
					std::memset(&objEvent, 0, sizeof(struct epoll_event));
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
			catch(...)
			{
				std::cerr << "OMDC load Json fail" << std::endl;
				::close(iDefEopll);
				return false;
			}
		}
		CDefMap omddStatus;
		itActivate = mActivateChannel.find("OmddChannel");
		if(itActivate != mActivateChannel.end())
		{
			try
			{
				flush_printf("tm:%llu, Load Omdd Definition Config \n", dbp::tools::srv::current());
				auto& Omdd = Definition["OMDD"];
				if (0 == Omdd.size())
				{
					std::cerr << "OMDD node size is 0" << std::endl;
					::close(iDefEopll);
					for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
					{
						::close(it->first);
					}
					return false;
				}
				for (size_t i = 0; i < Omdd.size(); ++i)
				{
					auto& UdpNode = Omdd[i];
					std::string strInterfaceIp = UdpNode["InterfaceIp"].get<std::string>();
					std::string strMulticastIp = UdpNode["MulticastIp"].get<std::string>();
					unsigned short int uMulticastPort = UdpNode["MulticastPort"].get<unsigned short int>();
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
					std::memset(&objEvent, 0, sizeof(struct epoll_event));
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
			catch(...)
			{
				std::cerr << "OMDD load Json fail" << std::endl;
				::close(iDefEopll);
				for (CDefMap::iterator it = omdcStatus.begin(); it != omdcStatus.end(); ++it)
				{
					::close(it->first);
				}
				return false;
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
													omdcMap[uSecurityCode].m_Code = uSecurityCode;
													cache["omdc"].push_back(uSecurityCode);
													auto instrument_Type = OMD_GET_STR(pszBuffer, 24, 4);
													if (instrument_Type == "WRNT")
													{
														auto warrant_code = OMD_GET_VALUE(pszBuffer, 4, unsigned int);
														auto underlying_code = OMD_GET_VALUE(pszBuffer, 464, unsigned int);
														warrantToUnderlying[warrant_code] = underlying_code;
														underlyingToWarrant[underlying_code] = warrant_code;
														cache["warrent_map"][std::to_string(warrant_code)] = underlying_code;
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
													unsigned long orderbookid = OMD_GET_VALUE(pszBuffer, 4, unsigned int);
													auto idx = symbol.find(' ');
													if (std::string::npos != idx)
													{
														symbol = symbol.substr(0, idx);
													}
													cache["omdd_name"][std::to_string(orderbookid)] = symbol;
													omddMap[orderbookid].m_Code = orderbookid;
													cache["omdd"].push_back(orderbookid);
													codeToName[orderbookid] = symbol;
													nameToCode[symbol] = orderbookid;
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
		std::ofstream ofs(szCacheFileName);
		ofs << cache.dump() << std::endl;
		ofs.close();
	}
	return true;
}
inline static bool loadRetran(json& _json)
{
	try
	{
		auto& RetranProxy = _json["RetranProxy"];
		for (std::size_t i = 0; i < RetranProxy.size(); ++i)
		{
			auto& Node = RetranProxy[i];
			CRetranProxy proxy;
			proxy.m_strIp = Node["Ip"].get<std::string>();
			proxy.m_uPort = Node["Port"].get<unsigned short int>();
			retranVec.push_back(proxy);
		}
	}
	catch (...)
	{
		std::cerr << "load loadRetran fail" << std::endl;
		return false;
	}
	return true;
}
inline static bool loadChannel(json& _json, const char* pszName, CStreamVec& vec)
{
	try
	{
	auto& Channel = _json[pszName];
	for (std::size_t i = 0; i < Channel.size(); ++i)
	{
		CStreamChannel channel;
		auto& ChannelNode = Channel[i];
		channel.m_uChannelId = ChannelNode["ChannelId"].get<unsigned short int>();
		auto& Hot = ChannelNode["Hot"];
		std::string InterfaceIp = Hot["InterfaceIp"].get<std::string>();
		std::string MulticastIp = Hot["MulticastIp"].get<std::string>();
		unsigned short int MulticastPort = Hot["MulticastPort"].get<unsigned short int>();
		int iHot = dbp::net::srv::getNoBlockReuseUdpListener(MulticastPort, MulticastIp, InterfaceIp);
		if (-1 == iHot)
		{
			return false;
		}
		auto& Refresh = ChannelNode["Refresh"];
		InterfaceIp = Refresh["InterfaceIp"].get<std::string>();
		MulticastIp = Refresh["MulticastIp"].get<std::string>();
		MulticastPort = Refresh["MulticastPort"].get<unsigned short int>();
		int iRefresh = dbp::net::srv::getNoBlockReuseUdpListener(MulticastPort, MulticastIp, InterfaceIp);
		if (-1 == iRefresh)
		{
			close(iHot);
			return false;
		}
		channel.m_uRetranProxyIdx = ChannelNode["RetranProxyIndex"].get<std::size_t>();
		channel.m_iHot = iHot;
		channel.m_iRefresh = iRefresh;
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
		channel.m_uChannelIdx = i;
		vec.push_back(channel);
	}
	}
	catch(...)
	{
		std::cerr << "load loadChannel: " << pszName << " fail" << std::endl;
		return false;
	}
	return true;
}
inline static bool loadActivateChannel(json& _json)
{
	try
	{
		auto& ActivateChannel = _json["ActivateChannel"];
		for (std::size_t i = 0; i < ActivateChannel.size(); ++i)
		{
			auto& Node = ActivateChannel[i];
			std::string strChannel = Node["Channel"].get<std::string>();
			mActivateChannel[strChannel] = "";
			flush_printf("tm:%llu, Activate Channel = %s \n", dbp::tools::srv::current(), strChannel.c_str());
		}
	}
	catch(...)
	{
		std::cerr << "load loadActivateChannel fail" << std::endl;
		return false;
	}
	return true;
}

#endif


