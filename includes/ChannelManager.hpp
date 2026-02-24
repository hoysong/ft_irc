#ifndef CHANNELMANAGER_HPP
# define CHANNELMANAGER_HPP
# include "Channel.hpp"
# include <map>
# include <string>

class ChannelManger
{
	private:
		std::map<std::string, Channel> m_channels;
	public:
		bool addChannel( const std::string &channelName );
};

#endif
