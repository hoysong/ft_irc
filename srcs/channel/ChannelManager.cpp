#include "ChannelManager.hpp"
#include "Channel.hpp"
#include "ircError.hpp"
#include "Msg.hpp"
#include <iostream>

void ChannelManager::channelManagerAnnounce( void )
{
	std::cout << "==================================================" << std::endl;
	std::cout << "ChannelManager" << std::endl;
	std::cout << "==================================================" << std::endl;
	std::map<std::string, Channel>::iterator iter = m_channels.begin();
	std::map<std::string, Channel>::iterator iter_end = m_channels.end();
	while (iter != iter_end)
	{
		iter->second.announce();
		iter++;
	}
}

bool ChannelManager::addClientToChannel(
		Client &client,
		const std::string &channelName,
		const std::string &passwd)
{
	std::cout << "[ChannelManager::addClientToChannel()]" << std::endl;
	chanMap::iterator iter = m_channels.find(channelName);
	if (iter != m_channels.end())
	{ // 찾은 채널 클라이언트 추가.
		std::cout << "Channel Found!" << std::endl;
		if (!(iter->second.addMember(client, passwd)))
			return (false); // 추가 실패.
		else
			return (true); // 추가 성공.
	}
	std::cout << "Channel not found" << std::endl;
	/* 채널 못찾음.
	 * 채널 생성, 클라이언트 추가.
	 */
	m_channels[channelName].setChannelName(channelName).assignPasswd(passwd).addMember(client, passwd);
	m_channels[channelName].addChannelOper(client);
	std::cout << "\tadd client to channel done." << std::endl;
	return (true);
}

bool ChannelManager::partClientFromChannel(
		Client &client,
		const std::string &channelName,
		const std::string &msg)
{
	std::cout << "[ChannelManager::partClientFromChannel()]" << std::endl;
	chanMap::iterator iter = m_channels.find(channelName);
	if (iter == m_channels.end())
	{
//		sendMsg(client.getFd(),
//				errMsg(
//					ERR_NOSUCHCHANNEL,
//					client,
//					iter->first,
//					"No such channel"));
		Msg().errNoSuchChannel(client.getNickName(), iter->first).sendTo(client.getFd());
		return (false);
	}

	if (!(iter->second.removeMember(client, msg)))
	{
//		sendMsg(client.getFd(),
//				errMsg(
//					ERR_NOTONCHANNEL,
//					client,
//					iter->first,"You're not on that channel"));
		Msg().errNotOnChannel(client.getNickName(), iter->first).sendTo(client.getFd());
		return (false);
	}

	if (iter->second.isChannelEmpty())
		m_channels.erase(iter);
	return (true);
}

bool ChannelManager::kickClientFromChannel(
		Client &client,
		const std::string &channelName,
		const std::string &target,
		const std::string &msg)
{
	std::map<std::string, Channel>::iterator iter = m_channels.find(channelName);
	if (iter == m_channels.end())
	{
//		sendMsg(client.getFd(), errMsg(
//					ERR_NOSUCHCHANNEL,
//					client,
//					channelName,
//					"No such Channel"));
		Msg().errNoSuchChannel(client.getNickName(), channelName).sendTo(client.getFd());
		return (false);
	}

	if (!iter->second.findMember(target))
	{
//		sendMsg(client.getFd(),
//				errMsg(
//					ERR_USERNOTINCHANNEL,
//					client,
//					target,
//					channelName,
//					"They aren't on that channel"));
		Msg()
			.setPrefix(SERVER_PREFIX)
			.addParam(client.getNickName())
			.addParam(target)
			.addParam(channelName)
			.addParam("They aren't on that channel")
			.sendTo(client.getFd());
		return (false);
	}

	if (!iter->second.isChannelOper(client))
	{
//		sendMsg(client.getFd(),
//				errMsg(
//					ERR_CHANOPRIVSNEEDED,
//					client,
//					channelName,
//					"You're not channel operator"));
		Msg().errChanOpPrivsNeeded(client.getNickName(), channelName).sendTo(client.getFd());
		return (false);
	}

	iter->second.removeMember(target, msg);

	return (true);
}

bool ChannelManager::getChannel( const std::string &name, Channel *&channel )
{
	std::map<std::string, Channel>::iterator iter = m_channels.find(name);
	if (iter == m_channels.end())
		return (false);
	channel = &(iter->second);
	return (true);
}

bool ChannelManager::eraseEmptyChannel( const std::string &channelName )
{
	std::map<std::string, Channel>::iterator iter = m_channels.find(channelName);
	if (iter == m_channels.end())
		return (false);
	m_channels.erase(iter);
	return (true);
}

void ChannelManager::eraseAllEmptyChannels( void )
{
	std::map<std::string, Channel>::iterator iter = m_channels.begin();
	std::map<std::string, Channel>::iterator iter_end = m_channels.end();
	while (iter != iter_end)
	{
		if (iter->second.isChannelEmpty())
		{
			m_channels.erase(iter);
			iter = m_channels.begin();
			iter_end = m_channels.end();
		}
		else
			iter++;
	}
}

bool ChannelManager::findChannel( const std::string &channelName )
{
	if (m_channels.find(channelName) == m_channels.end())
		return (false);
	return (true);
}

ChannelManager::ChannelManager( void )
{
	std::cout << "[ChannelManager::ChannelManager()]" << std::endl;
}
ChannelManager::~ChannelManager( void )
{
	std::cout << "[ChannelManager::~ChannelManager()]" << std::endl;
}
