#ifndef IRCSERVER_HPP
# define IRCSERVER_HPP
# include "ListenSocket.hpp"
# include "EpollManager.hpp"
# include "ClientManager.hpp"
# include <string>

# define MAX_EVENTS 10

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
	public:
		IRCServer(std::string ip, int port, std::string passwd);
		void serverLoop( void );
};

#endif
