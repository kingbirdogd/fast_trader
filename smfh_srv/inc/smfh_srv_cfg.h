#ifndef __SMFH_SRV_CFG__
#define __SMFH_SRV_CFG__
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdio>
#include <cstdlib>
#include <epoll.h>
#ifdef TCPDIRECT
#include "zf_utils.h"
#include <zf/zf.h>
#endif
#include <pthread.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <json.hpp>
#include <tools.h>
#include <net.h>
#include <global_memory.hpp>
#include <algo_loader.hpp>
#include <limits>
#include <dbp_tcp_md.hpp>

using json = nlohmann::json;

inline void loadThreadPriority(json& _json)
{
	auto it = _json.find("SCHED_TYPE");
	if (_json.end() != it)
	{
		if (it.value() == json::value_t::string)
		{
			auto str_SCHED_TYPE = it.value().get<std::string>();
			if (str_SCHED_TYPE == "SCHED_OTHER")
			{
				SCHED_TYPE = SCHED_OTHER;
			}
			else if (str_SCHED_TYPE == "SCHED_FIFO")
			{
				SCHED_TYPE = SCHED_FIFO;
			}
			else if (str_SCHED_TYPE == "SCHED_RR")
			{
				SCHED_TYPE = SCHED_RR;
			}
		}
	}
	it = _json.find("SCHED_PRIORITY");
	if (_json.end() != it)
	{
		if (it.value() == json::value_t::number_unsigned)
		{
			SCHED_PRIORITY = it.value().get<int>();
		}
	}
}

inline static void loadIvFile(json& _json)
{
	try
	{
		auto it = _json.find("IVFile");
		if (_json.end() != it)
		{
			std::string path = it.value().get<std::string>();
			flush_printf("tm:%llu, loadIvFile %s \n", dbp::tools::srv::current(), path.c_str());
			ivLoader.load(path.c_str());
		}
	}
	catch(...)
	{
		std::cerr << "loadIVFile fail" << std::endl;

	}
}

inline static void loadIvFilePut(json& _json)
{
	try
	{
		auto it = _json.find("IVFilePut");
		if (_json.end() != it)
		{
			std::string path = it.value().get<std::string>();
			flush_printf("tm:%llu, loadIvFilePut %s \n", dbp::tools::srv::current(), path.c_str());
			ivLoaderPut.load(path.c_str());
		}
	}
	catch(...)
	{
		std::cerr << "IVFilePut fail" << std::endl;

	}
}

inline static void loadOrderbookFile(json& _json)
{
	try
	{
		auto it = _json.find("OrderbookFile");
		if (_json.end() != it)
		{

			auto& OrderbookFile_Node = _json["OrderbookFile"];

			std::string path = OrderbookFile_Node["OrderbookPath"].get<std::string>();
			flush_printf("tm:%llu, OrderbookFile %s \n", dbp::tools::srv::current(), path.c_str());
			orderbookLoader.load(path.c_str());

			std::string str_thick = OrderbookFile_Node["algothick"].get<std::string>();
			std::string str_thin = OrderbookFile_Node["algothin"].get<std::string>();
			std::string str_sell = OrderbookFile_Node["algobidtriggersell"].get<std::string>();
			std::string str_buy = OrderbookFile_Node["algoasktriggerbuy"].get<std::string>();
			std::string str_lost = OrderbookFile_Node["algoraisestoplost"].get<std::string>();
			std::string str_bidbuy = OrderbookFile_Node["algoreadybidbuy"].get<std::string>();
			std::string str_range = OrderbookFile_Node["algotickrange"].get<std::string>();

			algoParam.Thick = stod(str_thick.c_str());
			algoParam.Thin = stod(str_thin.c_str());
			algoParam.BidTriggerSell = stod(str_sell.c_str());
			algoParam.AskTriggerBuy = stod(str_buy.c_str());
			algoParam.RaiseStopLost = stod(str_lost.c_str());
			algoParam.ReadyBidBuy = stod(str_bidbuy.c_str());
			algoParam.ReadyAskBuy = stod(str_bidbuy.c_str());
			algoParam.TickRange = stod(str_range.c_str());
		}
	}
	catch(...)
	{
		std::cerr << "OrderbookFile fail" << std::endl;

	}
}

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

