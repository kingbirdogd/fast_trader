/**************************************************************************************************************************
 *Export class:
 *Mutex:
 	dbp::spin::spin_mutex
 	dbp::spin::spin_shared_mutex
*Lock:
 	dbp::spin::spin_lock
 	dbp::spin::spin_unique_lock
 	dbp::spin::spin_shared_lock
 *
 *
 *
 *************************************************************************************************************************/
#ifndef __DBP_SPIN__
#define __DBP_SPIN__
#include <unistd.h>
#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <limits>
namespace dbp
{
	class spin
	{
	private:
		class spin_mutex_core
		{
		private:
			std::atomic_flag m_Flag;
		public:
			spin_mutex_core():
				m_Flag(false)
			{
			}
			~spin_mutex_core()
			{
			}
			spin_mutex_core(const spin_mutex_core&) = delete;
			spin_mutex_core(spin_mutex_core&&) = delete;
			spin_mutex_core& operator= (const spin_mutex_core&) = delete;
			spin_mutex_core& operator= (spin_mutex_core&&) = delete;
			void lock()
			{
				while(m_Flag.test_and_set(std::memory_order_acquire)){}
			}
			bool try_lock()
			{
				return !m_Flag.test_and_set(std::memory_order_acquire);
			}
			void unlock()
			{
				m_Flag.clear(std::memory_order_release);
			}
		};
		typedef std::lock_guard<spin_mutex_core> spin_lock_core;
		class spin_shared_mutex_core
		{
		private:
			std::atomic<unsigned long long> m_Flag;
		public:
			spin_shared_mutex_core():
				m_Flag(0)
			{
			}
			~spin_shared_mutex_core()
			{
			}
			spin_shared_mutex_core(const spin_shared_mutex_core&) = delete;
			spin_shared_mutex_core(spin_shared_mutex_core&&) = delete;
			spin_shared_mutex_core& operator= (const spin_shared_mutex_core&) = delete;
			spin_shared_mutex_core& operator= (spin_shared_mutex_core&&) = delete;
			void lock()
			{
				while (true)
				{
					auto v = m_Flag.load(std::memory_order_relaxed);
					if (m_Flag.compare_exchange_weak(v, std::numeric_limits<unsigned long long>::max(), std::memory_order_release, std::memory_order_relaxed))
						break;
				}
			}
			bool try_lock()
			{
				auto v = m_Flag.load(std::memory_order_relaxed);
				return m_Flag.compare_exchange_strong(v, std::numeric_limits<unsigned long long>::max(), std::memory_order_release, std::memory_order_relaxed);
			}
			void lock_shared()
			{
				while (true)
				{
					auto v = m_Flag.load(std::memory_order_relaxed);
					if (v != std::numeric_limits<unsigned long long>::max())
					{
						if (m_Flag.compare_exchange_weak(v, v + 1, std::memory_order_release, std::memory_order_relaxed))
							break;
					}
				}
			}
			bool try_lock_shared()
			{
				auto v = m_Flag.load(std::memory_order_relaxed);
				if (v != std::numeric_limits<unsigned long long>::max())
				{
					return m_Flag.compare_exchange_strong(v, v + 1, std::memory_order_release, std::memory_order_relaxed);
				}
				else
					return false;
			}
			void unlock()
			{
				m_Flag.store(0, std::memory_order_relaxed);
			}
			void unlock_shared()
			{
				m_Flag.fetch_sub(std::memory_order_relaxed);
			}
		};
		typedef std::unique_lock<spin_shared_mutex_core> spin_unique_lock_core;
		typedef std::shared_lock<spin_shared_mutex_core> spin_shared_lock_core;
	public:
		class spin_mutex : public spin_mutex_core
		{
		public:
			unsigned char m_uFiller[sizeof(unsigned long long) - sizeof(spin_mutex_core)];
		public:
			spin_mutex():
				spin_mutex_core(),
				m_uFiller{0}
			{
			}
			~spin_mutex()
			{
			}
			spin_mutex(const spin_mutex&) = delete;
			spin_mutex(spin_mutex&&) = delete;
			spin_mutex& operator= (const spin_mutex&) = delete;
			spin_mutex& operator= (spin_mutex&&) = delete;

		};
		typedef std::lock_guard<spin_mutex> spin_lock;
		typedef spin_shared_mutex_core spin_shared_mutex;
		typedef spin_unique_lock_core spin_unique_lock;
		typedef spin_shared_lock_core spin_shared_lock;
	};
}
#endif


