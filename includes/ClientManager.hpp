#ifndef CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP
# include "Client.hpp"
# include <map>

# define MAX_CLIENT 1000

class IRCServer;

class ClientManager
{
	private:
		//std::vector<Client *> m_clients;
		std::map<int, Client *> m_fdBased;
		std::map<std::string , Client *> m_nameBased;
	public:
		Client *addNewClient( int fd );
		void removeClient( int fd, const std::string &msg );
		Client &getClient( const std::string &nickName );

		bool isMaxClient( void );
//		bool setClientNickName( int fd, const std::string &name );
		bool setClientNickName( Client &client, const std::string &name );
		bool isNickExists( const std::string &nickName ) const;

		bool sendPrivmsg(Client &client, const std::string &name, const std::string &msg);

//		bool sendMsg( int fd, const std::string &msg );
//		bool sendMsg( Client &client, const std::string &msg );
		ClientManager( void );
		~ClientManager( void );
		void clientManagerAnnounce( void );
};

#endif
