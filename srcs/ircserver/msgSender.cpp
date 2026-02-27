#include "IRCServer.hpp"

void IRCServer::welcomeMsg( Client &client )
{
	std::string msg;
	msg = ":" + m_serverName + " 001 " + client.getNickName() + " :Welcome to the Internet Relay Network " + client.getNickName() + "!" + client.getUserName() + "@" + client.getHost();
	sendMsg(client.getFd(), msg + "\r\n");
	msg.clear();
	msg = ":" + m_serverName + " 002 " + client.getNickName() + " :Your host is " + m_serverName + ", " + "running version ft_irc-0.1";
	sendMsg(client.getFd(), msg + "\r\n");
	msg.clear();
	msg = ":" + m_serverName + " 003 " + client.getNickName() + " :This server was created " + m_startStamp;
	sendMsg(client.getFd(), msg + "\r\n");
	msg.clear();
}
