#include "Client.hpp"
#include "Channel.hpp"
#include <unistd.h> // close().
#include <iostream> // cout cerr.
#include "MyLibft.hpp"

void Client::appendBuffer( char *buffer, ssize_t size )
{
	m_buffer.append(buffer, size);
	MyLibft::showBuffer(m_buffer);
}

bool Client::recvFd( void )
{
	std::cout << "\t[Client::recvBuffer()]" << std::endl;
	char buffer[5];
	ssize_t bytes_read = recv(m_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read <= 0)
	{ // 연결 종료 처리 (기존 코드 유지)
		
		if (bytes_read == 0)
			std::cout << "\t\tClient disconnected: " << m_fd << std::endl;
		else
			std::cerr << "\t\trecv error: " << m_fd << std::endl;
		return( false );
	}
	appendBuffer(buffer, bytes_read);
	return ( true );
};

int Client::getFd( void ) const
{
	return (m_fd);
};

std::string Client::getNickName( void )
{
	return (m_nickName);
}

std::string Client::getUserName( void )
{
	return (m_userName);
}

std::string Client::getRealName( void )
{
	return (m_realName);
}

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

std::string	Client::getHost(void)
{
	struct sockaddr_in	addr;
	socklen_t			len = sizeof(addr);

	if (getpeername(m_fd, (struct sockaddr*)&addr, &len) == -1)
		return "unknown";

	char	buf[INET_ADDRSTRLEN];

	if (!inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf)))
		return "unknown";

	return std::string(buf); // ip같은거 203.0.113.10
}

std::string Client::getMsgPrefix( void )
{
	std::string host = getHost();
	std::stringstream ss;
	ss << ":" << m_nickName << "!" << m_userName << "@" << host;
	return (ss.str());
}

std::map<std::string, Channel *> Client::getJoinedChannel( void )
{
	return (m_channels);
}

//std::string &Client::getRefNickName( void )
//{
//	return (m_nickName);
//}

/* client manager로부터 닉변 받은 상황. */
/* 자체적으로 닉네임 채널들에게 알리기.*/
void Client::assignNickName( const std::string &name )
{
	std::map<std::string, Channel *>::iterator iter = m_channels.begin();
	std::map<std::string, Channel *>::iterator iterEnd = m_channels.end();
	while (iter != iterEnd)
	{
		iter->second->broadcastNickChanged(*this, name);
		iter++;
	}
	m_nickName = name;
}

bool Client::isAuthed( void )
{
	return (m_authed);
}

bool Client::isRegistered( void )
{
	return (m_registered);
}

void Client::setAuthed( void )
{
	m_authed = true;
}

void Client::setUserName( const std::string &str )
{
	m_userName = str;
}

void Client::setInvisible( bool flag )
{
	m_modInvisible = flag;
}
void Client::setWallops( bool flag )
{
	m_modWallops = flag;
}

void Client::setRealName( const std::string &name )
{
	m_realName = name;
}

void Client::setRegistered( void )
{
	m_registered = true;
}

void Client::addJoinedChannel( Channel &channel )
{
	m_channels[channel.getChannelName()] = &channel;
}

/* 함수 수정 금지. 채널에서 호출되고 있음. */
bool Client::removeJoinedChannel( Channel &channel )
{
	std::map<std::string, Channel *>::iterator iter = m_channels.find(channel.getChannelName());
	if (iter == m_channels.end())
		return (false); // 입장하지 않았음.
	m_channels.erase(iter);
	return (true);
}

/* 검증 안된 함수. */
void Client::quitAllChannels( void )
{
	std::map<std::string, Channel *>::iterator iter = m_channels.begin();
	std::map<std::string, Channel *>::iterator iter_end = m_channels.end();
	while (iter != iter_end)
	{
		iter->second->removeMember(*this, "");
		iter++;
	}
	m_channels.clear();
}

bool Client::popLine( std::string &line )
{
	size_t crlfPos = m_buffer.find("\r\n");
	if ( crlfPos == std::string::npos )
	{ /* 아직 라인이 완성되지 않음. */
		return (false);
	}
	line = m_buffer.substr(0, crlfPos);
	m_buffer.erase(0, crlfPos + 2);
	return (true);
}

/***************************/
/* constcurtor/destructor. */
/***************************/
Client::Client( int fd ) :
	m_fd(fd),
	m_authed(false),
	m_registered(false),
	m_IRCOperaotr(false),
	m_modInvisible(false),
	m_modWallops(false),
	m_nickName("*")
{
	std::cout<<"\t[Client::Client()]: client constructor called. fd: " << fd << std::endl;
};
Client::~Client( void )
{
	std::cout << "\t[Client::~Client()] fd: " << m_fd << std::endl;
	close(m_fd);
};

bool Client::operator < (const Client &ref ) const
{
	return (this->m_fd < ref.m_fd);
}
