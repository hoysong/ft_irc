#include "IRCServer.hpp"

typedef std::vector<std::string> strVect;

extern bool splitUntilChar(std::string &line, std::string seperator, std::string &result);

static bool splitTrailing(std::string str , strVect &vect)
{
	for( std::string result; splitUntilChar(str, ",", result); )
	{
		if (result.empty())
			return (false);
		vect.push_back(str);
	}
	if (!str.empty())
		vect.push_back(str);
	else if (str.empty())
		return (false);
	return (true);
}

void IRCServer::aboutExistChannel(Client &client,
		Channel &channel,
		strVect &servers,
		strVect &keys,
		strVect::iterator &servIter,
		strVect::iterator &keyIter
		)
{
	if (!channel.findMember( client ))
	{ // 멤버 못찾음 추가요망.
		if (!channel.getPasswd().empty())
		{ // 패스워드 있음.
			if (keyIter == keys.end() || *keyIter != channel.getPasswd())
			{ // 패스워드 불일치.
				msgSender(client, MsgBuilder::buildErrMsg(ERR_BADCHANNELKEY, client, *servIter, "Cannot join channel (+k) bad key"));
			}
			return ;
		}
		// 패스워드 없음. 채널에 그냥 추가.
		m_channelManager.addClientToChannel(client, *servIter);
		welcomeBroadcast(client, channel);
	}
	// 멤버 찾음 -> 무시됨.
}

void IRCServer::joinProcess(Client &client, paramVector &servers, paramVector &keys)
{
	paramVector::iterator servIter = servers.begin();
	paramVector::iterator keyIter = keys.begin();

	while (servIter != servers.end())
	{
		Channel *channel;
		if (m_channelManager.getChannel( *servIter, channel ))
		{ // 서버 찾음
			aboutExistChannel(client, *channel, servers, keys, servIter, keyIter);
		}
		else
		{ // 서버 없음.
			m_channelManager.addClientToChannel(client, *servIter);
			if (keyIter != keys.end())
			{
				m_channelManager.getChannel( *servIter, channel );
				channel->assignPasswd( *keyIter );
			}
			msgSender(client, MsgBuilder::buildSendMsg(client, "JOIN", *servIter));
			
			// 성공 브로드캐스트 필요.
		}
		servIter++;
		if (keyIter != keys.end())
			keyIter++;
	}
}

void    IRCServer::handleJoin(Client& client, const paramVector& params)
{
	if (params.size() < 1)
	{
		msgSender(client, MsgBuilder::buildErrMsg(ERR_NEEDMOREPARAMS, client, "JOIN", "Not enough parameters"));
		return ;
	}
	paramVector servers;
	paramVector keys;
	if (params[0] == "0")
	{ // 모든 채널에서 나가는 로직 넣어줘야 함.
		return ;
	}
	else
	{
		if (!splitTrailing(params[0], servers))
		{
			msgSender(client, MsgBuilder::buildErrMsg(ERR_BADCHANMASK, client, "bad channel name"));
			return ;
		}
	}
	if (params.size() > 1)
	{
		if (!splitTrailing(params[1], keys))
		{
			msgSender(client, MsgBuilder::buildErrMsg(ERR_NEEDMOREPARAMS, client, "JOIN", "key param error"));
			return ;
		}
	}
	/* 키가 채널보다 많으면 빈 값으로 취급할거임. */
	/* 이미 존재하는 채널이면 생성이 아닌 참여. */
	joinProcess(client, servers, keys);
}
