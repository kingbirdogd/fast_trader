#ifndef INC_STREAM_HANDLER_HPP_
#define INC_STREAM_HANDLER_HPP_
#include <cstring>
#include <string>
#include <vector>
template
<
	typename THandler,
	typename TSize,
	std::size_t BufferSize,
	std::size_t HeaderSize,
	std::size_t LenOffSet
>
class stream_handler
{
private:
	using Buffer = std::vector<char>;
private:
	THandler& _handler;
	Buffer _buffer;
public:
	stream_handler(THandler& handler):
		_handler(handler),
		_buffer()
	{
		_buffer.reserve(BufferSize);
	}
	stream_handler(stream_handler&& h):
		_handler(h._handler),
		_buffer(std::move(h._buffer))
	{
	}
	stream_handler& operator= (stream_handler&& h)
	{
		_buffer = std::move(h._buffer);
		return *this;
	}
	~stream_handler() = default;
	stream_handler(const stream_handler&) = delete;
	stream_handler& operator= (const stream_handler&) = delete;
	void reset()
	{
		_buffer.resize(0);
	}
	void handle_msg(const char* ptr, std::size_t size)
	{
		auto p = ptr;
		auto len = size;
		auto ori_len = _buffer.size();
		bool m_move = false;
		if (0 != ori_len)
		{
			len += ori_len;
			_buffer.resize(len);
			std::memcpy(&_buffer[ori_len], ptr, size);
			p = &_buffer[0];
			m_move = true;
		}
		while (len > HeaderSize)
		{
			const TSize& msg_size = (*static_cast<const TSize*>(static_cast<const void*>(p + LenOffSet)));
			if (len >= msg_size)
			{
				_handler.handle_msg(p);
				len -= msg_size;
				p += msg_size;
			}
			else
			{
				break;
			}
		}
		if (len > 0)
		{
			_buffer.resize(len);
			if (m_move)
				std::memmove(&_buffer[0], p, len);
			else
				std::memcpy(&_buffer[0], p, len);
		}
	}

};



#endif /* INC_STREAM_HANDLER_HPP_ */
