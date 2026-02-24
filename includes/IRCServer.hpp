#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP
# include "ListenSocket.hpp"
# include "EpollManager.hpp"
# include "ClientManager.hpp"
# include "MsgBuilder.hpp"
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
		const std::string m_passwd;
		ListenSocket m_listenSocket;
		ClientManager m_clientManager;
		EpollManager m_epoll;
		
		void eventHandler( struct epoll_event &event );
		 void acceptLogics( void );
		 void recvClient( Client &ref );
		  void processLine( Client &client, std::string &line);
		   void dispatch( Client &client, t_message &message );
		void softDisconnect( Client &client ); // 연결차단 전 메시지를 보낸 경우.
		void hardDisconnect( Client &client ); // 즉시차단 하는 경우.

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
};

#endif
