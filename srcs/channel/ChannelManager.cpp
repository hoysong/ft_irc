#include "ChannelManager.hpp"
#include "msgHdler.hpp"
#include <iostream>

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
			return (false); // 클라이언트 추가 실패 (이미 존재 가만히 무시).
	}
	std::cout << "Channel not found" << std::endl;
	/* 채널 못찾음.
	 * 채널 생성, 클라이언트 추가.
	 */
	m_channels[channelName].setChannelName(channelName).assignPasswd(passwd).addMember(client, passwd);
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
	if (iter != m_channels.end())
	{
		if (!(iter->second.removeMember(client, msg)))
		{
			sendMsg(client.getFd(), errMsg(ERR_NOTONCHANNEL, client, iter->first, "You're not on that channel"));
			return (false);
		}
	}
	else if (iter == m_channels.end())
	{
		sendMsg(client.getFd(), errMsg(ERR_NOSUCHCHANNEL, client, iter->first, "No such channel"));
		return (false);
	}
	if (iter->second.isChannelEmpty())
		m_channels.erase(iter);
	return (true);
}

//bool ChannelManager::removeClientFromChannel(
//		Client &client,
//		const std::string &msg,
//		const std::string &channelName,
//		IRCServer &server)
//{
//	std::cout << "[ChannelManager::removeClientFromChannel()]" << std::endl;
//	chanMap::iterator iter = m_channels.find(channelName);
//	if (iter != m_channels.end())
//	{
//		if (iter->second.removeMember(client, msg, server))
//		{
//			if (iter->second.isChannelEmpty())
//				m_channels.erase(iter);
//			return (true);
//		}
//	}
//	return (false); // 채널을 찾지 못함.
//}

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
	std::map<std::string, Channel>::iterator iter_end = m_channels.begin();
	while (iter != iter_end)
	{
		if (iter->second.isChannelEmpty())
		{
			m_channels.erase(iter);
			iter = m_channels.begin();
			iter_end = m_channels.end();
		}
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
	std::cout << "[ChannelManger::ChannelManger()]" << std::endl;
}
ChannelManager::~ChannelManager( void )
{
	std::cout << "[ChannelManger::~ChannelManger()]" << std::endl;
}
