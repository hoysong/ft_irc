#include "IServerController.hpp"
#include "Msg.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "MyLibft.hpp"
#include "ircError.hpp"
#include <iostream>
#include <map>
#include <set>
#include <sstream>

void Channel::announce( void )
{
	std::cout << "============Channel============" << std::endl;
	std::cout << "m_channelName: " << m_channelName << std::endl;
	std::cout << "m_passwd     : " << m_passwd << std::endl;
	std::cout << "m_inviteOnly : " << m_inviteOnly << std::endl;
	std::cout << "m_topicOpOnly: " << m_topicOpOnly << std::endl;
	std::cout << "m_maxMembers : " << m_maxMembers << std::endl;

	std::cout << "[Announce Channel Members]" << std::endl;
	std::map<std::string, Client *>::iterator iter = m_members.begin();
	std::map<std::string, Client *>::iterator iter_end = m_members.end();
	while (iter != iter_end)
	{
		iter->second->announce();
		iter++;
	}

	std::cout << "[Announce Channel Operators]" << std::endl;
	std::set<Client *>::iterator operIter = m_opers.begin();
	std::set<Client *>::iterator operIter_end = m_opers.end();
	while (operIter != operIter_end)
	{
		(*operIter)->announce();
		operIter++;
	}
}

bool Channel::broadcastMsg( const std::string &msg )
{
	std::map<std::string, Client *>::iterator iter = m_members.begin();
	std::map<std::string, Client *>::iterator iter_end = m_members.end();
	while (iter != iter_end)
	{
		if (!sendMsg(iter->second->getFd(), msg))
			m_server.addClientToRemove(*(iter->second));
		iter++;
	}
	return (true);
}

void Channel::newMemberBroadcast(Client &client)
{
	std::map<std::string, Client *>::iterator iter = m_members.begin();
	std::map<std::string, Client *>::iterator iter_end = m_members.end();
	while (iter != iter_end)
	{
		Msg()
			.setPrefix(client.getMsgPrefix())
			.addParam("JOIN")
			.addParam(m_channelName)
			.sendTo(*(iter->second), m_server);
		iter++;
	}
}

void Channel::syncNick( Client &client, const std::string &newNick)
{
	std::map<std::string, Client *>::iterator iter = m_members.find(client.getNickName());
	if (iter == m_members.end())
		return ; // 없으면 무시.
	m_members.erase(iter);
	m_members[newNick] = &client;
}

std::string Channel::membersToString(void)
{
	std::map<std::string, Client *>::iterator iter = m_members.begin();
	std::map<std::string, Client *>::iterator iterEnd = m_members.end();
	std::string members;
	while (iter != iterEnd)
	{
		if(m_opers.find(iter->second) != m_opers.end())
			members += "@" + iter->second->getNickName() + ' ';
		else
			members += iter->second->getNickName() + ' ';
		iter++;
	}
	if (members.rbegin() != members.rend() && *members.rbegin() == ' ')
		members.erase(members.length() - 1);
	return (members);
}

void Channel::addInvitedMember( Client &client )
{
	m_invitedMembers.insert(&client);
}
void Channel::removeInvitedMember( Client &client )
{
	std::set<Client *>::iterator iter = m_invitedMembers.find(&client);
	if (iter == m_invitedMembers.end())
		return ;
	m_invitedMembers.erase(iter);
}

bool Channel::addMember( Client &client, const std::string &passwd )
{
	Channel::memberMap::iterator iter = m_members.find(client.getNickName());
	if (iter != m_members.end())
	{
		std::cout << "failed to add member to channel " << m_channelName << std::endl;
		return (false); // 이미 있음.
	}
	if (passwd != m_passwd)
	{
		Msg().errBadChannelKey(client.getNickName(), m_channelName).sendTo(client, m_server);
		return (false);
	}
	if (m_inviteOnly && (m_invitedMembers.find(&client) == m_invitedMembers.end()))
	{
		Msg().errInviteOnlyChan(client.getNickName(), m_channelName).sendTo(client, m_server);
		return (false);
	}
	else
	{
		client.removeInvitedChannel(*this);
		removeInvitedMember(client);
	}
	m_members[client.getNickName()] = &client;
	if (m_members.size() == 1)
		addChannelOper(client);
	client.addJoinedChannel(*this);

	broadcastMsg(
			//:nobody2!hoysong@127.0.0.1 JOIN :#test
			Msg()
			.setPrefix(client.getMsgPrefix())
			.addParam("JOIN")
			.addParam(m_channelName)
			.serialize()
		    );

	sendMsg(client.getFd(),
			Msg()
			.rplNamReply(client.getNickName(), m_channelName, membersToString())
			.serialize() + 
			Msg()
			.rplEndOfNames(client.getNickName(), m_channelName)
			.serialize()
			);

	if (m_topic.empty())
		Msg().rplNoTopic(client.getNickName(), m_channelName).sendTo(client, m_server);
	else
		Msg().rplTopic(client.getNickName(), m_channelName, m_topic).sendTo(client, m_server);

	std::cout << "success to add member to channel " << m_channelName << std::endl;
	return (true);
}
bool Channel::removeMember( Client &client )
{
	Channel::memberMap::iterator iter = m_members.find(client.getNickName());
	if (iter == m_members.end())
		return (false); // 이미 없음.
	iter->second->removeJoinedChannel(*this); // 유저 객체에서 채널목록 삭제.
	m_members.erase(iter); // 채널측 유저목록 삭제.
	removeChannelOper(client); // 오퍼라면 삭제.
	return (true);
}

