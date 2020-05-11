#ifndef INC_HEX_HPP_
#define INC_HEX_HPP_
#include <string>

inline char half_byte_hex(const unsigned char& v)
{
	if (v < 10)
	{
		char base = '0';
		return base + v;
	}
	else
	{
		char base = 'A';
		return base + (v - 10);
	}
}

inline std::string byte_hex(const unsigned char& v)
{
	unsigned char high = v / 16;
	unsigned char low = v % 16;
	std::string msg = "";
	msg += half_byte_hex(high);
	msg += half_byte_hex(low);
	return msg;
}

inline std::string get_hex(const void* ptr, std::size_t size)
{
	auto p = static_cast<const unsigned char*>(ptr);
	std::string msg = "0x";
	for (std::size_t i = 0; i < size; ++i)
	{
		msg += byte_hex(p[i]);
	}
	return msg;
}




#endif /* INC_HEX_HPP_ */
