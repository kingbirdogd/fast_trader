#include <algo.hpp>
#include <semi.hpp>

algo::algo(user& u, const std::string& name):
	_u(u),
	_name(name)
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
	else
	{
		return nullptr;
	}
}



