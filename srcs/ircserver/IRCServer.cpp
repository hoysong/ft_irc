#include "IRCServer.hpp"
#include <ios>
#include <signal.h>
#include "MyLibft.hpp"

// =========================================================================
// signal handler.
// =========================================================================

#define CYAN "\033[46m"
#define BOLDGREEN "\033[1;32m"
#define NC "\033[0m"

void IRCServer::serverAnnounce( void )
{
	std::cout << BOLDGREEN << "serverAnnounce" << NC << std::endl;
	std::cout << std::boolalpha;
	m_clientManager.clientManagerAnnounce();
	m_channelManager.channelManagerAnnounce();
	std::cout << std::noboolalpha;
}

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

// =========================================================================
// loop logics.
// =========================================================================

#include <sys/socket.h>
#include <sys/time.h>

static bool setSendTimeOut( int fd )
{
	struct timeval	timeout;
	
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	
	if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
		return (false);
	return (true);
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
//	if (m_clientManager.isMaxClient())
//	{ // max client 도달. 연결받기, 즉시 close().
//		MyLibft::setLingerZero(clientFd);
//		std::cerr << "\tmax client detected." << std::endl;
//		close(clientFd);
//	}

	Client *newClient = m_clientManager.addNewClient( clientFd );
	if (newClient == NULL)
	{
		std::cerr << "\tfaild to 'new Client'." << std::endl;
		return ;
	}

	if (!m_epoll.add(clientFd, EPOLLIN | EPOLLRDHUP, newClient))
		m_clientManager.removeClient(newClient->getFd(), "");
}

/* 클라이언트 이벤트. */
void IRCServer::recvClient( Client &refClient )
{
	std::cout << "[IRCServer::recvClient()]" << std::endl;
	if (!refClient.recvFd())
	{ // client recv() 실패.
		std::cerr << "\tclient recv() fail." << std::endl;
		hardDisconnect(refClient, goodMsg(refClient, "QUIT", "Disconnected by server"));
		return ;
	}

	while (true)
	{
		std::string line;
		if (!refClient.popLine(line))
		{ // line not ready. nothing to do.
			break;
		}
		processLine(refClient, line); // 라인에 따른 적절한 처리.
		line.clear();
	}
}

void IRCServer::eventHandler( struct epoll_event &event )
{
	if (event.events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
	{ // 종료 처리
		Client *ptr = static_cast<Client *>(event.data.ptr);
		if (ptr)
			hardDisconnect(*ptr, goodMsg(*ptr, "QUIT", "Disconnected by unknown reason"));
		return ;
	}
	
	if (event.events & EPOLLIN)
	{
		if (event.data.ptr == NULL)
			acceptLogics();
		else
			recvClient(*static_cast<Client *>(event.data.ptr));
	}
}

// =========================================================================
// disconnect client.
// =========================================================================

void IRCServer::softDisconnect( Client &client, const std::string &msg )
{
	std::cout << "[softDisconnect]" << std::endl;
	m_epoll.del(client.getFd());
	m_clientManager.removeClient(client.getFd(), msg);
	m_channelManager.eraseAllEmptyChannels();
	std::cout << "\t[softDisconnect] END" << std::endl;
}
void IRCServer::hardDisconnect( Client &client, const std::string &msg )
{
	std::cout << "[hardDisconnect]" << std::endl;
	MyLibft::setLingerZero(client.getFd());
	m_epoll.del(client.getFd());
	m_clientManager.removeClient(client.getFd(), msg);
	m_channelManager.eraseAllEmptyChannels();
	std::cout << "\t[hardDisconnect] END" << std::endl;
}

// =========================================================================
// server loop.
// =========================================================================
void IRCServer::serverLoop( void )
{
	struct epoll_event events[MAX_EVENTS];
	int eventCount = 0;

	while (g_running)
	{
		std::cout << CYAN 
			"[IRCServer::serverLoop()]: 새로운 루프입니다." << 
			NC <<
		std::endl;
		eventCount = m_epoll.wait(events, MAX_EVENTS, -1);
		for(int i = 0; i < eventCount; i++)
		{
			eventHandler(events[i]);
			serverAnnounce();
		}
	}
}

/**************************/
/* IRCServer initializer. */
/**************************/

#include <ctime>
#include <string>

std::string makeStartStamp( void )
{
    std::time_t now = std::time(NULL);          // 또는 서버 시작 시각을 저장해둔 값
    std::tm *lt = std::localtime(&now);

    char buf[128];
    std::strftime(buf, sizeof(buf),
                  "%a %b %d %Y at %H:%M:%S %Z",
                  lt);

    return std::string(buf);
}


IRCServer::IRCServer(std::string ip, int port, std::string passwd) :
	m_serverName("irc.ft_irc.42Gyeongsan.kr"),
	m_version("ft_irc-0.5"),
	m_passwd(passwd),
	m_listenSocket(ip, port)
{
	std::cout << "[IRCServer::IRCServer()]" << std::endl;
	m_startStamp = makeStartStamp();
	setQuitSignal();
	m_epoll.add(m_listenSocket.getFd(), EPOLLIN, NULL);
	std::cout << "\tListenSocket을 epoll에 등록하였습니다." << std::endl;

	// 1. 등록 및 인증 관련 (Connection Registration)
	m_commands["PASS"]    = &IRCServer::handlePass;
	m_commands["NICK"]    = &IRCServer::handleNick;
	m_commands["USER"]    = &IRCServer::handleUser;
	m_commands["QUIT"]    = &IRCServer::handleQuit;
	m_commands["OPER"]    = &IRCServer::handleOper;

	// 2. 메시지 전송 (Message Sending)
	m_commands["PRIVMSG"] = &IRCServer::handlePrivmsg;
	m_commands["NOTICE"]  = &IRCServer::handleNotice;

	// 3. 채널 조작 (Channel Operations)
	m_commands["JOIN"]    = &IRCServer::handleJoin;
	m_commands["PART"]    = &IRCServer::handlePart;
	m_commands["TOPIC"]   = &IRCServer::handleTopic;
	m_commands["NAMES"]   = &IRCServer::handleNames;
	m_commands["LIST"]    = &IRCServer::handleList;
	m_commands["INVITE"]  = &IRCServer::handleInvite;
	m_commands["KICK"]    = &IRCServer::handleKick;
	m_commands["MODE"]    = &IRCServer::handleMode;

	// 4. 서버 및 유저 정보 (IRCServer Queries & User Info)
	m_commands["WHO"]     = &IRCServer::handleWho;
	m_commands["WHOIS"]   = &IRCServer::handleWhois;
	m_commands["WHOWAS"]  = &IRCServer::handleWhowas;
	m_commands["PING"]    = &IRCServer::handlePing;
	m_commands["PONG"]    = &IRCServer::handlePong;

	// 5. 기타 편의/보너스 (Miscellaneous / Optional)
	m_commands["CAP"]     = &IRCServer::handleCap;     // irssi 접속 대응
	m_commands["AWAY"]    = &IRCServer::handleAway;
	m_commands["KILL"]    = &IRCServer::handleKill;    // Oper 전용 강퇴
}
