#ifndef CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP
# include "Client.hpp"
# include <map>
# include <vector>

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
		void removeClient( int fd, const std::string &msg, IRCServer &server );

		bool isMaxClient( void );
//		bool setClientNickName( int fd, const std::string &name );
		bool setClientNickName( Client &client, const std::string &name );
		bool isNickExists( const std::string &nickName ) const;

		bool sendMsg( int fd, const std::string &msg );
		bool sendMsg( Client &client, const std::string &msg );
		ClientManager( void );
		~ClientManager( void );
};

#endif
