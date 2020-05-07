#ifndef INC_EPOLL_H_
#define INC_EPOLL_H_

#ifndef __APPLE__
#include <sys/epoll.h>
#else
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <vector>

static const int EPOLL_CTL_ADD = 0;
static const int EPOLL_CTL_MOD = 1;
static const int EPOLL_CTL_DEL = 2;

static const uint32_t EPOLLIN =          0b00000000000000000000000000000001;
static const uint32_t EPOLLOUT =         0b00000000000000000000000000000010;
static const uint32_t EPOLLRDHUP =       0b00000000000000000000000000000100;
static const uint32_t EPOLLPRI =         0b00000000000000000000000000001000;
static const uint32_t EPOLLERR =         0b00000000000000000000000000010000;
static const uint32_t EPOLLHUP =         0b00000000000000000000000000100000;
static const uint32_t EPOLLET =          0b00000000000000000000000001000000;
static const uint32_t EPOLLONESHOT =     0b00000000000000000000000010000000;
static const uint32_t EPOLLEXCLUSIVE =   0b00000000000000000000000100000000;

typedef union epoll_data
{
	void    *ptr;
	int      fd;
	uint32_t u32;
	uint64_t u64;
}
epoll_data_t;

struct epoll_event
{
     uint32_t     events;
     epoll_data_t data;
};




inline int epoll_create(int)
{
	return ::kqueue();
}


int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
	struct kevent ev[4];
	int n = 0;
	int flag = 0;
	if (event->events & EPOLLONESHOT)
	{
		flag |= EV_ONESHOT;
	}
	if (event->events & EPOLLIN)
	{
		if (EPOLL_CTL_ADD == op)
		{
			EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD|EV_ENABLE|flag, 0, 0, event->data.ptr);
		}
		else if (EPOLL_CTL_MOD)
		{
			EV_SET(&ev[n++], fd, 0, EV_DELETE, 0, 0, event->data.ptr);
			EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD|EV_ENABLE|flag, 0, 0, event->data.ptr);
		}
		else if (EPOLL_CTL_DEL)
		{
			EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD|EV_ENABLE|flag, 0, 0, event->data.ptr);
		}
	}

	if (event->events & EPOLLOUT)
	{
		if (EPOLL_CTL_ADD == op)
		{
			EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD|EV_ENABLE|flag, 0, 0, event->data.ptr);
		}
		else if (EPOLL_CTL_MOD)
		{
			EV_SET(&ev[n++], fd, 0, EV_DELETE, 0, 0, event->data.ptr);
			EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD|EV_ENABLE|flag, 0, 0, event->data.ptr);
		}
		else if (EPOLL_CTL_DEL)
		{
			EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD|EV_ENABLE|flag, 0, 0, event->data.ptr);
		}
	}
	return kevent(epfd, ev, n, nullptr, 0, nullptr);
}

inline int epoll_wait(int epfd, struct epoll_event* event, int maxevents, int timeout)
{
	struct timespec ts;
	ts.tv_sec = timeout / 1000;
	ts.tv_nsec = (timeout % 1000) * 1000 * 1000;
	std::vector<struct kevent> activeEvs;
	activeEvs.resize(maxevents);
	int n = kevent(epfd, NULL, 0, &activeEvs[0], maxevents, &ts);
	if (n > 0)
	{
		for (int i = 0; i < n; ++i)
		{
			event->data.ptr = activeEvs[i].udata;
			event->events = 0;
			if (EVFILT_READ & activeEvs[i].filter)
			{
				event->events |= EPOLLIN;
			}
			if (EVFILT_WRITE & activeEvs[i].filter)
			{
				event->events |= EPOLLOUT;
			}
		}
	}
	return n;
}



#endif
#endif /* INC_EPOLL_H_ */
