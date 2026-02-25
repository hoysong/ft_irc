#ifndef CHANNELMANAGER_HPP
# define CHANNELMANAGER_HPP
# include "Channel.hpp"
# include <map>
# include <string>

class ChannelManager
{
	private:
		typedef std::map<std::string, Channel> chanMap;
		std::map<std::string, Channel> m_channels;
//		bool addChannel( const std::string &channelName );
	public:
		bool addClientToChannel(
				Client &client,
				const std::string &channelName);
		bool removeClientFromChannel(
				Client &client,
				const std::string &channelName);
		bool getChannel( const std::string &name, Channel *&channel );

		ChannelManager( void );
		~ChannelManager( void );
};

#endif
