#include <algo_loader.hpp>
#include <bear.hpp>
#include <csalgo.hpp>
#include <s1algo.hpp>
#include <s1algoput.hpp>
#include <semi.hpp>
#include <semipro.hpp>

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

algo* algo_loader::get_algo(user& u, const std::string& name, const std::string& lib, json&)
{
	if (lib == "semi")
	{
		return new semi(u, name);
	}
	else if (lib == "semipro")
	{
		return new semipro(u, name);
	}
	else if (lib == "bear")
	{
		return new bear(u, name);
	}
	else if (lib == "s1algo")
	{
		return new s1algo(u, name);
	}
	else if (lib == "s1algoput")
	{
		return new s1algoput(u, name);
	}
	else if (lib == "csalgo")
	{
		return new csalgo(u, name);
	}
	else
	{
		return nullptr;
	}
}




