#include "IRCServer.hpp"

typedef std::vector<std::string> strVect;

extern bool splitUntilChar(std::string &line, std::string seperator, std::string &result);

bool splitTrailing(std::string str , strVect &vect)
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

void IRCServer::joinProcess(Client &client, paramVector &servers, paramVector &keys)
{
	paramVector::iterator servIter = servers.begin();
	paramVector::iterator keyIter = keys.begin();

	while (servIter != servers.end())
	{
		if ((*servIter)[0] != '#' || servIter->size() == 1)
			msgSender(client, MsgBuilder::buildErrMsg(ERR_BADCHANMASK, client, *servIter, "Invalid channel name"));
		else if (keyIter == keys.end())
			m_channelManager.addClientToChannel(client, *servIter, "", *this);
		else
			m_channelManager.addClientToChannel(client, *servIter, *keyIter, *this);
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
