#include "IRCServer.hpp"

void IRCServer::msgSender( Client &client, const std::string &msg )
{
	if (!MyLibft::sendMsg(client.getFd(), msg + "\r\n"))
	{ // 전송 실패.
		hardDisconnect(client, "");
	}
}

// ==============================================================================================================
// 브로드캐스트류 함수들.
// ==============================================================================================================

/* 채널에 입장했을 때 브로드캐스트. */

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

//void IRCServer::quitBroadcast( Client &client, Channel &channel , const std::string &msg)
//{
//	std::map<std::string, Client *> members = channel.getChannelMembers();
//	std::string buff = msg;
//	if (buff.empty())
//		buff = "Connection closed";
//	for (std::map<std::string, Client *>::iterator iter = members.begin(); iter != members.end(); iter++)
//	{
//		msgSender( *(iter->second),
//				":" + client.getNickName() +"!" + client.getUserName() + "@" + client.getHost() + " QUIT :" + buff );
//	}
//}

//void IRCServer::quitBroadcastToChannels( Client &client, const std::string &msg)
//{
//	std::map<std::string, Channel *> channels = client.getJoinedChannel();
//	for(std::map<std::string, Channel *>::iterator iter = channels.begin(); iter != channels.end(); iter++)
//	{
//		quitBroadcast(client, *(iter->second), msg);
//	}
//}

// ==============================================================================================================
// 기타 메시지.
// ==============================================================================================================

void IRCServer::sendNotEnoughParam( Client &client, const std::string &cmd )
{
	std::string msg = MsgBuilder::notEnoughParam(client, cmd);
	msgSender(client, msg);
}

void IRCServer::welcomeMsg( Client &client )
{
	std::string msg;
	msg = ":" + m_serverName + " 001 " + client.getNickName() + " :Welcome to the Internet Relay Network " + client.getNickName() + "!" + client.getUserName() + "@" + client.getHost();
	msgSender(client, msg);
	msg.clear();
	msg = ":" + m_serverName + " 002 " + client.getNickName() + " :Your host is " + m_serverName + ", " + "running version ft_irc-0.1";
	msgSender(client, msg);
	msg.clear();
	msg = ":" + m_serverName + " 003 " + client.getNickName() + " :This server was created " + m_startStamp;
	msgSender(client, msg);
	msg.clear();
}
