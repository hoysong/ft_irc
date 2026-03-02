#include "IRCServer.hpp"
#include "ircError.hpp"
#include "modeMask.hpp"
#include "msgHdler.hpp"
#include <cctype>
#include <set>
#include <sys/socket.h>
#include <vector>

// tailing 파라미터 split 함수.
extern bool splitUntilChar(std::string &line, std::string seperator, std::string &result);

void splitMultiTarget(std::string str , std::vector<std::string> &vect)
{
	std::cout << "str: " << str << std::endl;
	for( std::string result; splitUntilChar(str, ",", result); )
	{
		std::cout << "str: " << str << std::endl;
		if (!result.empty())
			vect.push_back(result);
	}
	if (!str.empty())
			vect.push_back(str);
}

// ==============================================================================
// 1. 등록 및 인증 관련 (Connection Registration)
// ==============================================================================

void    IRCServer::handlePass(Client& client, const paramVector& params)
{
	if (client.isRegistered())
	{
		sendMsg(client.getFd(), errMsg(ERR_ALREADYREGISTRED, client, "already registered"));
		return ;
	}
	else if (!params.size())
	{
		sendMsg(client.getFd(), notEnoughParam(client, "PASS"));
		return ;
	}
	else if (client.isAuthed())
		return;
	else if (params[0] != m_passwd)
	{
		sendMsg(client.getFd(), errMsg(ERR_PASSWDMISMATCH, client, "Password incorrect"));
		softDisconnect(client, "");
		return ;
	}
	client.setAuthed();
	if (client.getUserName().size()
		&& client.isAuthed()
		&& client.getNickName() != "*"
		)
	{
		client.setRegistered();
		welcomeMsg(client);
	}
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
		sendMsg(client.getFd(), errMsg(ERR_NONICKNAMEGIVEN, client, "No nickname given"));
		return ;
	}
	if ( !isValidNick(params[0]) )
	{
		sendMsg(client.getFd(), errMsg(ERR_ERRONEUSNICKNAME, client, params[0], "Erroneous nickname"));
		return ;
	}
	if ( client.getNickName() == params[0] )
		return ; // 이미 동일하니 무시하기.
	std::string oldNickBuffer = client.getNickName();
	if ( !m_clientManager.setClientNickName(client, params[0]) )
	{
		sendMsg(client.getFd(), errMsg(ERR_NICKNAMEINUSE, client, params[0], "Nickname is already in use"));
		return ;
	}
	/* 변경 성공! */
	// client가 속한 채널에 대해 브로드캐스트 로직.
	//nickChangeBroadcastToChannels(client, oldNickBuffer);
	if (client.getUserName().size()
		&& client.isAuthed()
		&& client.getNickName() != "*"
		)
	{
		if (!client.isRegistered())
			welcomeMsg(client);
		client.setRegistered();
	}
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
	if (client.isRegistered())
	{
		sendMsg(client.getFd(), errMsg(ERR_ALREADYREGISTRED, client, "already registered"));
		return ;
	}
	else if (params.size() < 4)
	{
		sendMsg(client.getFd(), notEnoughParam(client, "USER"));
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
	{
		client.setRegistered();
		welcomeMsg(client);
	}
}

void    IRCServer::handleQuit(Client& client, const paramVector& params)
{
	softDisconnect(client, goodMsg(client, "QUIT", "Disconnected from client"));
}

void    IRCServer::handleOper(Client& client, const paramVector& params)
{
	if (params.size() < 2)
	{
		sendMsg(client.getFd(), notEnoughParam(client, "OPER"));
		return ;
	}
}

// ==============================================================================
// 2. 메시지 전송 (Message Sending)
// ==============================================================================


//| 411 | `ERR_NORECIPIENT` | `:No recipient given (PRIVMSG)` |
//| 412 | `ERR_NOTEXTTOSEND` | `:No text to send` |
//| 413 | `ERR_NOTOPLEVEL` | `<mask> :No toplevel domain specified` |
//| 414 | `ERR_WILDTOPLEVEL` | `<mask> :Wildcard in toplevel domain` |
//| 407 | `ERR_TOOMANYTARGETS` | `<target> :<error code> recipients. <abort message>` |
//| 403 | `ERR_NOSUCHCHANNEL` | `<channel name> :No such channel` |

//| 401 | `ERR_NOSUCHNICK` | `<nickname> :No such nick/channel` |



void IRCServer::privmsgProcess(Client &client, std::set<std::string> targets, const std::string &msg)
{
	std::string current;
	for(std::set<std::string>::iterator iter = targets.begin(); iter != targets.end(); iter++)
	{
		current = *iter;
		std::string line = goodMsg(client, current, "PRIVMSG", msg);
		if (current[0] == '#')
		{ // server
			Channel *channel;
			if (m_channelManager.getChannel(current, channel))
				channel->broadcastPrivmsg(client, line);
			else
			{
				sendMsg(client.getFd(), errMsg( ERR_NOSUCHCHANNEL, client,
							current, "No such channel"));
			}
		}
		else
		{ // client
			m_clientManager.sendPrivmsg(client, current, line);
		}
	}
	return ;
}

