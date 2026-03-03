#include "IRCServer.hpp"
#include "Msg.hpp"
#include "ircError.hpp"
#include "modeMask.hpp"
#include "msgHdler.hpp"
#include <cctype>
#include <set>
#include <sys/socket.h>
#include <vector>
#include "ircError.hpp"

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
//		sendMsg(client.getFd(), errMsg(ERR_ALREADYREGISTRED, client, "already registered"));
		Msg().errAlreadyRegist(client.getNickName()).sendTo(client.getFd());
		return ;
	}
	else if (!params.size())
	{
//		sendMsg(client.getFd(), notEnoughParam(client, "PASS"));
		Msg().errNotEnoughParam(client.getNickName(), "PASS").sendTo(client.getFd());
		return ;
	}
	else if (client.isAuthed())
		return;
	else if (params[0] != m_passwd)
	{
//		sendMsg(client.getFd(), errMsg(ERR_PASSWDMISMATCH, client, "Password incorrect"));
		Msg().errPasswdMismatch(client.getNickName()).sendTo(client.getFd());
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
//		sendMsg(client.getFd(), errMsg(ERR_NONICKNAMEGIVEN, client, "No nickname given"));
		Msg().errNoNickGiven(client.getNickName()).sendTo(client.getFd());
		return ;
	}
	if ( !isValidNick(params[0]) )
	{
//		sendMsg(client.getFd(), errMsg(ERR_ERRONEUSNICKNAME, client, params[0], "Erroneous nickname"));
		Msg().errOneousNick(client.getNickName(), params[0]).sendTo(client.getFd());
		return ;
	}
	if ( client.getNickName() == params[0] )
		return ; // 이미 동일하니 무시하기.
	std::string oldNickBuffer = client.getNickName();
	if ( !m_clientManager.setClientNickName(client, params[0]) )
	{
//		sendMsg(client.getFd(), errMsg(ERR_NICKNAMEINUSE, client, params[0], "Nickname is already in use"));
		Msg().errNickInUse(client.getNickName(), params[0]).sendTo(client.getFd());
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
//		sendMsg(client.getFd(), errMsg(ERR_ALREADYREGISTRED, client, "already registered"));
		Msg().errAlreadyRegist(client.getNickName()).sendTo(client.getFd());
		return ;
	}
	else if (params.size() < 4)
	{
//		sendMsg(client.getFd(), notEnoughParam(client, "USER"));
		Msg().errNotEnoughParam(client.getNickName(), "USER").sendTo(client.getFd());
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
	softDisconnect(client,
		//	goodMsg(client, "QUIT", "Disconnected from client")
			Msg()
			.setPrefix(client.getMsgPrefix())
			.addParam("QUIT")
			.addParam("Disconnected from server")
			.serialize()
			);
}

void    IRCServer::handleOper(Client& client, const paramVector& params)
{
	if (params.size() < 2)
	{
		Msg().errNotEnoughParam(client.getNickName(), "OPER").sendTo(client.getFd());
		return ;
	}
}

// ==============================================================================
// 2. 메시지 전송 (Message Sending)
// ==============================================================================

