#include "ClientManager.hpp"
#include "MyLibft.hpp"
#include <unistd.h> // close().
#include <iostream> // cout cerr.
#include <cerrno>

bool ClientManager::isMaxClient( void )
{
	if (m_fdBased.size() < MAX_CLIENT)
		return (false);
	return (true);
}

/* client에게 닉네임을 최초로 부여하는 함수. */
bool ClientManager::setClientNickName( int fd, const std::string &name )
{
	/* 일단 클라이언트 받아오기. */
	std::map<int, Client *>::iterator fdIter = m_fdBased.find(fd);
	if (fdIter == m_fdBased.end()) // fd 조차 존재하지 않으면.
		throw(std::runtime_error("[ClientManager::setClientNickName]: fd not found."));
	/* 이름 기반 map에 추가하기. */
	std::map<std::string, Client *>::iterator nameIter = m_nameBased.find(name);
	if ( nameIter != m_nameBased.end() )
		return (false); // 이미 존재합니다.
	m_nameBased[name] = fdIter->second;
	fdIter->second->assignNickName( name );
	return (true); // 이름 부여 성공.
}

/* changeClientNick() 함수 필요함!!!! */

bool ClientManager::isNickExists( const std::string &nickName ) const
{
	if (m_nameBased.find(nickName) == m_nameBased.end())
		return (false);
	return (true);
}

/*sendMsg()는 send 실패를 봐야해서 Client가 존재하는지는 해당 함수 호출부에서 검사해야 함.*/
bool ClientManager::sendMsg( int fd, const std::string &msg )
{
	return (MyLibft::sendMsg(fd, msg));
}
bool ClientManager::sendMsg( Client &client, const std::string &msg )
{
	return (sendMsg(client.getFd(), msg));
}


/**********************/
/* add/remove Client. */
/**********************/

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
		m_fdBased[fd] = newClient;
	}
	catch(std::bad_alloc &e)
	{
		std::cerr << "failed m_fdBased[fd](newClient);" << std::endl;
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

	std::map<int, Client *>::iterator iter = m_fdBased.find( fd );
	std::map<std::string, Client *>::iterator nameIter;

	std::string nameBuff;

	/* delete from fdBased map first. */
	if (iter == m_fdBased.end())
		return ;
	else
	{
		nameBuff = iter->second->getNickName();
		delete iter->second;
		m_fdBased.erase(iter);
	}

	/* delete from nameBased map at the end. */
	if (nameBuff == "*")
		return ;
	nameIter = m_nameBased.find(nameBuff);
	if (nameIter != m_nameBased.end())
		m_nameBased.erase(nameIter);
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
	while (m_fdBased.size())
	{
		delete m_fdBased.begin()->second;
		m_fdBased.erase(m_fdBased.begin());
	}
};
