#include "IRCServer.hpp"

typedef std::vector<std::string> strVect;

extern bool splitUntilChar(std::string &line, std::string seperator, std::string &result);

bool splitMultiTarget(std::string str , strVect &vect)
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
			sendMsg(client.getFd(), errMsg(ERR_BADCHANMASK, client, *servIter, "Invalid cahnnel name"));
		else if (keyIter == keys.end())
			m_channelManager.addClientToChannel(client, *servIter, "");
		else
			m_channelManager.addClientToChannel(client, *servIter, *keyIter);
		servIter++;
		if (keyIter != keys.end())
			keyIter++;
	}
}

void    IRCServer::handleJoin(Client& client, const paramVector& params)
{
	if (params.size() < 1)
	{
		sendMsg(client.getFd(), notEnoughParam(client, "JOIN"));
		return ;
	}
	paramVector servers;
	paramVector keys;
	if (params[0] == "0")
	{ // 모든 채널에서 나가는 로직 넣어줘야 함.
		softDisconnect(client, goodMsg(client, "PART", "Parted by client"));
		return ;
	}
	else
	{
		if (!splitMultiTarget(params[0], servers))
		{
			sendMsg(client.getFd(), errMsg(ERR_BADCHANMASK, client, params[0], "bad channel name"));
			return ;
		}
	}
	if (params.size() > 1)
	{
		if (!splitMultiTarget(params[1], keys))
		{
			sendMsg(client.getFd(), errMsg(ERR_NEEDMOREPARAMS, client, "JOIN", "key param error"));
			return ;
		}
	}
	/* 키가 채널보다 많으면 빈 값으로 취급할거임. */
	/* 이미 존재하는 채널이면 생성이 아닌 참여. */
	joinProcess(client, servers, keys);
}
