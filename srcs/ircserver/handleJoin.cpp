#include "IRCServer.hpp"
#include "Msg.hpp"
#include "ircError.hpp"

typedef std::vector<std::string> strVect;

extern void splitMultiTarget(std::string str , strVect &vect);

void IRCServer::joinProcess(Client &client, paramVector &servers, paramVector &keys)
{
	paramVector::iterator servIter = servers.begin();
	paramVector::iterator keyIter = keys.begin();

	while (servIter != servers.end())
	{
		if ((*servIter)[0] != '#' || servIter->size() == 1)
			Msg()
				.setPrefix(SERVER_PREFIX)
				.numeric(ERR_NOSUCHCHANNEL)
				.addParam(client.getNickName())
				.addParam(*servIter)
				.addParam("Invalid channel name")
				.sendTo(client, *this);
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
		Msg().errNotEnoughParam(client.getNickName(), "JOIN").sendTo(client, *this);
		return ;
	}
	paramVector servers;
	paramVector keys;
	if (params[0] == "0")
	{
		std::map<std::string, Channel *> channels = client.getJoinedChannel();
		for(std::map<std::string, Channel *>::iterator iter = channels.begin(); iter != channels.end(); iter++)
		{
			iter->second->broadcastMsg(
					Msg()
					.setPrefix(client.getMsgPrefix())
					.addParam("PART")
					.addParam(iter->second->getChannelName())
					.serialize()
					);
		}
		return ;
	}
	else
		splitMultiTarget(params[0], servers); // 콤마 기준 split
	if (params.size() > 1)
		splitMultiTarget(params[1], keys); // 콤마 기준 split
	if (servers.empty() && keys.empty())
	{
		Msg().errNotEnoughParam(client.getNickName(), "JOIN").sendTo(client, *this);
		return ;
	}
	/* 키가 채널보다 많으면 빈 값으로 취급할거임. */
	/* 이미 존재하는 채널이면 생성이 아닌 참여. */
	joinProcess(client, servers, keys);
}
