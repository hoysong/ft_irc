#include "IRCServer.hpp"
#include <signal.h>

/*******************/
/* signal handler. */
/*******************/

bool g_running = true;

static void signalHandler(int signum)
{
	(void)signum;
	std::cout << "\n[signalHandler()]: 종료시그널 수신." << std::endl;
	g_running = false;
}

static void setQuitSignal(void)
{
	std::cout << "\t[setQuitSignal()]: ctrl + c를 종료시그널로 지정합니다." << std::endl;
	if (signal(SIGINT, signalHandler) == SIG_ERR)
		throw std::runtime_error("[setQuitSignal()]: signal() failed.");
}

void IRCServer::acceptLogics( void )
{
	std::cout << "[IRCServer::acceptLogics()]" << std::endl;

	int clientFd = m_listenSocket.acceptClient();
	if ( clientFd < 0 )
	{
		std::cerr << "\tfailed to accept client for unknown reason." << std::endl;
		return ;
	}
	if (m_clientManager.isMaxClient())
	{ // max client 도달. 연결받기, 즉시 close().
		std::cerr << "\tmax client detected." << std::endl;
		close(clientFd);
	}

	Client *newClient = m_clientManager.addNewClient( clientFd );
	if (newClient == NULL)
	{
		std::cerr << "\tfaild to 'new Client'." << std::endl;
		return ;
	}

	if (!m_epoll.add(clientFd, EPOLLIN | EPOLLRDHUP, newClient))
		m_clientManager.removeClient(newClient->getFd());
}

void IRCServer::recvClient( Client &refClient )
{
	std::cout << "[IRCServer::recvClient()]" << std::endl;
	if (!refClient.recvBuffer())
	{
		std::cerr << "\tclient recv() fail." << std::endl;
		m_epoll.del(refClient.getFd());
		m_clientManager.removeClient(refClient.getFd());
	}
}

void IRCServer::eventHandler( struct epoll_event &event )
{
    // 1. 에러나 종료 플래그가 있는지 먼저 확인 (우선순위 높음)
    if (event.events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
    {
        // 종료 처리
        Client *ptr = static_cast<Client *>(event.data.ptr);
        if (ptr) {
            m_epoll.del(ptr->getFd());
            m_clientManager.removeClient(ptr->getFd());
        }
        return; // 이미 닫았으니 recv 안 함
    }

    // 2. 읽기 이벤트 처리
    if (event.events & EPOLLIN)
    {
        if (event.data.ptr == NULL)
            acceptLogics();
        else
            recvClient(*static_cast<Client *>(event.data.ptr));
    }
}

//void IRCServer::eventHandler( struct epoll_event &event )
//{
//	if (event.events & EPOLLIN)
//	{
//		if (event.data.ptr == NULL) // listenSocket인 상황.
//			acceptLogics();
//		else if (event.data.ptr != NULL) // Client인 상황.
//			recvClient(*static_cast<Client *>(event.data.ptr));
//	}
//	else if (
//			//event.events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)
//			event.events & (EPOLLHUP | EPOLLERR)
//		)
//	{
//		if (event.data.ptr != NULL)
//		{ // Client측 에러로 추정.
//			Client *ptr
//				= static_cast<Client *>(event.data.ptr);
//			m_epoll.del(ptr->getFd());
//			m_clientManager.removeClient(ptr->getFd());
//		}
//	}
//}

/****************/
/* server loop. */
/****************/
void IRCServer::serverLoop( void )
{
	struct epoll_event events[MAX_EVENTS];
	int eventCount = 0;

	while (g_running)
	{
		std::cout << "[IRCServer::serverLoop()]: 새로운 루프입니다." << std::endl;
		eventCount = m_epoll.wait(events, MAX_EVENTS, -1);
		for(int i = 0; i < eventCount; i++)
		{
			eventHandler(events[i]);
		}
	}
}

/**************************/
/* IRCServer initializer. */
/**************************/
IRCServer::IRCServer(std::string ip, int port, std::string passwd) :
	m_passwd(passwd),
	m_listenSocket(ip, port)
{
	std::cout << "[IRCServer::IRCServer()]" << std::endl;
	setQuitSignal();
	m_epoll.add(m_listenSocket.getFd(), EPOLLIN, NULL);
	std::cout << "\tListenSocket을 epoll에 등록하였습니다." << std::endl;
}

