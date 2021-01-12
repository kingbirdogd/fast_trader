#include <algo.hpp>
#include <semi.hpp>
#include <bear.hpp>
#include <s1algo.hpp>
#include <s1algoput.hpp>
#include <csalgo.hpp>

algo::algo(user& u, const std::string& name):
	_u(u),
	_name(name),
	_Profit(0)
{
}

const std::string& algo::get_name() const
{
	return  _name;
}

algo* algo::get_algo(user& u, const std::string& name, const std::string& lib, json&)
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

long long algo::getProfit()
{
	return _Profit;
}



