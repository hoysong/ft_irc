#include "IRCServer.hpp"
//#include "modeMask.hpp"
#include "Msg.hpp"
#include <iostream>

static std::string getUserName( const std::string &str )
{
	std::string buffer;

	for (size_t i = 0; str[i] != '\0'; i++)
	{
		if (str[i] >= 33 && str[i] <= 126)
			buffer += str[i];
	}
	return (buffer);
}

void    IRCServer::handleUser(Client& client, const paramVector& params)
{
	if (client.isRegistered())
	{
		Msg().errAlreadyRegist(client.getNickName()).sendTo(client, *this);
		return ;
	}
	else if (params.size() < 4)
	{
		Msg().errNotEnoughParam(client.getNickName(), "USER").sendTo(client, *this);
		return ;
	}
	else if (params[3].empty())
	{
		Msg().errNotEnoughParam(client.getNickName(), "USER").sendTo(client, *this);
		return ;
	}
	std::string userName = getUserName(params[0]);
	if (userName.empty())
		client.setUserName( "unknownUserName" );
	else
		client.setUserName(userName);
	client.setRealName(params[3]);

	std::cout << "nickname: " << client.getNickName() << std::endl;
	std::cout << "username: " << client.getUserName() << std::endl;
	std::cout << "realname: " << client.getRealName() << std::endl;

	if (client.getUserName().size()
		&& client.isAuthed()
		&& client.getNickName() != "*"
		)
	{
		client.setRegistered();
		welcomeMsg(client);
	}
}
