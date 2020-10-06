#include <dbp_tcp_md.hpp>
#include <cstring>
#include <ctime>

dbp_tcp_md::dbp_tcp_md
(
	const std::string& host,
	unsigned short int port,
	book_handler&& handle_book,
	trade_handler&& handle_trade
):
     _codes(),
	_client(host, port),
	_handler(*this),
	_handle_book(std::move(handle_book)),
	_handle_trade(std::move(handle_trade)),
	_last_map_size(0),
	_last_map_tm(0),
	_ready(false)
{
	_client.set_connected([&]()
	{
		 fprintf(stderr, "Tcp Channel Connected\n");
	});
	_client.set_disconnected([&]()
	{
		_handler.reset();
	});
	_client.set_on_msg([&](const char* ptr, std::size_t size){_handler.handle_msg(ptr, size);});
}

dbp_tcp_md::dbp_tcp_md(dbp_tcp_md&& client):
	_codes(std::move(client._codes)),
	_client(std::move(client._client)),
	_handler(std::move(client._handler)),
	_handle_book(std::move(client._handle_book)),
	_handle_trade(std::move(client._handle_trade)),
	_last_map_size(client._last_map_size),
	_last_map_tm(client._last_map_tm),
	_ready(client._ready)
{
}

dbp_tcp_md::~dbp_tcp_md()
{
	close();
}

dbp_tcp_md& dbp_tcp_md::operator= (dbp_tcp_md&& client)
{
	_codes = std::move(client._codes);
	_client = std::move(client._client);
	_handler = std::move(client._handler);
	_handle_book = std::move(client._handle_book);
	_handle_trade = std::move(client._handle_trade);
	_ready = client._ready;
	return *this;
}

void dbp_tcp_md::run()
{
	_client.run();
}

void dbp_tcp_md::close()
{
	_client.close();
}

const std::vector<dbp_tcp_md::code_node> dbp_tcp_md::codes() const
{
	std::vector<dbp_tcp_md::code_node> rt;
	for (const auto& item : _codes)
		rt.push_back(item.second);
	return rt;
}

dbp_tcp_md::operator bool() const
{
	return _ready;
}

void dbp_tcp_md::handle_static(const char* ptr)
{
	code_node n;
	n.code = ptr + 210;
	n.md_code = ptr + 8;
	n.trade_code = n.code;
	n.underlying_code = ptr + 132;

	//fprintf(stderr, "Available Code  = %s \n", n.code.c_str());

	if (_codes.end() != _codes.find(n.code))
	{
		fprintf(stderr, "Available Code  = %s \n", n.code.c_str());
		auto tm = time(0);
		auto size = _codes.size();
		if (0 == _last_map_tm || size != _last_map_size)
		{
			_last_map_size = size;
			_last_map_tm = tm;
		}
		else if (tm - _last_map_tm > 10)
		{
			_ready = true;
		}
		return;
	}
	_codes[n.code] = n;
}

void dbp_tcp_md::handle_msg(const char* ptr)
{
	auto type = get_type(ptr);
	if(book_msg_type == type)
		_handle_book(get_book(ptr));
	else if (trade_msg_type == type)
		_handle_trade(get_trade(ptr));
	else if (static_msg_type == type)
		handle_static(ptr);
}

std::vector<dbp_tcp_md::code_node> dbp_tcp_md::get_codes(const std::string& host, unsigned short int port)
{
	dbp_tcp_md cli(host, port, [&](const dbp_tcp_md::book&){}, [&](const dbp_tcp_md::trade&){});
	while (!cli)
	{
		cli.run();
	}
	std::vector<dbp_tcp_md::code_node> rt = cli.codes();
	return rt;
}




