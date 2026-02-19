#include "EpollManager.hpp"
#include <unistd.h>
#include <iostream>

EpollManager::EpollManager() : m_epfd(-1)
{
	this->m_epfd = epoll_create(1);
	std::cout << "[EpollManager::EpollManager()]: epoll fd: " << m_epfd << std::endl;
	if (this->m_epfd < 0)
		throw std::runtime_error("epoll_create failed");
}

EpollManager::~EpollManager()
{
	std::cout << "[EpollManager::~EpollManager()]" << std::endl;
	if (this->m_epfd >= 0)
		close(this->m_epfd);
}

int EpollManager::getFd() const
{
	return this->m_epfd;
}

bool EpollManager::add(int fd, uint32_t events, void *event_handler)
{
	struct epoll_event ev;
	ev.events  = events;
	ev.data.fd = fd;
	ev.data.ptr = event_handler;
	if (epoll_ctl(this->m_epfd, EPOLL_CTL_ADD, fd, &ev) < 0)
	{
		std::cerr << "\t[EpollManger::mod()]: Failed to EPOLL_CTL_ADD fd " << fd << std::endl;
		return (false);
	}
	std::cout << "\t[EpollManger::mod()]: EPOLL_CTL_ADD success to fd " << fd << std::endl;
	return (true);
}

int EpollManager::mod(int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events  = events;
	ev.data.fd = fd;
	if (epoll_ctl(this->m_epfd, EPOLL_CTL_MOD, fd, &ev) < 0)
	{
		std::cerr << "\t[EpollManger::mod()]: Failed to EPOLL_CTL_MOD to fd " << fd << std::endl;
		return (-1);
	}
	std::cerr << "\t[EpollManger::mod()]: EPOLL_CTL_MOD success to fd " << fd << std::endl;
	return (0);
}

bool EpollManager::del(int fd)
{
	if (epoll_ctl(this->m_epfd, EPOLL_CTL_DEL, fd, 0) < 0)
	{
		std::cerr << "\t[EpollManager::del()]: Failed to close " << fd << std::endl;
		return (false);
	}
	std::cout << "\t[EpollManager::del()]: fd " << fd << " deleted from epoll." << std::endl;
	return (true);
}

int EpollManager::wait(struct epoll_event *events, int maxevents, int timeout)
{
	// 여기서는 errno로 분기하지 않고, 호출한 쪽에서
	// "리턴값 < 0 이면 전체 에러로 보고 서버 정리" 정도로만 취급하게 설계
	return epoll_wait(this->m_epfd, events, maxevents, timeout);
}
