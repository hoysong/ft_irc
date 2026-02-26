#include "IRCServer.hpp"
#include "modeMask.hpp"
#include <cctype>

// ==============================================================================
// 1. 등록 및 인증 관련 (Connection Registration)
// ==============================================================================

void    IRCServer::handlePass(Client& client, const paramVector& params)
{
	if (!params.size())
	{
		sendNotEnoughParam(client, "PASS");
		return ;
	}
	else if (client.isRegistered())
	{
		msgSender(client, MsgBuilder::buildErrMsg(ERR_ALREADYREGISTRED, client, "already registered"));
		return ;
	}
	else if (client.isAuthed())
		return;
	else if (params[0] != m_passwd)
	{
		msgSender(client, MsgBuilder::buildErrMsg(ERR_PASSWDMISMATCH, client, "Password incorrect"));
		softDisconnect(client);
		return ;
	}
	client.setAuthed();
}

static bool charValiedCheck( char c )
{
	if ( std::isalpha(c) )
		return (true);

	const std::string specials = "[]\\`_^{}|";
	if (specials.find(c) != std::string::npos)
		return (true);
	return (false);
}

static bool isValidNick( const std::string &nick )
{
	if (nick.size() > 9)
		return (false);
	if (!charValiedCheck(nick[0]))
		return (false);

	for (size_t i = 0; nick[i] != '\0'; i++)
	{
		if (charValiedCheck(nick[i]))
			continue ;
		else if (std::isdigit(nick[i]))
			continue ;
		else if (nick[i] == '-')
			continue ;
		else
			return (false);
	}
	return (true);
}

void    IRCServer::handleNick(Client& client, const paramVector& params)
{
	if (!params.size())
	{ // 파라미터 부족.
		msgSender(client, MsgBuilder::buildErrMsg(ERR_NONICKNAMEGIVEN, client, "No nickname given"));
		return ;
	}
	else if (client.isRegistered())
	{
		msgSender(client, MsgBuilder::buildErrMsg(ERR_ALREADYREGISTRED, client, "already registered"));
		return ;
	}
	if ( !isValidNick(params[0]) )
	{
		msgSender(client, MsgBuilder::buildErrMsg(ERR_ERRONEUSNICKNAME, client, params[0], "Erroneous nickname"));
		return ;
	}
	if ( client.getNickName() == params[0] )
		return ; // 이미 동일하니 무시하기.
	if ( !m_clientManager.setClientNickName(client.getFd(), params[0]) )
	{
		msgSender(client, MsgBuilder::buildErrMsg(ERR_NICKNAMEINUSE, client, params[0], "Nickname is already in use"));
		return ;
	}
	/* 변경 성공! */
	// client가 속한 채널에 대해 브로드캐스트 로직.
	if (client.getUserName().size()
		&& client.isAuthed()
		&& client.getNickName() != "*"
		)
		client.setRegistered();
}

static std::string getUserName( const std::string &str )
{
	std::string buffer;

	for (size_t i = 0; str[i] != '\0'; i++)
	{
		if (str[i] >= 33 && str[i] <= 126)
			buffer += str[i];
	}
	return (buffer);
}

static bool isNumeric( const std::string &str )
{
	for (size_t i = 0; str[i] != '\0'; i++)
	{
		if ( !std::isdigit(str[i]) )
			return (false);
	}
	return (true);
}

static void setUserMode( Client &client, const std::string &str )
{
	unsigned int mode = 0;
	if (isNumeric(str))
	{
		mode = *(const_cast<std::string &>(str).rbegin()) - '0';
		std::cout << mode << std::endl;
	}

	if (hasMode(mode, UMODE_INVISIBLE))
		client.setInvisible(true);
	else
		client.setInvisible(false);

	if (hasMode(mode, UMODE_WALLOPS))
		client.setWallops(false);
	else
		client.setWallops(false);
}

void    IRCServer::handleUser(Client& client, const paramVector& params)
{
	if (params.size() < 4)
	{
		sendNotEnoughParam(client, "USER");
		return ;
	}
	else if (client.isRegistered())
	{
		msgSender(client, MsgBuilder::buildErrMsg(ERR_ALREADYREGISTRED, client, "already registered"));
		return ;
	}
	std::string userName = getUserName(params[0]);
	if (userName.empty())
		client.setUserName( "unknownUserName" );
	else
		client.setUserName(userName);
	setUserMode(client, params[1]);
	client.setRealName(params[3]);

	std::cout << "nickname: " << client.getNickName() << std::endl;
	std::cout << "username: " << client.getUserName() << std::endl;
	std::cout << "realname: " << client.getRealName() << std::endl;

	if (client.getUserName().size()
		&& client.isAuthed()
		&& client.getNickName() != "*"
		)
		client.setRegistered();
}

void    IRCServer::handleQuit(Client& client, const paramVector& params)
{
	softDisconnect(client);
}

void    IRCServer::handleOper(Client& client, const paramVector& params)
{
	if (params.size() < 2)
	{
		msgSender(client, MsgBuilder::buildErrMsg(ERR_NEEDMOREPARAMS, client, "OPER", "Not enough parameters"));
		return ;
	}
}

// ==============================================================================
// 2. 메시지 전송 (Message Sending)
// ==============================================================================

void    IRCServer::handlePrivmsg(Client& client, const paramVector& params)
{
}

void    IRCServer::handleNotice(Client& client, const paramVector& params)
{
}

// ==============================================================================
// 3. 채널 조작 (Channel Operations)
// ==============================================================================

void    IRCServer::handlePart(Client& client, const paramVector& params)
{
}

void    IRCServer::handleTopic(Client& client, const paramVector& params)
{
}

void    IRCServer::handleNames(Client& client, const paramVector& params)
{
}

void    IRCServer::handleList(Client& client, const paramVector& params)
{
}

void    IRCServer::handleInvite(Client& client, const paramVector& params)
{
}

void    IRCServer::handleKick(Client& client, const paramVector& params)
{
}

void    IRCServer::handleMode(Client& client, const paramVector& params)
{
}

// ==============================================================================
// 4. 서버 및 유저 정보 (Server Queries & User Info)
// ==============================================================================

void    IRCServer::handleWho(Client& client, const paramVector& params)
{
}

void    IRCServer::handleWhois(Client& client, const paramVector& params)
{
}

void    IRCServer::handleWhowas(Client& client, const paramVector& params)
{
}

void    IRCServer::handlePing(Client& client, const paramVector& params)
{
}

void    IRCServer::handlePong(Client& client, const paramVector& params)
{
}

// ==============================================================================
// 5. 기타 편의/보너스 (Miscellaneous / Optional)
// ==============================================================================

void    IRCServer::handleCap(Client& client, const paramVector& params)
{
}

void    IRCServer::handleAway(Client& client, const paramVector& params)
{
}

void    IRCServer::handleKill(Client& client, const paramVector& params)
{
}