void IRCServer::privmsgProcess(Client &client, std::set<std::string> targets, const std::string &msg)
{
	std::string current;
	for(std::set<std::string>::iterator iter = targets.begin(); iter != targets.end(); iter++)
	{
		current = *iter;
		//goodMsg(client, current, "PRIVMSG", msg);
		std::string line = Msg()
			.setPrefix(client.getMsgPrefix())
			.addParam(current)
			.addParam("PRIVMSG")
			.addParam(msg)
			.serialize();
		if (current[0] == '#')
		{ // server
			Channel *channel;
			if (m_channelManager.getChannel(current, channel))
				channel->broadcastPrivmsg(client, line);
			else
			{
//				sendMsg(client.getFd(), errMsg( ERR_NOSUCHCHANNEL, client,
//							current, "No such channel"));
				Msg().errNoSuchChannel(client.getNickName(), current).sendTo(client.getFd());
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
//		sendMsg(client.getFd(), errMsg(ERR_NORECIPIENT, client, "No recipient given (PRIVMSG)"));
		Msg()
			.setPrefix(SERVER_PREFIX)
			.numeric(ERR_NORECIPIENT)
			.addParam(client.getNickName())
			.addParam("No recipient given (PRIVMSG)")
			.sendTo(client.getFd());
		return ;
	}
	else if (params.size() == 1)
	{
//		sendMsg(client.getFd(), errMsg(ERR_NOTEXTTOSEND, client, "No text to send"));
		Msg()
			.setPrefix(SERVER_PREFIX)
			.numeric(ERR_NOTEXTTOSEND)
			.addParam(client.getNickName())
			.addParam("No text to send")
			.sendTo(client.getFd());
		return ;
	}
	else if (params.size() > 2)
	{
		if (params[1].empty())
		{
//			sendMsg(client.getFd(), errMsg(ERR_NOTEXTTOSEND, client, "No text to send"));
			Msg()
				.setPrefix(SERVER_PREFIX)
				.numeric(ERR_NOTEXTTOSEND)
				.addParam(client.getNickName())
				.addParam("No text to send")
				.sendTo(client.getFd());
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
//			sendMsg(client.getFd(), errMsg(ERR_TOOMANYTARGETS, client, params[0], "Too many targets (max: 4)"));
			Msg()
				.setPrefix(SERVER_PREFIX)
				.numeric(ERR_TOOMANYTARGETS)
				.addParam(client.getNickName())
				.addParam(params[0])
				.addParam("Too many targets (max: 4)")
				.sendTo(client.getFd());
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
			m_channelManager.partClientFromChannel(client, *iter, 
					Msg()
					.setPrefix(client.getMsgPrefix())
					.addParam(client.getNickName())
					.addParam("PART")
					.addParam(*iter)
					.addParam("Left from channel")
					.serialize()
					);
		else
			m_channelManager.partClientFromChannel(client, *iter, 
					Msg()
					.setPrefix(client.getMsgPrefix())
					.addParam(client.getNickName())
					.addParam("PART")
					.addParam(*iter)
					.addParam(msg)
					.serialize()
					);
		iter++;
	}
}

void    IRCServer::handlePart(Client& client, const paramVector& params)
{
	if (params.empty())
	{ // 파라미터 부족
//		sendMsg(client.getFd(), notEnoughParam(client, "PART"));
		Msg().errNotEnoughParam(client.getNickName(), "PART").sendTo(client.getFd());
		return ;
	}
	std::vector<std::string> targets;
	splitMultiTarget(params[0], targets);
	if (targets.empty() && params.size() == 1)
	{ // 파라미터 부족
//		sendMsg(client.getFd(), notEnoughParam(client, "PART"));
		Msg().errNotEnoughParam(client.getNickName(), "PART").sendTo(client.getFd());
		return ;
	}
	if (params.size() == 2)
		exitChannels(client, targets, params[1]);
	else
		exitChannels(client, targets, "");
}

void    IRCServer::handleTopic(Client& client, const paramVector& params)
{
	if (params.empty())
	{ // 파라미터 부족
//		sendMsg(client.getFd(), notEnoughParam(client, "TOPIC"));
		Msg().errNotEnoughParam(client.getNickName(), "TOPIC").sendTo(client.getFd());
		return ;
	}
	if(!m_channelManager.findChannel(params[0]))
	{
//		sendMsg(client.getFd(), errMsg(ERR_NOSUCHCHANNEL, client, params[0], "No such Channel"));
		Msg().errNoSuchChannel(client.getNickName(), params[0]).sendTo(client.getFd());
		return ;
	}
	if (!client.isInChannel(params[0]))
	{
//		sendMsg(client.getFd(), errMsg(ERR_NOTONCHANNEL, client, params[0], "You're not on channel"));
		Msg().errNotOnChannel(client.getNickName(), params[0]).sendTo(client.getFd());
		return ;
	}
	Channel *channel;
	m_channelManager.getChannel(params[0], channel);
	if (params.size() == 1)
	{
		if (channel->getTopic().empty())
			Msg().rplNoTopic(client.getNickName(), params[0]).sendTo(client.getFd());
		else
			Msg().rplTopic(client.getNickName(), params[0], channel->getTopic()).sendTo(client.getFd());
		return ;
	}
	if (channel->isTopicMode() && !channel->isChannelOper(client.getNickName()))
	{
		Msg().errChanOpPrivsNeeded(client.getNickName(), params[1]);
		return ;
	}
	channel->setTopic(params[2]);
	channel->broadcastMsg(
			Msg()
			.setPrefix(client.getMsgPrefix())
			.addParam("TOPIC")
			.addParam(params[0])
			.serialize()
			);
}

void    IRCServer::handleNames(Client& client, const paramVector& params)
{
}

void    IRCServer::handleList(Client& client, const paramVector& params)
{
}

void    IRCServer::handleInvite(Client& client, const paramVector& params)
{
	if (params.size() < 2)
	{ // 파라미터 부족
//		sendMsg(client.getFd(), notEnoughParam(client, "INVITE"));
		Msg().errNotEnoughParam(client.getNickName(), "INVITE").sendTo(client.getFd());
		return ;
	}
	if (!m_clientManager.isNickExists(params[0]))
	{
//		sendMsg(client.getFd(), errMsg(ERR_NOSUCHNICK, client, params[0], "No such nickanme"));
		Msg().errNoSuchNick(client.getNickName(), params[0]).sendTo(client.getFd());
		return ;
	}
	if (!client.isInChannel(params[1]))
	{
//		sendMsg(client.getFd(), errMsg(ERR_NOTONCHANNEL, client, params[1], "You're not on that channel"));
		Msg().errNotOnChannel(client.getNickName(), params[1]).sendTo(client.getFd());
		return ;
	}
	Channel *channel;
	m_channelManager.getChannel(params[1], channel);
	if(channel->findMember(params[1]))
	{
//		sendMsg(client.getFd(), errMsg(ERR_USERONCHANNEL, client, params[0], params[1], "is already on channel"));
		Msg().errUserOnChannel(client.getNickName(), params[0], params[1]).sendTo(client.getFd());
		return ;
	}
	if (!channel->isChannelOper(client.getNickName()))
	{
//		sendMsg(client.getFd(), errMsg(ERR_CHANOPRIVSNEEDED, client, params[1], "you're not channel operator"));
		Msg().errChanOpPrivsNeeded(client.getNickName(), params[1]).sendTo(client.getFd());
		return ;
	}
	/*여기까지 예외처리 끝.*/
	Client &invitedClient = m_clientManager.getClient(params[0]);
	//sendMsg(client.getFd(), noTrailingMsg(RPL_INVITING, client, params[0], params[1]));
	Msg()
		.setPrefix(SERVER_PREFIX)
		.numeric(RPL_INVITING)
		.addParam(client.getNickName())
		.addParam(params[1])
		.addParam(params[0])
		.trailing(false)
		.sendTo(client.getFd());
//	sendMsg(invitedClient.getFd(), goodMsg(client, "INVITE", params[0], params[1]));
	Msg()
		.setPrefix(client.getMsgPrefix())
		.addParam("INVITE")
		.addParam(params[0])
		.addParam(params[1])
		.sendTo(invitedClient.getFd());
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
//			msg = goodMsg(client, "KICK", *channelIter, *targetlIter, trailing);
			msg = Msg()
				.setPrefix(client.getMsgPrefix())
				.addParam("KICK")
				.addParam(*channelIter)
				.addParam(*targetlIter)
				.addParam(trailing)
				.serialize();
			m_channelManager.kickClientFromChannel(client, *channelIter, *targetlIter, msg);
			targetlIter++;
		}
	else
		while (channelIter != channelIterEnd)
		{
//			msg = goodMsg(client, "KICK", *channelIter, *targetlIter, trailing);
			msg = Msg()
				.setPrefix(client.getMsgPrefix())
				.addParam("KICK")
				.addParam(*channelIter)
				.addParam(*targetlIter)
				.addParam(trailing)
				.serialize();
			m_channelManager.kickClientFromChannel(client, *channelIter, *targetlIter, msg);
			channelIter++;
			targetlIter++;
		}
}
void    IRCServer::handleKick(Client& client, const paramVector& params)
{
	if (params.size() < 2)
	{
//		sendMsg(client.getFd(), notEnoughParam(client, "KICK"));
		Msg().errNotEnoughParam(client.getNickName(), "KICK").sendTo(client.getFd());
	}
	
	/*서버 멀티타겟 스플릿.*/
	std::vector<std::string> channels;
	splitMultiTarget(params[0], channels);
	if (channels.empty())
	{
//		sendMsg(client.getFd(), notEnoughParam(client, "KICK"));
		Msg().errNotEnoughParam(client.getNickName(), "KICK").sendTo(client.getFd());
		return ;
	}

	/*유저 멀티타겟 스플릿.*/
	std::vector<std::string> clients;
	splitMultiTarget(params[1], clients);
	if (clients.empty())
	{
//		sendMsg(client.getFd(), notEnoughParam(client, "KICK"));
		Msg().errNotEnoughParam(client.getNickName(), "KICK").sendTo(client.getFd());
		return ;
	}

	if (channels.size() != 1 && (channels.size() != clients.size()))
	{ // 1대1 매칭인 경우 파라미터 에러 반환
//		sendMsg(client.getFd(), notEnoughParam(client, "KICK"));
		Msg().errNotEnoughParam(client.getNickName(), "KICK").sendTo(client.getFd());
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
