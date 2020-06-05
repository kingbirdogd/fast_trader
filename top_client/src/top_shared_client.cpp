#include <top_shared_client.hpp>
#include <cstring>

top_shared_node* top_shared_client::_node = nullptr;

top_shared_client::top_shared_client
(
	const std::string& user,
	const std::string& pass,
	unsigned long long buy_power
):
	top_client(user, pass, buy_power),
	_queue(),
	_connected(false)
{
	if (nullptr == _node)
	{
		_node = new top_shared_node();
	}
	std::fprintf(stderr, "start top_shared_client\n");
	_queue.warm_up();
}

top_shared_client::top_shared_client(top_shared_client&& client):
	top_client(std::move(client)),
	_queue(),
	_connected(false)
{
	_queue.warm_up();
}

top_shared_client& top_shared_client::operator= (top_shared_client&& client)
{
	top_client& self = *this;
	self = std::move(client);
	_connected = false;
	return *this;
}

void top_shared_client::run()
{
	/*
	top_buffer buff;
	if (_queue.try_dequeue(buff))
	{
		handle_msg(&buff[0], buff.size());
	}
	*/
	if (!_connected)
	{
		std::fprintf(stderr, "top_shared_client::run\n");
		auto id = BindAccount(callback);
		_node->obj_to_id[this] = id;
		_node->id_to_obj[id] = this;
		_connected = true;
		login();
	}
}

void top_shared_client::send(const void* ptr, std::size_t size)
{
	if (_connected)
	{
		//std::fprintf(stderr, "top_shared_client::send\n");
		auto it = _node->obj_to_id.find(this);
		if (_node->obj_to_id.end() != it)
		{
			//std::fprintf(stderr, "top_shared_client::find obj : send\n");
			SendClientRequest(it->second, static_cast<const char*>(ptr), size);
		}
		//std::fprintf(stderr, "top_shared_client::end send\n");
	}
}

void top_shared_client::close()
{
	if (_connected)
	{
		auto it = _node->obj_to_id.find(this);
		if (_node->obj_to_id.end() != it)
		{
			_node->id_to_obj.erase(it->second);
			UnbindAccount(it->second);
			_node->obj_to_id.erase(it);
		}
		_connected = false;
	}
}

void top_shared_client::callback(int id, const char* ptr, size_t size)
{
	//std::fprintf(stderr, "top_shared_client::callback\n");
	auto it = _node->id_to_obj.find(id);
	if (_node->id_to_obj.end() != it)
	{
		auto cli = static_cast<top_shared_client*>(it->second);
		top_buffer buff;
		buff.reserve(size);
		std::memcpy(&buff[0], ptr, size);
		//cli->_queue.enqueue(buff);
		cli->handle_msg(ptr, size);
	}
}

void top_shared_client::oldcallback(const char* , size_t )
{
	std::fprintf(stderr, "top_shared_client::old callback\n");
}




