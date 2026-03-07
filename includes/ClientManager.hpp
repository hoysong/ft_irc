#ifndef CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP
# include "IServerController.hpp"
# include "Client.hpp"
# include <map>

# define MAX_CLIENT 1000

class IRCServer;

class ClientManager
{
	private:
		IServerController &m_server;
		std::map<int, Client *> m_fdBased;
		std::map<std::string , Client *> m_nameBased;
	public:
		Client *addNewClient( int fd );
		void removeClient( int fd );
		Client &getClient( const std::string &nickName );

		bool isMaxClient( void );
		bool setClientNickName( Client &client, const std::string &name );
		bool isNickExists( const std::string &nickName ) const;

		bool sendPrivmsg(Client &client, const std::string &name, const std::string &msg);

		ClientManager( IServerController &ircServer );
		~ClientManager( void );
		void clientManagerAnnounce( void );
};

#endif
