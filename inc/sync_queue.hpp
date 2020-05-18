#ifndef CONTAINER_INC_CONTAINER_SYNC_QUEUE_HPP_
#define CONTAINER_INC_CONTAINER_SYNC_QUEUE_HPP_
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace container
{
	template <typename T>
	class sync_queue
	{
	private:
		std::condition_variable _cond;
		std::mutex   _mutex;
		std::list<T> _q;
	public:
		sync_queue() = default;
		~sync_queue() = default;
		void enqueue(const T& val)
		{
			std::lock_guard<std::mutex> lck(_mutex);
			_q.push_back(val);
			_cond.notify_one();
		}
		void enqueue(T&& val)
		{
			std::lock_guard<std::mutex> lck(_mutex);
			_q.push_back(std::forward<T>(val));
			_cond.notify_one();
		}
		void deque(T& val)
		{
			std::unique_lock<std::mutex> lck(_mutex);
			while (_q.empty())
		    {
				_cond.wait(lck);
		    }
	        val = _q.front();
	        _q.pop_front();
		}
		bool try_deque(T& val)
		{
			std::unique_lock<std::mutex> lck(_mutex);
			if (_q.empty())
				return false;
	        val = _q.front();
	        _q.pop_front();
	        return true;
		}
		void deque(const std::function<void(T&)>& func)
		{
			std::unique_lock<std::mutex> lck(_mutex);
			while (_q.empty())
		    {
				_cond.wait(lck);
		    }
	        func(_q.front());
	        _q.pop_front();
		}
		bool try_deque(const std::function<void(T&)>& func)
		{
			std::unique_lock<std::mutex> lck(_mutex);
			if (_q.empty())
				return false;
	        func(_q.front());
	        _q.pop_front();
	        return true;
		}
		void deque(std::function<void(T&)>&& func)
		{
			deque(func);
		}
		bool try_deque(std::function<void(T&)>&& func)
		{
			return try_deque(func);
		}
	};
}
#endif /* CONTAINER_INC_CONTAINER_SYNC_QUEUE_HPP_ */
