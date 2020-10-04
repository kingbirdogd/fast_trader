#ifndef DBP_TCP_MD_INC_DBP_TCP_MD_HPP_
#define DBP_TCP_MD_INC_DBP_TCP_MD_HPP_

#include <functional>
#include <net/tcp_client.hpp>
#include <vector>
#include <unordered_map>
#include <stream_handler.hpp>

class dbp_tcp_md
{
public:
	#pragma pack(push, 1)
	struct book
	{
		char code[16];
		int gen_val10;
		int best_bid_price;
		int best_ask_price;
		unsigned long long best_bid_quantity;
		unsigned long long best_ask_quantity;
		long long timestamp;
	};
	#pragma pack(pop)
	struct code_node
	{
		std::string code;
		std::string md_code;
		std::string trade_code;
		std::string underlying_code;
	};
	#pragma pack(push, 1)
	struct trade
	{
		char code[16];
		unsigned long long trade_id;
		long long total_trade_quantity;
		unsigned long long total_trade_turnover;
		unsigned int trade_cnt;
		int trade_price;
		int trade_turnover;
		int net_change;
		int percentage_change;
		int weighting;
		unsigned long long trade_quantity;
		long long gen_vall;
		int peRido;
		unsigned char trade_type;
		unsigned char aggressive_side;
		long long timestamp;
	};
	#pragma pack(pop)
private:
	const static unsigned short int book_msg_type = 201;
	const static unsigned short int trade_msg_type = 202;
	const static unsigned short int static_msg_type = 203;
	const static std::size_t type_offset = 6;
	const static std::size_t header_offset = 8;
private:
	unsigned short int get_type(const char* ptr)
	{
		return (*static_cast<const unsigned short int*>(static_cast<const void*>(ptr + type_offset)));
	}
	const book& get_book(const char* ptr)
	{
		return (*static_cast<const book*>(static_cast<const void*>(ptr + header_offset)));
	}
	const trade& get_trade(const char* ptr)
	{
		return (*static_cast<const trade*>(static_cast<const void*>(ptr + header_offset)));
	}
private:
	using Handler = stream_handler<dbp_tcp_md, unsigned short int, 2048, 2, 0>;
	friend class stream_handler<dbp_tcp_md, unsigned short int, 2048, 2, 0>;
public:
	using book_handler = std::function<void(const book&)>;
	using trade_handler = std::function<void(const trade&)>;
private:
	std::unordered_map<std::string, code_node> _codes;
	tcp_client _client;
	Handler _handler;
	book_handler _handle_book;
	trade_handler _handle_trade;
	std::size_t _last_map_size;
	time_t _last_map_tm;
	bool _ready;
private:
	void handle_static(const char* ptr);
public:
	dbp_tcp_md
	(
		const std::string& host,
		unsigned short int port,
		book_handler&& handle_book,
		trade_handler&& handle_trade
	);
	dbp_tcp_md(dbp_tcp_md&&);
	dbp_tcp_md& operator= (dbp_tcp_md&&);
	~dbp_tcp_md();
	void run();
private:
	void handle_msg(const char* ptr);
	void close();
	const std::vector<code_node> codes() const;
	operator bool () const;
public:
	dbp_tcp_md(const dbp_tcp_md&) = delete;
	dbp_tcp_md& operator= (const dbp_tcp_md&) = delete;
public:
	static std::vector<code_node> get_codes(const std::string& host, unsigned short int port);
};



#endif /* DBP_TCP_MD_INC_DBP_TCP_MD_HPP_ */
