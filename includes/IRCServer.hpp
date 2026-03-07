#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP
//# include "msgHdler.hpp"
# include "ListenSocket.hpp"
# include "EpollManager.hpp"
# include "ClientManager.hpp"
# include "ChannelManager.hpp"
# include "IServerController.hpp"
# include <string>
# include <map>
# include <vector>
# include <set>

# define MAX_EVENTS 10

typedef struct s_message
{
	std::string prefix;
	std::string command;
	std::vector<std::string> params;
} t_message;

class IRCServer : public IServerController
{
	private:
		std::string m_startStamp;
		std::string m_serverName;
		std::string m_version;
		const std::string m_passwd;
		ListenSocket m_listenSocket;
		ClientManager m_clientManager;
		EpollManager m_epoll;
		std::set<Client *> m_clientsToRemove;
		
		void eventHandler( struct epoll_event &event );
		 void acceptLogics( void );
		 void recvClient( Client &ref );
		  void processLine( Client &client, std::string &line);
		   void dispatch( Client &client, t_message &message );

		void syncDisconnect( Client &client );
		void softDisconnect( Client &client ); // 연결차단 전 메시지를 보낸 경우.
		void hardDisconnect( Client &client ); // 즉시차단 하는 경우.
		void addClientToRemove( Client &client );
		void disconnectClients( void );
		void welcomeMsg( Client &client );

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
			void privmsgProcess(Client &client, std::set<std::string> uniqueTargets, const std::string &msg);
		void	handleNotice(Client& client, const paramVector& params);

		// 3. 채널 조작 (Channel Operations)
		void	handleJoin(Client& client, const paramVector& params);
			void joinProcess(Client &client, paramVector &servers, paramVector &keys);
		void	handlePart(Client& client, const paramVector& params);
			void exitChannels(Client &client, std::vector<std::string> &targets, const std::string &msg);
		void	handleTopic(Client& client, const paramVector& params);
		void	handleNames(Client& client, const paramVector& params);
		void	handleList(Client& client, const paramVector& params);
		void	handleInvite(Client& client, const paramVector& params);
		void	handleKick(Client& client, const paramVector& params);
			void kickProcess(Client &client,
					std::vector<std::string> &channels,
					std::vector<std::string> &targets,
					const std::string &reason);

		void	handleMode(Client& client, const paramVector& params);
			void modeProcess(Client &client,
					const std::string &target,
					const std::string &modes,
					std::vector<std::string> &modeTargets);
			void setChannelMode(Client &client,
					const std::string &target,
					const std::string &modes,
					std::vector<std::string> &modeTargets);
			void setCliientMode(Client &client,
					const std::string &target,
					const std::string &modes,
					std::vector<std::string> &modeTargets);


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
		void	serverAnnounce( void );

	public:
		ChannelManager m_channelManager;
		IRCServer(std::string ip, int port, std::string passwd);
		void serverLoop( void );
};

#endif
