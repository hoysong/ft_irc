#include "Channel.hpp"
#include <iostream>

bool Channel::addMember( Client &client, const std::string &passwd )
{
	Channel::memberMap::iterator iter = m_members.find(client.getNickName());
	if (iter != m_members.end())
		return (false); // 이미 있음.
	m_members[client.getNickName()] = &client;
	return (true);
}
bool Channel::removeMember( Client &client )
{
	Channel::memberMap::iterator iter = m_members.find(client.getNickName());
	if (iter == m_members.end())
		return (false); // 이미 없음.
	m_members.erase(iter);
	return (true);
}

bool Channel::isChannelEmpty( void )
{
	return (m_members.empty());
}

Channel &Channel::setChannelName( const std::string &name )
{
	m_channelName = name;
	return (*this);
}

Channel &Channel::assignPasswd( const std::string &passwd )
{
	m_passwd = passwd;
	return (*this);
}

// ======================================================================
// constructor/destructor.
// ======================================================================

Channel::Channel( void )
{
	std::cout << "[Channel::Channel()]" << std::endl;
}

Channel::~Channel( void )
{
	std::cout << "[Channel::~Channel()]" << m_channelName + ":" + m_passwd << std::endl;
}
