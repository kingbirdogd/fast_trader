#ifndef ALGO_LOADER_INC_ALGO_LOADER_HPP_
#define ALGO_LOADER_INC_ALGO_LOADER_HPP_

#include <algo.hpp>

class algo_loader
{
public:
	static algo* get_algo(user& u, const std::string& name, const std::string& lib, json& cfg);
};



#endif /* ALGO_LOADER_INC_ALGO_LOADER_HPP_ */
