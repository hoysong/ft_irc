#ifndef EPOLLMANAGER_HPP
# define EPOLLMANAGER_HPP

#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>

class EpollManager
{
	private:
		int m_epfd;
	
	public:
		EpollManager();
		~EpollManager();
		
		int  getFd() const;
		
		bool add( int fd, uint32_t events, void *event_handler);
		int mod( int fd, uint32_t events );
		bool del( int fd );
	
		int  wait( struct epoll_event *events, int maxevents, int timeout );
};

#endif
