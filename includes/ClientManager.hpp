#ifndef CLIENTMANAGER_HPP
# define CLIENTMANAGER_HPP
# include "Client.hpp"
# include <vector>

# define MAX_CLIENT 1000

class ClientManager
{
	private:
		std::vector<Client *> m_clients;
	public:
		Client *addNewClient( int fd );
		void removeClient( int fd );
		bool isMaxClient( void );

		ClientManager( void );
		~ClientManager( void );
};

#endif
