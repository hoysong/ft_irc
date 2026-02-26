#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP
# include "ListenSocket.hpp"
# include "EpollManager.hpp"
# include "ClientManager.hpp"
# include "ChannelManager.hpp"
# include "MsgBuilder.hpp"
# include "MyLibft.hpp"
# include <string>
# include <map>

# define MAX_EVENTS 10

typedef struct s_message
{
	std::string prefix;
	std::string command;
	std::vector<std::string> params;
} t_message;

class IRCServer
{
	private:
		std::string m_startStamp;
		std::string m_serverName;
		const std::string m_passwd;
		ListenSocket m_listenSocket;
		ClientManager m_clientManager;
		ChannelManager m_channelManager;
		EpollManager m_epoll;
		
		void eventHandler( struct epoll_event &event );
		 void acceptLogics( void );
		 void recvClient( Client &ref );
		  void processLine( Client &client, std::string &line);
		   void dispatch( Client &client, t_message &message );

		void softDisconnect( Client &client, const std::string &msg ); // 연결차단 전 메시지를 보낸 경우.
		void hardDisconnect( Client &client, const std::string &msg ); // 즉시차단 하는 경우.

		void welcomeMsg( Client &client );
		/* 단독호출 금지. nickChangeBroadcastToChannels()을 대신 사용.*/
		void nickChangeBroadcast(Client &client, Channel &channel, const std::string &oldNick);
		void nickChangeBroadcastToChannels( Client &client, const std::string  &oldNick);
		/* 단독호출 금지. quitBroadcastToChannels()을 대신 사용.*/
		void quitBroadcastToChannels( Client &client, const std::string &msg);
		void sendNotEnoughParam( Client &client, const std::string &cmd );

		/*************/
		/* dispatch. */
		/*************/

		typedef std::vector<std::string> paramVector;
		typedef void (IRCServer::*commandHandler)(Client&, const paramVector&);
		std::map<std::string, commandHandler> m_commands;

		// 1. 등록 및 인증 관련 (Connection Registration)
		void	handlePass(Client& client, const paramVector& params);
		void	handleNick(Client& client, const paramVector& params);
		void	handleUser(Client& client, const paramVector& params);
		void	handleQuit(Client& client, const paramVector& params);
		void	handleOper(Client& client, const paramVector& params);

		// 2. 메시지 전송 (Message Sending)
		void	handlePrivmsg(Client& client, const paramVector& params);
		void	handleNotice(Client& client, const paramVector& params);

		// 3. 채널 조작 (Channel Operations)
		void	handleJoin(Client& client, const paramVector& params);
//		void	aboutExistChannel(Client &client,
//				Channel &channel,
//				std::vector<std::string> &servers,
//				std::vector<std::string> &keys,
//				std::vector<std::string>::iterator &servIter,
//				std::vector<std::string>::iterator &keyIter
//				);
		void	joinProcess(Client &client, paramVector &servers, paramVector &keys);
		void	handlePart(Client& client, const paramVector& params);
		void	handleTopic(Client& client, const paramVector& params);
		void	handleNames(Client& client, const paramVector& params);
		void	handleList(Client& client, const paramVector& params);
		void	handleInvite(Client& client, const paramVector& params);
		void	handleKick(Client& client, const paramVector& params);
		void	handleMode(Client& client, const paramVector& params);

		// 4. 서버 및 유저 정보 (Server Queries & User Info)
		void	handleWho(Client& client, const paramVector& params);
		void	handleWhois(Client& client, const paramVector& params);
		void	handleWhowas(Client& client, const paramVector& params);
		void	handlePing(Client& client, const paramVector& params);
		void	handlePong(Client& client, const paramVector& params);

		// 5. 기타 편의/보너스 (Miscellaneous / Optional)
		void	handleCap(Client& client, const paramVector& params);
		void	handleAway(Client& client, const paramVector& params);
		void	handleKill(Client& client, const paramVector& params);

	public:
		IRCServer(std::string ip, int port, std::string passwd);
		void serverLoop( void );

		void msgSender( Client &client, const std::string &msg );
		/* use in Channel class. */
		void welcomeBroadcast( Client &client, Channel &channel );
		void quitBroadcast( Client &client, Channel &channel , const std::string &msg );
};

#endif
