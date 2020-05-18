#include <algo.hpp>
#include <semi.hpp>

algo::algo(user& u):
	_u(u)
{
}

algo* algo::get_algo(user& u, const std::string& lib, json&)
{
	if (lib == "semi")
	{
		return new semi(u);
	}
	else
	{
		return nullptr;
	}
}



