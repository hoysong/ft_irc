#include "Channel.hpp"
#include "Client.hpp"
#include <iostream>

bool Channel::addMember( Client &client )
{
	Channel::memberMap::iterator iter = m_members.find(client.getNickName());
	if (iter != m_members.end())
	{
		std::cout << "failed to add member to channel " << m_channelName << std::endl;
		return (false); // 이미 있음.
	}
	m_members[client.getNickName()] = &client;
	client.addJoinedChannel(*this);
	std::cout << "success to add member to channel " << m_channelName << std::endl;
	return (true);
}
bool Channel::removeMember( Client &client )
{
	Channel::memberMap::iterator iter = m_members.find(client.getNickName());
	if (iter == m_members.end())
		return (false); // 이미 없음.
	iter->second->removeJoinedChannel(*this);
	m_members.erase(iter);
	return (true);
}

bool Channel::findMember( Client &client )
{
	if (m_members.find(client.getNickName()) == m_members.end())
		return (false);
	return (true);
}

bool Channel::isChannelEmpty( void )
{
	std::cout << "\t[Channel::isChannelEmpty()]: size is " << m_members.size() << std::endl;
	return (m_members.empty());
}

Channel &Channel::setChannelName( const std::string &name )
{
	std::cout << "\t[Channel::setChannelName()]: " << this << std::endl;
	m_channelName = name;
	return (*this);
}

Channel &Channel::assignPasswd( const std::string &passwd )
{
	std::cout << "\t[Channel::assignPasswd()]: " << this << std::endl;
	m_passwd = passwd;
	return (*this);
}

std::string Channel::getPasswd( void )
{
	return (m_passwd);
}

std::map<std::string, Client *> &Channel::getChannelMembers( void )
{
	return (m_members);
}

std::string Channel::getChannelName( void )
{
	return (m_channelName);
}

// ======================================================================
// constructor/destructor.
// ======================================================================

Channel::Channel( void ) : m_passwd("")
{
	std::cout << "\t[Channel::Channel()]: " << this << std::endl;
}

Channel::~Channel( void )
{
	std::cout << "\t[Channel::~Channel()]: " << this << ":" << m_channelName + ":" + m_passwd << std::endl;
}
Channel::Channel(const Channel &other)
{
	std::cout << "\t[Channel::Channel(copy)]: this=" << this << " from=" << &other << std::endl;
}
