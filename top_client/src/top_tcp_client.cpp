#include <top_tcp_client.hpp>
#include <cstring>

top_tcp_client::top_tcp_client
(
	const std::string& host,
	unsigned short int port,
	const std::string& user,
	const std::string& pass,
	unsigned long long buy_power
):
	top_client(user, pass, buy_power),
	_client(host, port),
	_last_login(0),
	_retry_wait(false)
{
	std::fprintf(stderr, "start top_tcp_client");
	_client.set_connected([&]()
	{
		time_t now = ::time(nullptr);
		if (now - _last_login > 15)
		{
			login();
			_last_login = now;
		}
		else
		{
			_retry_wait = true;
		}

	});
	_client.set_disconnected([&](){clean();});
	_client.set_on_msg([&](const char* ptr, std::size_t size){handle_msg(ptr, size);});
}

top_tcp_client::top_tcp_client(top_tcp_client&& client):
	top_client(std::move(client)),
	_client(std::move(client._client)),
	_last_login(client._last_login),
	_retry_wait(client._retry_wait)
{
}

top_tcp_client::~top_tcp_client()
{
	close();
}

top_tcp_client& top_tcp_client::operator= (top_tcp_client&& client)
{
	top_client& self = *this;
	self = std::move(client);
	_client = std::move(client._client);
	_last_login = client._last_login;
	_retry_wait = client._retry_wait;
	return *this;
}

void top_tcp_client::run()
{
	_client.run();
	if (_retry_wait)
	{
		time_t now = ::time(nullptr);
		if (now - _last_login > 15)
		{
			login();
			_last_login = now;
			_retry_wait = false;
		}
	}
}

void top_tcp_client::send(const void* ptr, std::size_t size)
{
	_client.send(ptr, size);
}

void top_tcp_client::close()
{
	_client.close();
}