bool Channel::removeMember( const std::string &target )
{
	std::map<std::string, Client *>::iterator iter = m_members.find(target);
	if (iter == m_members.end())
		return (false);
	return (removeMember(*(iter->second)));
}

bool Channel::addChannelOper( Client &client)
{
	std::set<Client *>::iterator iter = m_opers.find(&client);
	if (iter != m_opers.end())
		return (false);
	m_opers.insert(&client);
	return (true);
}

bool Channel::addChannelOper( const std::string &nickName )
{
	std::map<std::string, Client *>::iterator iter = m_members.find(nickName);
	if (iter == m_members.end())
		return (false);
	std::set<Client *>::iterator operIter = m_opers.find(iter->second);
	if (operIter != m_opers.end())
		return (false);
	m_opers.insert(iter->second);
	return (true);
}
bool Channel::removeChannelOper( Client &client)
{
	std::set<Client *>::iterator iter = m_opers.find(&client);
	if (iter == m_opers.end())
		return (false); // 이미 없음.
	m_opers.erase(&client);
	return (true);
}
bool Channel::removeChannelOper( const std::string &nickName )
{
	std::map<std::string, Client *>::iterator iter = m_members.find(nickName);
	if (iter == m_members.end())
		return (false);
	std::set<Client *>::iterator operIter = m_opers.find(iter->second);
	if (operIter == m_opers.end())
		return (false);
	m_opers.erase(iter->second);
	return (true);
}


bool Channel::findMember( const std::string &name )
{
	if (m_members.find(name) == m_members.end())
		return (false);
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
bool Channel::isChannelOper( Client &client )
{
	std::set<Client *>::iterator iter = m_opers.find(&client);
	if (iter == m_opers.end())
		return (false);
	return (true);
}
bool Channel::isChannelOper( const std::string &nickName )
{
	std::map<std::string, Client *>::iterator iter = m_members.find(nickName);
	if (iter == m_members.end())
		return (false);
	std::set<Client *>::iterator operIter = m_opers.find(iter->second);
	if (operIter == m_opers.end())
		return (false);
	return (true);
}
bool Channel::isInviteMode( void )
{
	return (m_inviteOnly);
}
bool Channel::isTopicMode( void )
{
	return (m_topicOpOnly);
}
bool Channel::isKeyMode( void )
{
	return (!m_passwd.empty());
}
bool Channel::isLimitMode( void )
{
	return (m_maxMembers >= 0);
}

void Channel::setInviteMode( bool flag)
{
	this->m_inviteOnly = flag;
}
void Channel::setTopicMode( bool flag)
{
	this->m_topicOpOnly = flag;
}
void Channel::setTopic( const std::string &topic )
{
	m_topic = topic;
}
void Channel::setKeyMode( const std::string &value )
{
	this->m_passwd = value;
}
void Channel::setLimitMode( int value )
{
	this->m_maxMembers = value;
}
bool Channel::setLimitMode( const std::string &value )
{
	int limit;
	if (!MyLibft::aToInt(value, limit))
		return (false);
	m_maxMembers = limit;
	return (true);
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
Client &Channel::getChannelMember( const std::string &nickName )
{
	std::map<std::string, Client *>::iterator iter = m_members.find(nickName);
	return (*iter->second);
}

std::string Channel::getStringChannelLimit( void )
{
	std::stringstream ss;
	ss << m_maxMembers;
	return (ss.str());
}
int Channel::getIntChannelLimit( void )
{
	return (this->m_maxMembers);
}

std::string Channel::getTopic( void )
{
	return (m_topic);
}

#include <sstream>
std::string Channel::modeToString( void )
{
	std::string modes = "+";
	std::stringstream ss;
	if (!m_passwd.empty())
	{
		modes += 'k';
		ss << " " << m_passwd;
	}
	if (m_inviteOnly)
	{
		modes += 'i';
	}
	if (m_topicOpOnly)
	{
		modes += 't';
	}
	if (m_maxMembers >= 0)
	{
		modes += 'l';
		ss << " ";
		ss << m_maxMembers;
	}
	modes += ss.str();
	return (modes);
}

// ======================================================================
// constructor/destructor.
// ======================================================================

Channel::Channel( IServerController &ircServer ) :
	m_server(ircServer),
	m_passwd(""),
	m_inviteOnly(false),
	m_topicOpOnly(false),
	m_maxMembers(-1)
{
	std::cout << "\t[Channel::Channel()]: " << this << std::endl;
}

Channel::~Channel( void )
{
	std::cout << "\t[Channel::~Channel()]: " << this << ":" << m_channelName + ":" + m_passwd << std::endl;
	for(std::set<Client *>::iterator iter = m_invitedMembers.begin(); iter != m_invitedMembers.end(); iter ++)
		(*iter)->removeInvitedChannel(*this);
}
