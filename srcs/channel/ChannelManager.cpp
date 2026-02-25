#include "ChannelManager.hpp"
#include <iostream>

//bool ChannelManager::addChannel( const std::string &channelName )
//{
//}

bool ChannelManager::addClientToChannel(
		Client &client,
		const std::string &channelName)
{
	std::cout << "[ChannelManager::addClientToChannel()]" << std::endl;
	chanMap::iterator iter = m_channels.find(channelName);
	if (iter != m_channels.end())
	{ // 찾은 채널 클라이언트 추가.
		std::cout << "Channel Found!" << std::endl;
		if (!(iter->second.addMember(client)))
			return (false); // 클라이언트 추가 실패 (이미 존재).
	}
	std::cout << "Channel not found" << std::endl;
	/* 채널 못찾음.
	 * 채널 생성, 클라이언트 추가.
	 */
	m_channels[channelName].setChannelName(channelName).addMember(client);
	std::cout << "\tadd client to channel done." << std::endl;
	return (true);
}

bool ChannelManager::removeClientFromChannel( Client &client, const std::string &channelName)
{
	std::cout << "[ChannelManager::removeClientFromChannel()]" << std::endl;
	chanMap::iterator iter = m_channels.find(channelName);
	if (iter != m_channels.end())
	{
		if (iter->second.removeMember(client))
		{
			if (iter->second.isChannelEmpty())
				m_channels.erase(iter);
			return (true);
		}
	}
	return (false); // 채널을 찾지 못함.
}

bool ChannelManager::getChannel( const std::string &name, Channel *&channel )
{
	std::map<std::string, Channel>::iterator iter = m_channels.find(name);
	if (iter == m_channels.end())
		return (false);
	channel = &(iter->second);
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
