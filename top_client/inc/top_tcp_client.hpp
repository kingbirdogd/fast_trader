#ifndef TOP_CLIENT_INC_TOP_TCP_CLIENT_HPP_
#define TOP_CLIENT_INC_TOP_TCP_CLIENT_HPP_
#include <net/tcp_client.hpp>
#include <top_client.hpp>

class top_tcp_client : public top_client
{
private:
	net::tcp_client _client;
        time_t _last_login;
        bool _retry_wait;
public:
	top_tcp_client
	(
		unsigned long long id,
		const std::string& host,
		unsigned short int port,
		const std::string& user,
		const std::string& pass,
		unsigned long long buy_power = std::numeric_limits<unsigned long long>::max()
	);
	top_tcp_client(top_tcp_client&&);
	top_tcp_client& operator= (top_tcp_client&&);
	virtual ~top_tcp_client();
	virtual void run();
private:
	virtual void send(const void* ptr, std::size_t size);
	virtual void close();
public:
	top_tcp_client(const top_tcp_client&) = delete;
	top_tcp_client& operator= (const top_tcp_client&) = delete;
};




#endif /* TOP_CLIENT_INC_TOP_TCP_CLIENT_HPP_ */
