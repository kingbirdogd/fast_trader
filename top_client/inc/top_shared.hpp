#ifndef TOP_CLIENT_INC_TOP_SHARED_HPP_
#define TOP_CLIENT_INC_TOP_SHARED_HPP_
#include <unordered_map>
//typedef void(*TopTraderFlashCallback)(const char*, size_t);
typedef void(*pfStartTopTraderFlash)();
typedef void(*pfStopTopTraderFlash)();
typedef void(*TopTraderFlashCallback)(const char*, size_t);
typedef void(*TopTraderFlashAccountCallback)(int, const char*, size_t);
typedef int(*pfSendClientRequest)(int, const char*, size_t);
typedef int(*pfBindAccount)(TopTraderFlashAccountCallback);
typedef void(*pfUnbindAccount)(int);
typedef int(*pfBindClient)(TopTraderFlashCallback);
typedef void(*pfUnbindClient)(int);

extern "C" void StartTopTraderFlash();
extern "C" void StopTopTraderFlash();
extern "C" int SendClientRequest(int connectionID, const char* pData, size_t len);
extern "C" int BindClient(TopTraderFlashCallback pfnCallback);
extern "C" void UnbindClient(int connectionID);
extern "C" int BindAccount(TopTraderFlashAccountCallback pfnCallback);
extern "C" void UnbindAccount(int connectionID);

struct top_shared_node
{
	std::unordered_map<void*, int> obj_to_id;
	std::unordered_map<int, void*> id_to_obj;
	top_shared_node();
	~top_shared_node();
};

#endif //TOP_CLIENT_INC_TOP_SHARED_HPP_
