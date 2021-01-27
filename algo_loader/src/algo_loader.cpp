#include <algo_loader.hpp>
#include <bear.hpp>
#include <csalgo.hpp>
#include <s1algo.hpp>
#include <s1algoput.hpp>
#include <semi.hpp>
#include <semipro.hpp>
#include <dlfcn.h>

bool algo_loader::add_algo_to_user(user& u, const std::string& name, const std::string lib, json& cfg)
{
	auto al = algo_loader::get_algo(u, name, lib, cfg);
	if (nullptr == al)
		return false;
	auto it = u._algos.find(name);
	if (u._algos.end() != it)
		delete it->second;
	u._algos[name] = al;
	return true;
}

algo* algo_loader::get_algo(user& u, const std::string& name, const std::string& lib, json& cfg)
{
	algo* al = nullptr;
	if (lib == "semi")
	{
		al = new semi(u, name);
	}
	else if (lib == "semipro")
	{
		al = new semipro(u, name);
	}
	else if (lib == "bear")
	{
		al = new bear(u, name);
	}
	else if (lib == "s1algo")
	{
		al = new s1algo(u, name);
	}
	else if (lib == "s1algoput")
	{
		al = new s1algoput(u, name);
	}
	else if (lib == "csalgo")
	{
		al = new csalgo(u, name);
	}
	else
	{
		auto prefix = std::string(SLASH) + LIB_PREFIX;
		auto subfix = std::string(SHARE_SUBFFIX);
		auto pStart = lib.rfind(prefix);
		auto pEnd = lib.rfind(subfix);
		if (std::string::npos == pStart)
		{
			al =  nullptr;
		}
		else if (std::string::npos == pEnd)
		{
			al =  nullptr;
		}
		else
		{
			pStart += prefix.length();
			if (pEnd >= pStart)
			{
				al = nullptr;
			}
			else
			{
				auto algo_name = lib.substr(pStart, pEnd - pStart);
				auto function_name = "getAlgo_" + algo_name;
				auto handle = dlopen(lib.c_str(), RTLD_LAZY);
				if (!handle)
				{
					al = nullptr;
				}
				else
				{
					auto fp = dlsym(handle, function_name.c_str());
					if (!fp)
					{
						al = nullptr;
					}
					else
					{
						auto loaderFp = (algo::AlgoLoaderFp)(fp);
						return loaderFp(u, name);
					}
				}
			}
		}
	}
	if (al)
	{
		al->set_config(cfg);
	}
	return al;
}




