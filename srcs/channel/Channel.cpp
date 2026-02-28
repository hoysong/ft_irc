#include "Channel.hpp"
#include "IRCServer.hpp"
#include "Client.hpp"
#include <iostream>
#include <set>

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

void Channel::quitBroadcast( Client &client, const std::string &msg)
{
	std::map<std::string, Client *>::iterator iter = m_members.begin();
	std::map<std::string, Client *>::iterator iter_end = m_members.end();
	while (iter != iter_end)
	{
		sendMsg(iter->second->getFd(), msg);
		iter++;
	}
}

void Channel::newMemberBroadcast(Client &client)
{
	std::map<std::string, Client *>::iterator iter = m_members.begin();
	std::map<std::string, Client *>::iterator iter_end = m_members.end();
	while (iter != iter_end)
	{
		sendMsg(iter->second->getFd(), goodMsg(client, "JOIN", m_channelName));
		iter++;
	}
}

void Channel::broadcastNickChanged( Client &client, const std::string &newNick)
{
	/* 여기까지 함수호출로 들어왔으면 정제된 데이터만 있을 것임.
	 * 신뢰하고 로직 실행해도 됨.
	 * 문제가 있다면 이전에 있는 것이므로 여기서 데이터 검증없이 돌러서 터뜨려야 함.
	 */
	std::map<std::string, Client *>::iterator iter = m_members.begin();
	std::map<std::string, Client *>::iterator iter_end = m_members.end();
	while (iter != iter_end)
	{
		sendMsg(iter->second->getFd(), client.getMsgPrefix() + " NICK :" + newNick + "\r\n");
		iter++;
	}
	// 브로드캐스트 끝났으니 닉변 클라이언트 노드 교체.
	iter = m_members.find(client.getNickName());
	m_members.erase(iter);
	m_members[newNick] = &client;
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
		sendMsg(client.getFd(), errMsg(ERR_BADCHANNELKEY, client, m_channelName, "Cannot join channel (+k)"));
		return (false);
	}
	m_members[client.getNickName()] = &client;
	client.addJoinedChannel(*this);
	newMemberBroadcast(client);
	std::cout << "success to add member to channel " << m_channelName << std::endl;
	return (true);
}
bool Channel::removeMember( Client &client, const std::string &msg )
{
	Channel::memberMap::iterator iter = m_members.find(client.getNickName());
	if (iter == m_members.end())
		return (false); // 이미 없음.
	quitBroadcast(client, msg);
	iter->second->removeJoinedChannel(*this); // 유저 객체에서 채널목록 삭제.
	m_members.erase(iter); // 채널측 유저목록 삭제.
	removeChannelOper(client); // 오퍼라면 삭제.
	return (true);
}

bool Channel::addChannelOper( Client &client)
{
	std::set<Client *>::iterator iter = m_opers.find(&client);
	if (iter != m_opers.end())
		return (false);
	m_opers.insert(&client);
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

Channel::Channel( void ) :
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
}
