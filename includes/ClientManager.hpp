#ifndef CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP
# include "Client.hpp"
# include <map>
# include <vector>

# define MAX_CLIENT 1000

class ClientManager
{
	private:
		//std::vector<Client *> m_clients;
		std::map<int, Client *> m_fdBased;
		std::map<std::string, Client *> m_nameBased;
	public:
		Client *addNewClient( int fd );
		void removeClient( int fd );

		bool isMaxClient( void );
		bool setClientNickName( int fd, const std::string &name );
//		bool isNickExists( const std::string &nickName ) const;

		ClientManager( void );
		~ClientManager( void );
};

#endif
