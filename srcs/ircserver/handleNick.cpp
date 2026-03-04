#include "IRCServer.hpp"
#include "Msg.hpp"

static bool charValiedCheck( char c )
{
	if ( std::isalpha(c) )
		return (true);

	const std::string specials = "[]\\`_^{}|";
	if (specials.find(c) != std::string::npos)
		return (true);
	return (false);
}

static bool isValidNick( const std::string &nick )
{
	if (nick.size() > 9)
		return (false);
	if (!charValiedCheck(nick[0]))
		return (false);

	for (size_t i = 0; nick[i] != '\0'; i++)
	{
		if (charValiedCheck(nick[i]))
			continue ;
		else if (std::isdigit(nick[i]))
			continue ;
		else if (nick[i] == '-')
			continue ;
		else
			return (false);
	}
	return (true);
}

void    IRCServer::handleNick(Client& client, const paramVector& params)
{
	if (!params.size())
	{ // 파라미터 부족.
		Msg().errNoNickGiven(client.getNickName()).sendTo(client.getFd());
		return ;
	}
	if ( !isValidNick(params[0]) )
	{
		Msg().errOneousNick(client.getNickName(), params[0]).sendTo(client.getFd());
		return ;
	}
	if ( client.getNickName() == params[0] )
		return ; // 이미 동일하니 무시하기.
	std::string oldNickBuffer = client.getNickName();
	if ( !m_clientManager.setClientNickName(client, params[0]) )
	{
		Msg().errNickInUse(client.getNickName(), params[0]).sendTo(client.getFd());
		return ;
	}
	/* 변경 성공! */
	// client가 속한 채널에 대해 브로드캐스트 로직.
	if (client.getUserName().size()
		&& client.isAuthed()
		&& client.getNickName() != "*"
		)
	{
		if (!client.isRegistered())
			welcomeMsg(client);
		client.setRegistered();
	}
}
