#include "ClientManager.hpp"
#include <unistd.h> // close().
#include <iostream> // cout cerr.

Client *ClientManager::addNewClient( int fd )
{
	std::cout << "\t[ClientManager::addNewClient()]" << std::endl;
	Client *newClient = NULL;
	try
	{
		newClient = new Client(fd);
	}
	catch(std::bad_alloc &e)
	{
		std::cerr << e.what() << std::endl;
		close(fd);
		return (NULL);
	}
	try
	{
		m_clients.push_back(newClient);
	}
	catch(std::bad_alloc &e)
	{
		std::cerr << "failed m_clients.push_back(newClient);" << std::endl;
		std::cerr << e.what() << std::endl;
		delete newClient;
		return (NULL);
	}
	std::cout << "\tnew client added: fd " << fd << std::endl;
	return (newClient);
};
void ClientManager::removeClient( int fd )
{
	std::cout << "[ClientManager::removeClient()]" << std::endl;
	std::vector<Client *>::iterator iter = m_clients.begin();
	std::vector<Client *>::iterator iter_end = m_clients.end();
	while (iter != iter_end)
	{
		if ((*iter)->getFd() == fd)
		{
			delete *iter;
			m_clients.erase(iter);
			return ;
		}
		iter++;
	}
}

bool ClientManager::isMaxClient( void )
{
	if (m_clients.size() < MAX_CLIENT)
		return (false);
	return (true);
}

/***************************/
/* constructor/destructor. */
/***************************/
ClientManager::ClientManager( void )
{
};
ClientManager::~ClientManager( void )
{
	std::cout << "[ClientManager::~ClientManager()]" << std::endl;
	while (m_clients.size())
	{
		delete m_clients.back();
		m_clients.pop_back();
	}
};
