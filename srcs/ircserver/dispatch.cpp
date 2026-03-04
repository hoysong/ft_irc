#include "IRCServer.hpp"
#include "Msg.hpp"
#include "ircError.hpp"
#include "modeMask.hpp"
#include <cctype>
#include <sys/socket.h>
#include <vector>
#include "ircError.hpp"
#include <iostream>

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
		Msg().errAlreadyRegist(client.getNickName()).sendTo(client.getFd());
		return ;
	}
	else if (!params.size())
	{
		Msg().errNotEnoughParam(client.getNickName(), "PASS").sendTo(client.getFd());
		return ;
	}
	else if (client.isAuthed())
		return;
	else if (params[0] != m_passwd)
	{
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


void    IRCServer::handleQuit(Client& client, const paramVector& params)
{
	softDisconnect(client,
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
		Msg().errNotEnoughParam(client.getNickName(), "PART").sendTo(client.getFd());
		return ;
	}
	std::vector<std::string> targets;
	splitMultiTarget(params[0], targets);
	if (targets.empty() && params.size() == 1)
	{ // 파라미터 부족
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
		Msg().errNotEnoughParam(client.getNickName(), "TOPIC").sendTo(client.getFd());
		return ;
	}
	if(!m_channelManager.findChannel(params[0]))
	{
		Msg().errNoSuchChannel(client.getNickName(), params[0]).sendTo(client.getFd());
		return ;
	}
	if (!client.isInChannel(params[0]))
	{
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
		Msg().errNotEnoughParam(client.getNickName(), "INVITE").sendTo(client.getFd());
		return ;
	}
	if (!m_clientManager.isNickExists(params[0]))
	{
		Msg().errNoSuchNick(client.getNickName(), params[0]).sendTo(client.getFd());
		return ;
	}
	if (!client.isInChannel(params[1]))
	{
		Msg().errNotOnChannel(client.getNickName(), params[1]).sendTo(client.getFd());
		return ;
	}
	Channel *channel;
	m_channelManager.getChannel(params[1], channel);
	if(channel->findMember(params[1]))
	{
		Msg().errUserOnChannel(client.getNickName(), params[0], params[1]).sendTo(client.getFd());
		return ;
	}
	if (!channel->isChannelOper(client.getNickName()))
	{
		Msg().errChanOpPrivsNeeded(client.getNickName(), params[1]).sendTo(client.getFd());
		return ;
	}
	/*여기까지 예외처리 끝.*/
	Client &invitedClient = m_clientManager.getClient(params[0]);
	Msg()
		.setPrefix(SERVER_PREFIX)
		.numeric(RPL_INVITING)
		.addParam(client.getNickName())
		.addParam(params[1])
		.addParam(params[0])
		.trailing(false)
		.sendTo(client.getFd());
	Msg()
		.setPrefix(client.getMsgPrefix())
		.addParam("INVITE")
		.addParam(params[0])
		.addParam(params[1])
		.sendTo(invitedClient.getFd());
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
	if (params.empty())
	{
		Msg().errNotEnoughParam(client.getNickName(), "PING").sendTo(client.getFd());
		return ;
	}
	Msg().setPrefix(SERVER_PREFIX)
		.addParam("PONG")
		.addParam(SERVER_PREFIX)
		.addParam(params[0])
		.sendTo(client.getFd());
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
