#include <unistd.h>
#include <top_shared.hpp>
#include <cstdio>

top_shared_node::top_shared_node()
{
	StartTopTraderFlash();
	std::fprintf(stderr, "StartTopTraderFlash\n");
	::sleep(10);
}

top_shared_node::~top_shared_node()
{
	StopTopTraderFlash();
	::sleep(10);
}
