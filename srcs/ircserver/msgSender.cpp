#include "IRCServer.hpp"

void IRCServer::msgSender( Client &client, const std::string &msg )
{
	if (!MyLibft::sendMsg(client.getFd(), msg + "\r\n"))
	{ // 전송 실패.
		hardDisconnect(client);
	}
}

void IRCServer::welcomeBroadcast ( Client &client, Channel &channel )
{
	std::map<std::string, Client *> members = channel.getChannelMembers();
	for (std::map<std::string, Client *>::iterator iter = members.begin(); iter != members.end(); iter++)
	{
		msgSender( *(iter->second),
				MsgBuilder::buildSendMsg(client, "JOIN", channel.getChannelName())
				);
	}
}

void IRCServer::nickChangeBroadcast(Client &client, Channel &channel, const std::string &oldNick)
{
	std::map<std::string, Client *> members = channel.getChannelMembers();
	for (std::map<std::string, Client *>::iterator iter = members.begin(); iter != members.end(); iter++)
	{
		msgSender( *(iter->second),
				":" + oldNick+"!" + client.getUserName() + "@" + client.getHost() + " NICK :" + client.getNickName()
				);
	}

	std::map<std::string, Client *> memberMap = channel.getChannelMembers();
	std::map<std::string, Client *>::iterator memberIter = memberMap.find(oldNick);
	if (memberIter != memberMap.end())
	{
		memberMap.erase(memberIter);
		memberMap[client.getNickName()] = &client;
	}
}

void IRCServer::nickChangeBroadcastToChannels( Client &client, const std::string  &oldNick)
{
	std::map<std::string, Channel *> channels = client.getJoinedChannel();
	for(std::map<std::string, Channel *>::iterator iter = channels.begin(); iter != channels.end(); iter++)
	{
		nickChangeBroadcast(client, *(iter->second), oldNick);
	}
}

void IRCServer::sendNotEnoughParam( Client &client, const std::string &cmd )
{
	std::string msg = MsgBuilder::notEnoughParam(client, cmd);
	msgSender(client, msg);
}
