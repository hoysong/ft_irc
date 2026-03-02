#include "Channel.hpp"
#include "IRCServer.hpp"
#include "Client.hpp"
#include <exception>
#include <iostream>
#include <map>
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

void Channel::broadcastMsg( const std::string &msg )
{
	std::map<std::string, Client *>::iterator iter = m_members.begin();
	std::map<std::string, Client *>::iterator iter_end = m_members.end();
	while (iter != iter_end)
	{
		sendMsg(iter->second->getFd(), msg);
		iter++;
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

//| 404 | `ERR_CANNOTSENDTOCHAN` | `<channel name> :Cannot send to channel` |
bool Channel::broadcastPrivmsg(Client &client, const std::string &msg)
{
	if (m_inviteOnly)
	{
		if (!findMember(client))
		{
			sendMsg(client.getFd(), errMsg(ERR_CANNOTSENDTOCHAN, client, m_channelName, "Cannot send to channel (+i)"));
			return (false);
		}
	}

	std::map<std::string, Client *>::iterator iter = this->m_members.begin();
	std::map<std::string, Client *>::iterator iter_end = this->m_members.end();
	while (iter != iter_end)
	{
		sendMsg(iter->second->getFd(), msg);
		iter++;
	}
	return (true);
}

bool Channel::removeMember( const std::string &target, const std::string &msg)
{
	std::map<std::string, Client *>::iterator iter = m_members.find(target);
	if (iter == m_members.end())
		return (false);
	return (removeMember(*(iter->second), msg));
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
void Channel::setKeyMode( const std::string &value )
{
	this->m_passwd = value;
}
void Channel::setLimitMode( int value )
{
	this->m_maxMembers = value;
}
#include <sstream>
#include "MyLibft.hpp"
bool Channel::setLimitMode( const std::string &value )
{
	if (value.size() > 3)
		return (false);
	try {
		m_maxMembers = MyLibft::myAtoi(value);
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (false);
	}
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
