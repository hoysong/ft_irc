#ifndef CHANNELMANAGER_HPP
# define CHANNELMANAGER_HPP
# include "Channel.hpp"
#include "IServerController.hpp"
# include <map>
# include <string>

class IRCServer;

class ChannelManager
{
	private:
		IServerController &m_server;
		typedef std::map<std::string, Channel> chanMap;
		std::map<std::string, Channel> m_channels;
//		bool addChannel( const std::string &channelName );
	public:
		bool addClientToChannel(
				Client &client,
				const std::string &channelName,
				const std::string &passwd);
		bool partClientFromChannel(
				Client &client,
				const std::string &channelName,
				const std::string &msg);
		bool kickClientFromChannel(
				Client &client,
				const std::string &channelName,
				const std::string &target,
				const std::string &msg);
		/* 존재하는 채널에 대해서만 접근할 수 있도록 만들어주는 함수. */
		bool broadcastToChannel( const std::string &channelName,
				const std::string &msg );
		bool syncNickToChannel( const std::string &channelName,
				Client &client,
				const std::string &newNick);
//		bool removeClientFromChannel(
//				Client &client,
//				const std::string &msg,
//				const std::string &channelName,
//				IRCServer &server);
		bool getChannel( const std::string &name, Channel *&channel );
		bool eraseEmptyChannel( const std::string &channelName );
		void eraseAllEmptyChannels( void );
		bool findChannel( const std::string &channelName );

		ChannelManager( IServerController &ircServer );
		~ChannelManager( void );
		void channelManagerAnnounce( void );

};

#endif
