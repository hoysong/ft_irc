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

void IRCServer::sendNotEnoughParam( Client &client, const std::string &cmd )
{
	std::string msg = MsgBuilder::notEnoughParam(client, cmd);
	msgSender(client, msg);
}
