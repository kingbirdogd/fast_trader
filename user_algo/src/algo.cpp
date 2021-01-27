#include <algo.hpp>

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

long long algo::getProfit()
{
	return _Profit;
}

void algo::set_config(const json&)
{
}



