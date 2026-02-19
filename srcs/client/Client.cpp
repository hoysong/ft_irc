#include "Client.hpp"
#include <unistd.h> // close().
#include <sys/socket.h> // recv.
#include <iostream> // cout cerr.

bool Client::recvBuffer( void )
{
	std::cout << "\t[Client::recvBuffer()]" << std::endl;
	char buffer[4096];
	ssize_t bytes_read = recv(m_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0) {
		// 연결 종료 처리 (기존 코드 유지)
		if (bytes_read == 0)
			std::cout << "\t\tClient disconnected: " << m_fd << std::endl;
		else
			std::cerr << "\t\trecv error: " << m_fd << std::endl;
		return( false );
	}
	return ( true );
};

int Client::getFd( void ) const
{
	return (m_fd);
};

/***************************/
/* constcurtor/destructor. */
/***************************/
Client::Client( int fd ) : m_fd(fd)
{
	std::cout << "\t[Client::Client()]: client constructor called. fd: " << fd << std::endl;
};
Client::~Client( void )
{
	std::cout << "\t[Client::~Client()] fd: " << m_fd << std::endl;
	close(m_fd);
};