void    IRCServer::handlePrivmsg(Client& client, const paramVector& params)
{
	if (params.empty())
	{
		sendMsg(client.getFd(), errMsg(ERR_NORECIPIENT, client, "No recipient given (PRIVMSG)"));
		return ;
	}
	else if (params.size() == 1)
	{
		sendMsg(client.getFd(), errMsg(ERR_NOTEXTTOSEND, client, "No text to send"));
		return ;
	}
	else if (params.size() > 2)
	{
		if (params[1].empty())
		{
			sendMsg(client.getFd(), errMsg(ERR_NOTEXTTOSEND, client, "No text to send"));
			return ;
		}
	}

	/* 타겟 스플릿, 고유한 타겟으로만 정제. */
	std::vector<std::string> targets;
	std::set<std::string> uniqueTargets;
	splitMultiTarget(params[0], targets);
	for(std::vector<std::string>::iterator iter = targets.begin(); iter != targets.end(); iter++)
	{
		if (!targets.empty())
			uniqueTargets.insert(*iter);
	}
	if (targets.size() > 4)
	{
			sendMsg(client.getFd(), errMsg(ERR_TOOMANYTARGETS, client, params[0], "Too many targets (max: 4)"));
			return ;
	}

	privmsgProcess(client, uniqueTargets, params[1]);
}

void    IRCServer::handleNotice(Client& client, const paramVector& params)
{
}

// ==============================================================================
// 3. 채널 조작 (Channel Operations)
// ==============================================================================

void IRCServer::exitChannels( Client &client, std::vector<std::string> &targets, const std::string &msg)
{
	std::vector<std::string>::iterator iter = targets.begin();
	std::vector<std::string>::iterator iter_end = targets.end();
	while (iter != iter_end)
	{
		if (msg.empty())
			m_channelManager.partClientFromChannel(client, *iter, goodMsg(client, "PART", *iter, "Left from channel"));
		else
			m_channelManager.partClientFromChannel(client, *iter, goodMsg(client, "PART", *iter, msg));
		iter++;
	}
}

void    IRCServer::handlePart(Client& client, const paramVector& params)
{
	if (params.empty())
	{ // 파라미터 부족
		sendMsg(client.getFd(), notEnoughParam(client, "PART"));
		return ;
	}
	std::vector<std::string> targets;
	splitMultiTarget(params[0], targets);
	if (targets.empty() && params.size() == 1)
	{ // 파라미터 부족
		sendMsg(client.getFd(), notEnoughParam(client, "PART"));
		return ;
	}
	if (params.size() == 2)
		exitChannels(client, targets, params[1]);
	else
		exitChannels(client, targets, "");
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

void IRCServer::kickProcess(Client &client, std::vector<std::string> &channels, std::vector<std::string> &targets, const std::string &reason)
{
	std::vector<std::string>::iterator channelIter = channels.begin();
	std::vector<std::string>::iterator channelIterEnd = channels.end();
	std::vector<std::string>::iterator targetlIter = targets.begin();
	std::vector<std::string>::iterator targetlIterEnd = targets.end();
	std::string msg;
	std::string trailing;

	/* trailing 설정. */
	if (reason.empty())
		trailing = client.getNickName(); // 비었으면 오퍼닉.
	else
		trailing = reason; // 있으면 그대로.

	if (channels.size() == 1)
		while (targetlIter != targetlIterEnd)
		{
			msg = goodMsg(client, "KICK", *channelIter, *targetlIter, trailing);
			m_channelManager.kickClientFromChannel(client, *channelIter, *targetlIter, msg);
			targetlIter++;
		}
	else
		while (channelIter != channelIterEnd)
		{
			msg = goodMsg(client, "KICK", *channelIter, *targetlIter, trailing);
			m_channelManager.kickClientFromChannel(client, *channelIter, *targetlIter, msg);
			channelIter++;
			targetlIter++;
		}
}
void    IRCServer::handleKick(Client& client, const paramVector& params)
{
	if (params.size() < 2)
	{
		sendMsg(client.getFd(), notEnoughParam(client, "KICK"));
	}
	
	/*서버 멀티타겟 스플릿.*/
	std::vector<std::string> channels;
	splitMultiTarget(params[0], channels);
	if (channels.empty())
	{
		sendMsg(client.getFd(), notEnoughParam(client, "KICK"));
		return ;
	}

	/*유저 멀티타겟 스플릿.*/
	std::vector<std::string> clients;
	splitMultiTarget(params[1], clients);
	if (clients.empty())
	{
		sendMsg(client.getFd(), notEnoughParam(client, "KICK"));
		return ;
	}

	if (channels.size() != 1 && (channels.size() != clients.size()))
	{ // 1대1 매칭인 경우 파라미터 에러 반환
		sendMsg(client.getFd(), notEnoughParam(client, "KICK"));
		return ;
	}
	if (params.size() < 3)
		kickProcess(client, channels, clients, "");
	else
		kickProcess(client, channels, clients, params[2]);
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
