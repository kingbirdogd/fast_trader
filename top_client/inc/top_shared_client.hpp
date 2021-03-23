#ifndef TOP_CLIENT_INC_TOP_SHARED_CLIENT_HPP_
#define TOP_CLIENT_INC_TOP_SHARED_CLIENT_HPP_
#include <top_shared.hpp>
#include <top_client.hpp>
#include <vector>
#include <rapid_ring/ring_buffer_queue.hpp>

class top_shared_client : public top_client
{
private:
	using top_buffer = std::vector<char>;
	using top_buffer_queue = rapid_ring::mpsc_ring_buffer_queue<top_buffer, 8192>;
	struct send_item
	{
		int connectionID;
		top_buffer buff;
	};
	using top_send_buffer_queue = rapid_ring::mpsc_ring_buffer_queue<send_item, 8192>;
private:
	static top_shared_node* _node;
	static top_send_buffer_queue _send_queue;
public:
	static bool has_instanse;
public:
	static void do_send();
private:
	top_buffer_queue _queue;
	bool _connected;
public:
	top_shared_client
	(
		unsigned long long id,
		const std::string& user,
		const std::string& pass,
		unsigned long long buy_power = std::numeric_limits<unsigned long long>::max()
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
