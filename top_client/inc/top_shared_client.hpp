#ifndef TOP_CLIENT_INC_TOP_SHARED_CLIENT_HPP_
#define TOP_CLIENT_INC_TOP_SHARED_CLIENT_HPP_
#include <top_shared.hpp>
#include <top_client.hpp>

class top_shared_client : public top_client
{
private:
	static top_shared_node* _node;
private:
        bool _connected;
public:
	top_shared_client
	(
		const std::string& user,
		const std::string& pass
	);
	top_shared_client(top_shared_client&&);
	top_shared_client& operator= (top_shared_client&&);
	virtual ~top_shared_client() = default;
	virtual void run();
private:
	virtual void send(const void* ptr, std::size_t size);
	virtual void close();
private:
	static void oldcallback(const char* ptr, size_t size);
	static void callback(int id, const char* ptr, size_t size);
public:
	top_shared_client(const top_shared_client&) = delete;
	top_shared_client& operator= (const top_shared_client&) = delete;
};




#endif /* TOP_CLIENT_INC_TOP_SHARED_CLIENT_HPP_ */
