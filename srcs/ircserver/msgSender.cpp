#include "IRCServer.hpp"
#include "Msg.hpp"

void IRCServer::welcomeMsg( Client &client )
{
	std::string msg;
	msg = ":" + m_serverName + " 001 " + client.getNickName() + " :Welcome to the Internet Relay Network " + client.getNickName() + "!" + client.getUserName() + "@" + client.getHost();
	msg += "\r\n";
	msg += ":" + m_serverName + " 002 " + client.getNickName() + " :Your host is " + m_serverName + ", " + "running version ft_irc-0.1";
	msg += "\r\n";
	msg += ":" + m_serverName + " 003 " + client.getNickName() + " :This server was created " + m_startStamp;
	msg += "\r\n";
	msg += ":" + m_serverName + " 004 " + client.getNickName() + " " + m_serverName + " " + m_version + " i" + " itkol";
	msg += "\r\n";
	sendMsg(client.getFd(), msg);
	msg.clear();
}
