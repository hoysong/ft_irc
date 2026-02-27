#include "ClientManager.hpp"
#include "Channel.hpp"
#include "MyLibft.hpp"
#include "IRCServer.hpp"
#include <unistd.h> // close().
#include <iostream> // cout cerr.
#include <cerrno>

bool ClientManager::isMaxClient( void )
{
	if (m_fdBased.size() < MAX_CLIENT)
		return (false);
	return (true);
}

///* client에게 닉네임을 최초로 부여하는 함수. */
//bool ClientManager::setClientNickName( int fd, const std::string &name )
//{
//	/* 일단 클라이언트 받아오기. */
//	std::map<int, Client *>::iterator fdIter = m_fdBased.find(fd);
//	if (fdIter == m_fdBased.end()) // fd 조차 존재하지 않으면.
//		throw(std::runtime_error("[ClientManager::setClientNickName]: fd not found."));
//	/* 이름 기반 map에 추가하기. */
//	std::map<std::string, Client *>::iterator nameIter = m_nameBased.find(name);
//	if ( nameIter != m_nameBased.end() )
//		return (false); // 이미 존재합니다.
//	m_nameBased[name] = fdIter->second;
//	fdIter->second->assignNickName( name );
//	return (true); // 이름 부여 성공.
//}

bool ClientManager::setClientNickName( Client &client, const std::string &name )
{
	std::map<int, Client *>::iterator fdIter = m_fdBased.find(client.getFd());
	if (fdIter == m_fdBased.end())
		throw(std::runtime_error("[ClientManager::setClientNickName]: client not found."));

	std::map<std::string, Client *>::iterator nameIter = m_nameBased.find(name);
	if (nameIter != m_nameBased.end())
		return (false); // 이미 존재하는 이름입니다.

	/*m_nameBased에 이미 해당 클라이언트가 있는지?*/
	nameIter = m_nameBased.find(client.getNickName());
	if (nameIter != m_nameBased.end())
	{ // 이미 이름기반 map에 존재함.
		m_nameBased.erase(nameIter);
	}
	client.assignNickName(name);
	m_nameBased[name] = &client;

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

void ClientManager::removeClient( int fd, const std::string &msg )
{
	std::cout << "[ClientManager::removeClient()]" << std::endl;

	std::map<int, Client *>::iterator iter = m_fdBased.find( fd );
	std::map<std::string, Client *>::iterator nameIter;

	std::string nameBuff;

	/* disconnect from Channel. */
	Client &client = *(iter->second);
	/*얘는 여기 담는게 맞음. 컨테이너 복사생성으로 돌려야 없애면서 나갈 수 있음.*/
	std::map<std::string, Channel *> joined = client.getJoinedChannel();
	for(std::map<std::string, Channel *>::iterator iter = joined.begin(); iter != joined.end(); iter++)
	{
		iter->second->removeMember(client, msg);
	}

	/* delete from fdBased map. */
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