inline static void loadLog(json& _json)
{
	try
	{
		auto log_path = _json["LOG_PATH"].get<std::string>();
		auto convert_log_path = dbp::tools::srv::replace_env(log_path);
		dbp::tools::srv::get_YYYYMMDDHHMMSSsss();
		convert_log_path += "/smfh_";
		convert_log_path += std::to_string(dbp::tools::srv::get_YYYYMMDDHHMMSSsss() / 1000000000);
		convert_log_path += ".log";
		FILE* file = nullptr;
		file = std::fopen(convert_log_path.c_str(), "a");
		if (nullptr != file)
		{
			log_stream = file;
		}
	}
	catch(...)
	{
		std::cout << "do not have LOG_PATH, all log will send to stdout" << std::endl;
	}
	return;
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
				if (!algo_loader::add_algo_to_user(*pUser, name, lib, params))
				{
					delete pUser;
					return false;
				}
			}
			userMap[id] = pUser;
		}
		cfg.commit();
		cfg.warm_up();
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
			const auto& omdd_underlying = cache["omdd_underlying"].get<json::object_t>();
			const auto& omdc_addition_definition = cache["omdc_addition_definition"].get<json::object_t>();
			for (std::size_t i = 0; i < omdc.size(); ++i)
			{
				const auto& code = omdc[i].get<unsigned int>();
				omdcMap[code].m_Code = code;
#ifdef FULLTICK
				omdcFullTickBook[code];
#endif //FULLTICK



				if(ivLoader.exist(code)){
					omdcSlimMap[code].m_Code = code;
				}
				if(ivLoaderPut.exist(code)){
					omdcSlimMap[code].m_Code = code;
				}

			}
			for (std::size_t i = 0; i < omdd.size(); ++i)
			{
				const auto& code = omdd[i].get<unsigned int>();
				omddMap[code].m_Code = code;
#ifdef FULLTICK
				omddFullTickBook[code];
#endif //FULLTICK

			}




			for (auto it = warrent.begin(); it != warrent.end(); ++it)
			{
				const auto& key = it->first;
				const auto& underlying = it->second.get<unsigned int>();
				unsigned int warrent = static_cast<unsigned int>(std::stoul(key));
				warrantToUnderlying[warrent] = underlying;

				unsigned int omdd_code = 0;
				if(underlying > 0){
					underlyingToWarrant[underlying].insert(warrent);
				}else{
					auto itdef = omdc_addition_definition.find(key);
					if(itdef != omdc_addition_definition.end()){
						std::string sname = itdef->second["SecuritySortName"].get<std::string>();

						string::size_type pos=0;
						pos = sname.find("HSI");
						if(pos != sname.npos){
							underlyingToWarrant[100001].insert(warrent);
							warrantToUnderlying[warrent] = 100001;
							omdd_code = 100001;
						}
						pos = sname.find("HSCEI");
						if(pos != sname.npos){
							underlyingToWarrant[100002].insert(warrent);
							warrantToUnderlying[warrent] = 100002;
							omdd_code = 100002;
						}
						pos = sname.find("HSTEC");
						if(pos != sname.npos){
							underlyingToWarrant[100003].insert(warrent);
							warrantToUnderlying[warrent] = 100003;
							omdd_code = 100003;
						}
					}
				}

				stockWarrantomdcMap[warrent].m_Code = warrent;
				flush_printf("tm:%llu, stockWarrantomdcMap Wcode = %d \n", dbp::tools::srv::current(), warrent);
				if(underlying > 0){
					stockWarrantomdcMap[underlying].m_Code = underlying;
					flush_printf("tm:%llu, stockWarrantomdcMap UCode = %d \n", dbp::tools::srv::current(), underlying);
				}

//New
				ptomdcMap[warrent].m_Code = warrent;
				if(underlying > 0){
					ptomdcMap[underlying].m_Code = underlying;
					s1omdcMap[underlying].m_Code = underlying;
				}

				pricedataMap[warrent] = new pricedata();
				pricedataMap[warrent]->isWarrant = true;
				pricedataMap[warrent]->isUnderlying = false;
				pricedataMap[warrent]->UCode = underlying;

				pricedataMap[warrent]->IsIndex = false;
				pricedataMap[warrent]->Orderbook_id = 0;

				if(underlying == 0){
					pricedataMap[warrent]->UCode = omdd_code;
					pricedataMap[warrent]->IsIndex = true;
				}

				pricedataMap[warrent]->Bestbid=0ull;
				pricedataMap[warrent]->Bestask=0ull;
				pricedataMap[warrent]->BestBidQty=0ull;
				pricedataMap[warrent]->BestAskQty=0ull;
				pricedataMap[warrent]->PBestbid=0ull;
				pricedataMap[warrent]->PBestask=0ull;
				pricedataMap[warrent]->LBestbid=0ull;
				pricedataMap[warrent]->LBestask=0ull;
				pricedataMap[warrent]->BidIssuerSize=0ull;
				pricedataMap[warrent]->AskIssuerSize=0ull;
				pricedataMap[warrent]->LastBidSeq=0;
				pricedataMap[warrent]->LastAskSeq=0;
				pricedataMap[warrent]->BidSeq=1;
				pricedataMap[warrent]->AskSeq=1;
				pricedataMap[warrent]->BSeq=1;
				pricedataMap[warrent]->ASeq=1;
				pricedataMap[warrent]->BidVolSeq=1;
				pricedataMap[warrent]->AskVolSeq=1;

				if(underlying > 0){
					auto itp = pricedataMap.find(underlying);
					if(itp == pricedataMap.end()){
						pricedataMap[underlying] = new pricedata();
						pricedataMap[underlying]->isUnderlying = true;
						pricedataMap[underlying]->isWarrant = false;
						pricedataMap[underlying]->UCode = 0;
						pricedataMap[underlying]->IsIndex = false;
						pricedataMap[underlying]->Orderbook_id = 0;
						pricedataMap[underlying]->Bestbid=0ull;
						pricedataMap[underlying]->Bestask=0ull;
						pricedataMap[underlying]->BestBidQty=0ull;
						pricedataMap[underlying]->BestAskQty=0ull;
						pricedataMap[underlying]->PBestbid=0ull;
						pricedataMap[underlying]->PBestask=0ull;
						pricedataMap[underlying]->LBestbid=0ull;
						pricedataMap[underlying]->LBestask=0ull;
						pricedataMap[underlying]->BidIssuerSize=0ull;
						pricedataMap[underlying]->AskIssuerSize=0ull;
						pricedataMap[underlying]->BidSeq=1;
						pricedataMap[underlying]->AskSeq=1;
						pricedataMap[underlying]->BSeq=1;
						pricedataMap[underlying]->ASeq=1;
						pricedataMap[underlying]->BidVolSeq=1;
						pricedataMap[underlying]->AskVolSeq=1;

					}
				}

			}
			for (auto it = omdd_name.begin(); it != omdd_name.end(); ++it)
			{
				const auto& key = it->first;
				const auto& name = it->second.get<std::string>();
				unsigned int code = static_cast<unsigned int>(std::stoul(key));
				codeToName[code] = name;
				nameToCode[name] = code;
			}
			for (auto it = omdd_underlying.begin(); it != omdd_underlying.end(); ++it)
			{
				COmddUnderlying underlying;
				const auto& key = it->first;
				underlying.CommodityCode = it->second["CommodityCode"].get<unsigned short int>();
				underlying.InstrumentGroup = it->second["InstrumentGroup"].get<unsigned char>();
				underlying.Symbol = it->second["Symbol"].get<std::string>();
				unsigned int code = static_cast<unsigned int>(std::stoul(key));
				codeTounderlying[code] = underlying;

				if(underlying.CommodityCode == 4001){
					hsiVec.push_back(underlying.Symbol);
				}
				if(underlying.CommodityCode == 4002){
					hsceiVec.push_back(underlying.Symbol);
				}
				if(underlying.CommodityCode == 4038){
					hstecVec.push_back(underlying.Symbol);
				}


				OmddDefReady = true;
			}
			for (auto it = omdc_addition_definition.begin(); it != omdc_addition_definition.end(); ++it)
			{
				COmdcAdditionDefinitions defs;
				const auto& key = it->first;
				defs.SpreadTableCode = it->second["SpreadTableCode"].get<std::string>();
				defs.SecuritySortName = it->second["SecuritySortName"].get<std::string>();
				defs.CallPutFlag = it->second["CallPutFlag"].get<std::string>();
				defs.LotSize = it->second["LotSize"].get<unsigned int>();
				defs.ProductType = it->second["ProductType"].get<unsigned char>();
				unsigned int code = static_cast<unsigned int>(std::stoul(key));
				omdcAdditionDefinitionsMap[code] = defs;


				NormalDistribution* nd = orderbookLoader.getNormDistribution(code);
				if(nd != nullptr){
					s1SignalMap[code] = new s1signal();
					s1SignalPutMap[code] = new s1signal();

					unsigned int nooflot_RaiseStopLost = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.RaiseStopLost))/defs.LotSize) + 1;
					unsigned int nooflot_ReadyBidBuy = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.ReadyBidBuy))/defs.LotSize) + 1;
					unsigned int nooflot_AskTriggerBuy = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.AskTriggerBuy))/defs.LotSize) + 1;
					unsigned int nooflot_BidTriggerSell = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.BidTriggerSell))/defs.LotSize) + 1;
					unsigned int nooflot_Thick = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.Thick))/defs.LotSize) + 1;
					unsigned int nooflot_Thin = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.Thin))/defs.LotSize) + 1;

					s1SignalMap[code]->RaiseStopLost = nooflot_RaiseStopLost*defs.LotSize;
					s1SignalMap[code]->ReadyBidBuy = nooflot_ReadyBidBuy*defs.LotSize;
					s1SignalMap[code]->AskTriggerBuy = nooflot_AskTriggerBuy*defs.LotSize;
					s1SignalMap[code]->BidTriggerSell = nooflot_BidTriggerSell*defs.LotSize;
					s1SignalMap[code]->Thick = nooflot_Thick*defs.LotSize;
					s1SignalMap[code]->Thin = nooflot_Thin*defs.LotSize;

					s1SignalPutMap[code]->RaiseStopLost = nooflot_RaiseStopLost*defs.LotSize;
					s1SignalPutMap[code]->ReadyAskBuy = nooflot_ReadyBidBuy*defs.LotSize;
					s1SignalPutMap[code]->AskTriggerBuy = nooflot_AskTriggerBuy*defs.LotSize;
					s1SignalPutMap[code]->BidTriggerSell = nooflot_BidTriggerSell*defs.LotSize;
					s1SignalPutMap[code]->Thick = nooflot_Thick*defs.LotSize;
					s1SignalPutMap[code]->Thin = nooflot_Thin*defs.LotSize;


					//flush_printf("tm:%llu, fm cache s1SignalMap = %u  THICK = %llu \n\n", dbp::tools::srv::current(), code, s1SignalMap[code]->Thick);
				}



				int wtype = 0;
				if(defs.CallPutFlag == "C")
				{
					wtype = 1;
				}
				if(defs.CallPutFlag == "P")
				{
					wtype = 2;
				}
				if(wtype > 0){
					pricemarkMap[code] = new PriceMark(code, wtype);
				}













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
		cache["omdc_addition_definition"] = json::object();
		cache["warrent_map"] = json();
		cache["omdd_name"] = json();
		auto& Definition = _json["Definition"];
		int iDefEopll = epoll_create(4);
		if (-1 == iDefEopll)
		{
			std::cerr << "create epoll fail" << std::endl;
			return false;
		}

		bool omdcdefloaded = false;

		CDefMap omdcStatus;
		auto itActivate = mActivateChannel.find("OmdcChannel");
		if(itActivate != mActivateChannel.end())
		{
			omdcdefloaded = true;
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

		itActivate = mActivateChannel.find("SlimOmdcChannel");
		if(itActivate != mActivateChannel.end())
		{
			omdcdefloaded = true;
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
		itActivate = mActivateChannel.find("SWOmdcChannel");
		if(itActivate != mActivateChannel.end())
		{
			omdcdefloaded = true;
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

		itActivate = mActivateChannel.find("SWOmdcChannel5");
		if(itActivate != mActivateChannel.end())
		{
			omdcdefloaded = true;
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

		if(!omdcdefloaded){
			itActivate = mActivateChannel.find("PriceTableChannel");
			if(itActivate != mActivateChannel.end())
			{
				omdcdefloaded = true;
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

/*
		itActivate = mActivateChannel.find("OmddPriceTableChannel");
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
*/


		itActivate = mActivateChannel.find("OmddftChannel");
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
													auto& omdcAdditionDefinition = omdcAdditionDefinitionsMap[uSecurityCode];
													omdcAdditionDefinition.SpreadTableCode = OMD_GET_STR(pszBuffer, 30, 2);
													omdcAdditionDefinition.SecuritySortName = OMD_GET_STR(pszBuffer, 32, 40);
													omdcAdditionDefinition.CallPutFlag = OMD_GET_STR(pszBuffer, 460, 1);
													omdcAdditionDefinition.ProductType = OMD_GET_VALUE(pszBuffer, 28, unsigned char);
													omdcAdditionDefinition.LotSize = OMD_GET_VALUE(pszBuffer, 195, unsigned int);
													omdcMap[uSecurityCode].m_Code = uSecurityCode;
#ifdef FULLTICK
													omdcFullTickBook[uSecurityCode];
#endif //FULLTICK

													if(ivLoader.exist(uSecurityCode)){
														omdcSlimMap[uSecurityCode].m_Code = uSecurityCode;
													}

													if(ivLoaderPut.exist(uSecurityCode)){
														omdcSlimMap[uSecurityCode].m_Code = uSecurityCode;
													}



													cache["omdc"].push_back(uSecurityCode);
													auto strSecurityCode = std::to_string(uSecurityCode);
													cache["omdc_addition_definition"][strSecurityCode] = json::object();
													cache["omdc_addition_definition"][strSecurityCode]["SpreadTableCode"] = omdcAdditionDefinition.SpreadTableCode;
													cache["omdc_addition_definition"][strSecurityCode]["SecuritySortName"] = omdcAdditionDefinition.SecuritySortName;
													cache["omdc_addition_definition"][strSecurityCode]["CallPutFlag"] = omdcAdditionDefinition.CallPutFlag;
													cache["omdc_addition_definition"][strSecurityCode]["ProductType"] = omdcAdditionDefinition.ProductType;
													cache["omdc_addition_definition"][strSecurityCode]["LotSize"] = omdcAdditionDefinition.LotSize;

													NormalDistribution* nd = orderbookLoader.getNormDistribution(uSecurityCode);
													if(nd != nullptr){
														s1SignalMap[uSecurityCode] = new s1signal();
														//s1SignalMap[uSecurityCode].hasSignal = false;

														unsigned int nooflot_RaiseStopLost = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.RaiseStopLost))/omdcAdditionDefinition.LotSize) + 1;
														unsigned int nooflot_ReadyBidBuy = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.ReadyBidBuy))/omdcAdditionDefinition.LotSize) + 1;
														unsigned int nooflot_AskTriggerBuy = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.AskTriggerBuy))/omdcAdditionDefinition.LotSize) + 1;
														unsigned int nooflot_BidTriggerSell = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.BidTriggerSell))/omdcAdditionDefinition.LotSize) + 1;
														unsigned int nooflot_Thick = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.Thick))/omdcAdditionDefinition.LotSize) + 1;
														unsigned int nooflot_Thin = (unsigned int)(std::pow(10, nd->inverseCumulativeProbability(algoParam.Thin))/omdcAdditionDefinition.LotSize) + 1;

														s1SignalMap[uSecurityCode]->RaiseStopLost = nooflot_RaiseStopLost*omdcAdditionDefinition.LotSize;
														s1SignalMap[uSecurityCode]->ReadyBidBuy = nooflot_ReadyBidBuy*omdcAdditionDefinition.LotSize;
														s1SignalMap[uSecurityCode]->AskTriggerBuy = nooflot_AskTriggerBuy*omdcAdditionDefinition.LotSize;
														s1SignalMap[uSecurityCode]->BidTriggerSell = nooflot_BidTriggerSell*omdcAdditionDefinition.LotSize;
														s1SignalMap[uSecurityCode]->Thick = nooflot_Thick*omdcAdditionDefinition.LotSize;
														s1SignalMap[uSecurityCode]->Thin = nooflot_Thin*omdcAdditionDefinition.LotSize;

														s1SignalPutMap[uSecurityCode] = new s1signal();

														s1SignalPutMap[uSecurityCode]->RaiseStopLost = nooflot_RaiseStopLost*omdcAdditionDefinition.LotSize;
														s1SignalPutMap[uSecurityCode]->ReadyAskBuy = nooflot_ReadyBidBuy*omdcAdditionDefinition.LotSize;
														s1SignalPutMap[uSecurityCode]->AskTriggerBuy = nooflot_AskTriggerBuy*omdcAdditionDefinition.LotSize;
														s1SignalPutMap[uSecurityCode]->BidTriggerSell = nooflot_BidTriggerSell*omdcAdditionDefinition.LotSize;
														s1SignalPutMap[uSecurityCode]->Thick = nooflot_Thick*omdcAdditionDefinition.LotSize;
														s1SignalPutMap[uSecurityCode]->Thin = nooflot_Thin*omdcAdditionDefinition.LotSize;


														//flush_printf("tm:%llu, s1SignalMap = %u  THICK = %llu \n\n", dbp::tools::srv::current(), uSecurityCode, s1SignalMap[uSecurityCode]->Thick);

													}



													auto instrument_Type = OMD_GET_STR(pszBuffer, 24, 4);
													if (instrument_Type == "WRNT")
													{
														auto warrant_code = OMD_GET_VALUE(pszBuffer, 4, unsigned int);
														//auto underlying_code = OMD_GET_VALUE(pszBuffer, 464, unsigned int); OMD-C V1.3

														auto underlying_code = OMD_GET_VALUE(pszBuffer, 544, unsigned int);
														warrantToUnderlying[warrant_code] = underlying_code;
														underlyingToWarrant[underlying_code].insert(warrant_code);


														unsigned int omdd_code = 0;

														if(underlying_code > 0){

															cache["warrent_map"][std::to_string(warrant_code)] = underlying_code;
														}else{
															std::string sname = omdcAdditionDefinition.SecuritySortName;

															string::size_type pos=0;
															pos = sname.find("HSI");
															if(pos != sname.npos){
																warrantToUnderlying[warrant_code] = 100001;
																underlyingToWarrant[100001].insert(warrant_code);
																cache["warrent_map"][std::to_string(warrant_code)] = 100001;

																omdd_code = 100001;
															}
															pos = sname.find("HSCEI");
															if(pos != sname.npos){
																warrantToUnderlying[warrant_code] = 100002;
																underlyingToWarrant[100002].insert(warrant_code);
																cache["warrent_map"][std::to_string(warrant_code)] = 100002;

																omdd_code = 100002;
															}
															pos = sname.find("HSTEC");
															if(pos != sname.npos){
																warrantToUnderlying[warrant_code] = 100003;
																underlyingToWarrant[100003].insert(warrant_code);
																cache["warrent_map"][std::to_string(warrant_code)] = 100003;

																omdd_code = 100003;
															}


														}

														stockWarrantomdcMap[warrant_code].m_Code = warrant_code;
														flush_printf("tm:%llu, stockWarrantomdcMap Wcode = %d \n", dbp::tools::srv::current(), warrant_code);
														flush_printf("tm:%llu, stockWarrantomdcMap Omdd Code = %d \n", dbp::tools::srv::current(), omdd_code);
														if(underlying_code > 0){

															stockWarrantomdcMap[underlying_code].m_Code = underlying_code;
															flush_printf("tm:%llu, stockWarrantomdcMap Ucode = %d \n", dbp::tools::srv::current(), underlying_code);
														}

														int wtype = 0;
														if(omdcAdditionDefinition.CallPutFlag == "C")
														{
															wtype = 1;
														}
														if(omdcAdditionDefinition.CallPutFlag == "P")
														{
															wtype = 2;
														}



														pricemarkMap[uSecurityCode] = new PriceMark(uSecurityCode, wtype);
														ptomdcMap[uSecurityCode].m_Code = uSecurityCode;
														if(underlying_code > 0){
															ptomdcMap[underlying_code].m_Code = underlying_code;
														}

														if(underlying_code > 0){
															s1omdcMap[underlying_code].m_Code = underlying_code;

														}
														pricedataMap[uSecurityCode] = new pricedata();
														pricedataMap[uSecurityCode]->isWarrant = true;
														pricedataMap[uSecurityCode]->isUnderlying = false;
														pricedataMap[uSecurityCode]->UCode = underlying_code;
														pricedataMap[uSecurityCode]->IsIndex = false;
														pricedataMap[uSecurityCode]->Orderbook_id = 0;
														if(underlying_code == 0){
															pricedataMap[uSecurityCode]->UCode = omdd_code;
															pricedataMap[uSecurityCode]->IsIndex = true;
														}
														pricedataMap[uSecurityCode]->Bestbid=0ull;
														pricedataMap[uSecurityCode]->Bestask=0ull;
														pricedataMap[uSecurityCode]->BestBidQty=0ull;
														pricedataMap[uSecurityCode]->BestAskQty=0ull;
														pricedataMap[uSecurityCode]->PBestbid=0ull;
														pricedataMap[uSecurityCode]->PBestask=0ull;
														pricedataMap[uSecurityCode]->LBestbid=0ull;
														pricedataMap[uSecurityCode]->LBestask=0ull;
														pricedataMap[uSecurityCode]->BidIssuerSize=0ull;
														pricedataMap[uSecurityCode]->AskIssuerSize=0ull;
														pricedataMap[uSecurityCode]->LastBidSeq=0;
														pricedataMap[uSecurityCode]->LastAskSeq=0;
														pricedataMap[uSecurityCode]->BidSeq=1;
														pricedataMap[uSecurityCode]->AskSeq=1;
														pricedataMap[uSecurityCode]->BSeq=1;
														pricedataMap[uSecurityCode]->ASeq=1;
														pricedataMap[uSecurityCode]->BidVolSeq=1;
														pricedataMap[uSecurityCode]->AskVolSeq=1;

														if(underlying_code > 0){
															auto itp = pricedataMap.find(underlying_code);
															if(itp == pricedataMap.end()){
																pricedataMap[underlying_code] = new pricedata();
																pricedataMap[underlying_code]->isUnderlying = true;
																pricedataMap[underlying_code]->isWarrant = false;
																pricedataMap[underlying_code]->IsIndex = false;
																pricedataMap[underlying_code]->Orderbook_id = 0;
																pricedataMap[underlying_code]->UCode = 0;
																pricedataMap[underlying_code]->Bestbid=0ull;
																pricedataMap[underlying_code]->Bestask=0ull;
																pricedataMap[underlying_code]->BestBidQty=0ull;
																pricedataMap[underlying_code]->BestAskQty=0ull;
																pricedataMap[underlying_code]->PBestbid=0ull;
																pricedataMap[underlying_code]->PBestask=0ull;
																pricedataMap[underlying_code]->LBestbid=0ull;
																pricedataMap[underlying_code]->LBestask=0ull;
																pricedataMap[underlying_code]->BidIssuerSize=0ull;
																pricedataMap[underlying_code]->AskIssuerSize=0ull;
																pricedataMap[underlying_code]->BidSeq=1;
																pricedataMap[underlying_code]->AskSeq=1;
																pricedataMap[underlying_code]->BSeq=1;
																pricedataMap[underlying_code]->ASeq=1;
																pricedataMap[underlying_code]->BidVolSeq=1;
																pricedataMap[underlying_code]->AskVolSeq=1;



																//s1omdcMap[underlying_code].m_Code = underlying_code;
															}
														}

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
													OmddDefReady = true;
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

													unsigned short int CommodityCode = OMD_GET_VALUE(pszBuffer, 44, unsigned short int);
													unsigned char InstrumentGroup = OMD_GET_VALUE(pszBuffer, 42, unsigned char);
													auto expdate = OMD_GET_STR(pszBuffer, 80, 8);

													if(CommodityCode == 4001 || CommodityCode == 4002 || CommodityCode == 4038){
														if(InstrumentGroup == 4){

															std::string strToday = std::to_string(dbp::tools::srv::get_YYYYMMDD() / 1000000000);
															if(strToday != expdate){ //Exclude exp contract

														cache["omdd_name"][std::to_string(orderbookid)] = symbol;
														omddMap[orderbookid].m_Code = orderbookid;

														ptomddMap[orderbookid].m_Code = orderbookid;
														auto itp = pricedataMap.find(orderbookid);
														if(itp == pricedataMap.end()){
															pricedataMap[orderbookid] = new pricedata();
															pricedataMap[orderbookid]->isUnderlying = true;
															pricedataMap[orderbookid]->isWarrant = false;
															pricedataMap[orderbookid]->IsIndex = true;
															pricedataMap[orderbookid]->UCode = 0;
															pricedataMap[orderbookid]->Bestbid=0ull;
															pricedataMap[orderbookid]->Bestask=0ull;
															pricedataMap[orderbookid]->BestBidQty=0ull;
															pricedataMap[orderbookid]->BestAskQty=0ull;
															pricedataMap[orderbookid]->PBestbid=0ull;
															pricedataMap[orderbookid]->PBestask=0ull;
															pricedataMap[orderbookid]->LBestbid=0ull;
															pricedataMap[orderbookid]->LBestask=0ull;
															pricedataMap[orderbookid]->BidIssuerSize=0ull;
															pricedataMap[orderbookid]->AskIssuerSize=0ull;
															pricedataMap[orderbookid]->BidSeq=1;
															pricedataMap[orderbookid]->AskSeq=1;
															pricedataMap[orderbookid]->BSeq=1;
															pricedataMap[orderbookid]->ASeq=1;
															pricedataMap[orderbookid]->BidVolSeq=1;
															pricedataMap[orderbookid]->AskVolSeq=1;
														}



	#ifdef FULLTICK
														omddFullTickBook[orderbookid];
	#endif //FULLTICK
														cache["omdd"].push_back(orderbookid);
														codeToName[orderbookid] = symbol;
														nameToCode[symbol] = orderbookid;
														COmddUnderlying underlying;
														underlying.InstrumentGroup = OMD_GET_VALUE(pszBuffer, 42, unsigned char);
														underlying.CommodityCode = OMD_GET_VALUE(pszBuffer, 44, unsigned short int);
														codeTounderlying[orderbookid] = underlying;
														cache["omdd_underlying"][std::to_string(orderbookid)] = json::object();
														cache["omdd_underlying"][std::to_string(orderbookid)]["InstrumentGroup"] = static_cast<unsigned short int>(underlying.InstrumentGroup);
														cache["omdd_underlying"][std::to_string(orderbookid)]["CommodityCode"] = static_cast<unsigned short int>(underlying.CommodityCode);
														cache["omdd_underlying"][std::to_string(orderbookid)]["Symbol"] = static_cast<std::string>(underlying.Symbol);

														if(CommodityCode == 4001){
															hsiVec.push_back(symbol);
														}
														if(CommodityCode == 4002){
															hsceiVec.push_back(symbol);
														}
														if(CommodityCode == 4038){
															hstecVec.push_back(symbol);
														}


															flush_printf("tm:%llu, OMDD Underlying Set, code: %lu, InstrumentGroup:%u,CommodityCode:%u    Today=%s   ExpDate=%s\n",
																	dbp::tools::srv::current(),
																	orderbookid,
																	static_cast<unsigned int>(underlying.InstrumentGroup),
																	static_cast<unsigned int>(underlying.CommodityCode),strToday.c_str(), expdate.c_str() );
														}else{

															flush_printf("tm:%llu, OMDD Underlying Set, code: %lu, InstrumentGroup:%u,CommodityCode:%u   Expire Today\n",
																dbp::tools::srv::current(),
																orderbookid,
																static_cast<unsigned int>(InstrumentGroup),
																static_cast<unsigned int>(CommodityCode));
														}

														}
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



#ifdef TCPDIRECT
			std::string InterfaceName = Hot["InterfaceName"].get<std::string>();
			flush_printf("tm:%llu, loadChannel Config  TCPDIRECT 1\n", dbp::tools::srv::current());
			if (zf_attr_alloc(&channel.m_zfAttr))
			{
				return false;
			}
			struct zf_attr* attr =  channel.m_zfAttr;
			if (zf_attr_set_str(attr, "interface", InterfaceName.c_str()))
			{
				return false;
			}
			if (zf_stack_alloc(attr, &channel.m_zfStack))
			{
				zf_attr_free(attr);
				return false;
			}
			struct zf_stack* stack = channel.m_zfStack;
			flush_printf("tm:%llu, loadChannel Config  TCPDIRECT 2\n", dbp::tools::srv::current());
#endif
#ifndef TCPDIRECT
			int iHot = dbp::net::srv::getNoBlockReuseUdpListener(MulticastPort, MulticastIp, InterfaceIp);
			if (-1 == iHot)
			{
				std::cerr << "load loadChannel: " << pszName
						<< " getNoBlockReuseUdpListener fail, Channel_ID:"
						<< channel.m_uChannelId  << std::endl;
				return false;
			}
#else
			flush_printf("tm:%llu, loadChannel Config  TCPDIRECT 3\n", dbp::tools::srv::current());
			struct zfur *zfHot;
			{
				struct addrinfo *ai_local = nullptr;
				char buf[1024];
				snprintf(buf, sizeof(buf), "%s:%u", MulticastIp .c_str(), MulticastPort );
				if (getaddrinfo_hostport(buf, NULL, &ai_local) != 0) {
					zf_stack_free(stack);
					zf_attr_free(attr);
					return false;
				}
				if (zfur_alloc(&zfHot, stack, attr)) {
					zf_stack_free(stack);
					zf_attr_free(attr);
					return false;
				}
				if (zfur_addr_bind(zfHot, ai_local->ai_addr, ai_local->ai_addrlen, NULL, 0, 0)) {
					zfur_free(zfHot);
					zf_stack_free(stack);
					zf_attr_free(attr);
				return false;
				}
			}
			flush_printf("tm:%llu, loadChannel Config  TCPDIRECT 4\n", dbp::tools::srv::current());
#endif
			auto& Refresh = ChannelNode["Refresh"];
			InterfaceIp = Refresh["InterfaceIp"].get<std::string>();

			MulticastIp = Refresh["MulticastIp"].get<std::string>();
			MulticastPort = Refresh["MulticastPort"].get<unsigned short int>();
#ifndef TCPDIRECT
			int iRefresh = dbp::net::srv::getNoBlockReuseUdpListener(MulticastPort, MulticastIp, InterfaceIp);
			if (-1 == iRefresh)
			{
				std::cerr << "load loadChannel: " << pszName
						<< " getNoBlockReuseUdpListener fail, Channel_ID:"
						<< channel.m_uChannelId  << std::endl;
				close(iHot);
				return false;
			}
#else
			InterfaceName = Refresh["InterfaceName"].get<std::string>();
			flush_printf("tm:%llu, loadChannel Config  TCPDIRECT 5\n", dbp::tools::srv::current());
			struct zfur *zfRefresh;
			{
				struct addrinfo *ai_local = nullptr;
				char buf[1024];
				snprintf(buf, sizeof(buf), "%s:%u", MulticastIp.c_str(), MulticastPort);
				if (getaddrinfo_hostport(buf, NULL, &ai_local) != 0) {
					zfur_free(zfHot);
					zf_stack_free(stack);
					zf_attr_free(attr);
					return false;
				}
				if (zfur_alloc(&zfRefresh, stack, attr)) {
					zfur_free(zfHot);
					zf_stack_free(stack);
					zf_attr_free(attr);
					return false;
				}
				if (zfur_addr_bind(zfRefresh, ai_local->ai_addr, ai_local->ai_addrlen, NULL, 0, 0)) {
					zfur_free(zfRefresh);
					zfur_free(zfHot);
					zf_stack_free(stack);
					zf_attr_free(attr);
					return false;
				}
			}
			flush_printf("tm:%llu, loadChannel Config  TCPDIRECT 6\n", dbp::tools::srv::current());
#endif
			channel.m_uRetranProxyIdx = ChannelNode["RetranProxyIndex"].get<std::size_t>();
#ifndef TCPDIRECT
			channel.m_iHot = iHot;
			channel.m_iRefresh = iRefresh;
#else
			channel.m_zfHot = zfHot;
			channel.m_zfRefresh = zfRefresh;
#endif
			if (channel.m_uRetranProxyIdx >= retranVec.size())
			{
				std::cerr << "load loadChannel: " << pszName
						<< " retran proxy setting fail, Channel_ID:"
						<< channel.m_uChannelId  << std::endl;
#ifndef TCPDIRECT
				close(iHot);
				close(iRefresh);
#else
				zfur_free(zfRefresh);
				zfur_free(zfHot);
				zf_stack_free(stack);
				zf_attr_free(attr);
#endif
				return false;
			}
#ifndef TCPDIRECT
			channel.m_iEpoll = epoll_create(2);
			if (channel.m_iEpoll <= 0)
			{
				std::cerr << "load loadChannel: " << pszName
						<< " create epoll fail, Channel_ID:"
						<< channel.m_uChannelId  << std::endl;
				close(iHot);
				close(iRefresh);
				return false;
			}
#else
			flush_printf("tm:%llu, loadChannel Config  TCPDIRECT 7\n", dbp::tools::srv::current());
			if (zf_muxer_alloc(stack, &channel.m_zfMuxer))
			{
				zfur_free(zfRefresh);
				zfur_free(zfHot);
				zf_stack_free(stack);
				zf_attr_free(attr);
				return false;
			}
			flush_printf("tm:%llu, loadChannel Config  TCPDIRECT 8\n", dbp::tools::srv::current());
#endif
			std::cerr << "load loadChannel: " << pszName
					<< " success, Channel_ID:"
					<< channel.m_uChannelId  << std::endl;
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

inline static bool loadTcpChannel(json& _json)
{
	try
	{

		auto itActivate = mActivateChannel.find("TcpChannel");
		if(itActivate != mActivateChannel.end()){
			auto it = _json.find("TcpChannel");
			if (_json.end() != it)
			{
				auto cfg = _json["TcpChannel"];
				tcpConfig.IP = cfg["IP"];
				tcpConfig.PORT = cfg["PORT"];
				tcpConfig.BIND = cfg["BIND"];
				auto items = dbp_tcp_md::get_codes(tcpConfig.IP, tcpConfig.PORT);
				for (const auto& item : items)
				{
					auto& tradable = tcpMap[item.code];
					std::memcpy(tradable.m_TcpCode, item.code.c_str(), sizeof(item.code.length()));
					tradable.m_TradeSide = TradeSide::NO_SIDE;
					tradable.m_TradeType = 0;
	#ifndef NOT_MEASURE
					tradable.m_MsgTime = 0;
	#endif
					tradable.m_MsgType = MsgType::TCP_TRADE;
					tradable.m_Code = 0;
					tradable.m_LastTradePrice = 0;
					tradable.m_LastTradeQuantity = 0;
					tradable.m_Ask[0].m_iPrice = 0;
					tradable.m_Ask[0].m_uNumberOfOrder = 0;
					tradable.m_Ask[0].m_uQuantity = 0;
					tradable.m_Bid[0].m_iPrice = 0;
					tradable.m_Bid[0].m_uNumberOfOrder = 0;
					tradable.m_Bid[0].m_uQuantity = 0;
				}
			}
		}
	}
	catch(...)
	{
		std::cerr << "load loadTcpChannel fail" << std::endl;
		return false;
	}
	return true;
}

#endif


