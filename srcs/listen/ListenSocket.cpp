#include "ListenSocket.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <cstring>

int ListenSocket::acceptClient( void )
{
	int clientFd = accept(m_sockfd, NULL, NULL);
	return (clientFd);
}

/************/
/* Getters. */
/************/

int ListenSocket::getFd() const
{
	return m_sockfd;
}

int ListenSocket::getPort() const
{
	return m_port;
}

std::string ListenSocket::getIp() const
{
	return m_ip;
}

/****************/
/* init socket. */
/****************/
ListenSocket::ListenSocket(const std::string& ip, int port) : m_port(port), m_ip(ip)
{
	// make socket.
	std::cout << "[ListenSocket::ListenSocket()]"<< std::endl;
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockfd < 0)
		throw std::runtime_error("[ListenSocket::ListenSocket()]: socket creation failed");

	// set REUSEADDR.
	int opt = 1;
	if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(m_sockfd);
		throw std::runtime_error("[ListenSocket::ListenSocket()]: setsockopt failed");
	}

	// struct init.
	std::memset(&m_addr, 0, sizeof(m_addr));
	m_addr.sin_family = AF_INET;
	
	// set ip.
	m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	if (m_addr.sin_addr.s_addr == INADDR_NONE)
	{
		close(m_sockfd);
		throw std::runtime_error("[ListenSocket::ListenSocket()]: invalid IP address format");
	}
	
	m_addr.sin_port = htons(m_port);

	// Bind.
	if (bind(m_sockfd, (struct sockaddr *)&m_addr, sizeof(m_addr)) < 0)
	{
		close(m_sockfd);
		throw std::runtime_error("[ListenSocket::ListenSocket()]: bind failed");
	}

	// Listen.
	if (listen(m_sockfd, 128) < 0)
	{
		close(m_sockfd);
		throw std::runtime_error("[ListenSocket::ListenSocket()]: listen failed");
	}
	std::cout << "\t새로운 Listen 소켓이 생성되었습니다. fd: " << m_sockfd << std::endl;
}

ListenSocket::~ListenSocket()
{
	if (m_sockfd >= 0)
		close(m_sockfd);
}

