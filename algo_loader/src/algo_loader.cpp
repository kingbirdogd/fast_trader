#include <algo_loader.hpp>
#include <bear.hpp>
#include <csalgo.hpp>
#include <s1algo.hpp>
#include <s1algoput.hpp>
#include <semi.hpp>
#include <semipro.hpp>

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




