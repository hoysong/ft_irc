#include "IRCServer.hpp"
#include "Msg.hpp"
#include "ircError.hpp"

typedef std::vector<std::string> strVect;

extern void splitMultiTarget(std::string str , strVect &vect);

static bool nameVldChk( std::string &str )
{
	if (str.length() > 50)
		return (false);
	else if (str[0] != '#')
		return (false);
	else if (str.length() == 1)
		return (false);
	for(std::string::iterator iter = str.begin(); iter != str.end(); iter++)
	{
		if ( *iter == '\r' || *iter == ' ' || *iter == 7 || *iter == '\0' || *iter == '\n' || *iter == ':' || *iter == ',' )
			return (false);
	}
	return (true);
}

void IRCServer::joinProcess(Client &client, paramVector &servers, paramVector &keys)
{
	paramVector::iterator servIter = servers.begin();
	paramVector::iterator keyIter = keys.begin();

	while (servIter != servers.end())
	{
		if (!nameVldChk(*servIter))
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
			iter->second->removeMember(client);
			m_channelManager.eraseEmptyChannel(iter->second->getChannelName());
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
	joinProcess(client, servers, keys);
}
