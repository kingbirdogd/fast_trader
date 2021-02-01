#ifndef ALGO_LOADER_INC_ALGO_LOADER_HPP_
#define ALGO_LOADER_INC_ALGO_LOADER_HPP_

#include <algo.hpp>
#include <user.hpp>

class algo_loader
{
public:
	static bool add_algo_to_user(user& u, const std::string& name, const std::string lib, json& cfg);
private:
	static algo* get_algo(user& u, const std::string& name, const std::string& lib, json& cfg);
};



#endif /* ALGO_LOADER_INC_ALGO_LOADER_HPP_ */
