#include "IRCServer.hpp"
#include "modeMask.hpp"
#include "MyLibft.hpp"
#include <cctype>
typedef std::vector<std::string> strVect;

// ==============================================================================
// 1. 등록 및 인증 관련 (Connection Registration)
// ==============================================================================

void    IRCServer::handlePass(Client& client, const paramVector& params)
{
	MsgBuilder msg;
	if (!params.size())
	{
		msg.buildErrMsg(ERR_NEEDMOREPARAMS, client,
				"PASS", "Not enough parameters");
		return ;
	}
	else if (client.isRegistered())
	{
		msg.buildErrMsg(ERR_ALREADYREGISTRED, client,
				"already registered");
		return ;
	}
	else if (client.isAuthed())
		return;
	else if (params[0] != m_passwd)
	{
		msg.buildErrMsg(ERR_PASSWDMISMATCH, client,
				"Password incorrect");
		softDisconnect(client);
		return ;
	}
	client.setAuthed();
}

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
	MsgBuilder msg;
	if (!params.size())
	{ // 파라미터 부족.
		msg.buildErrMsg(ERR_NONICKNAMEGIVEN, client, "No nickname given");
		return ;
	}
	else if (client.isRegistered())
	{
		msg.buildErrMsg(ERR_ALREADYREGISTRED, client,
				"already registered");
		return ;
	}
	if ( !isValidNick(params[0]) )
	{
		msg.buildErrMsg(ERR_ERRONEUSNICKNAME,
				client, params[0], "Erroneous nickname");
		return ;
	}
	if ( client.getNickName() == params[0] )
		return ; // 이미 동일하니 무시하기.
	if ( !m_clientManager.setClientNickName(client.getFd(), params[0]) )
	{
		msg.buildErrMsg(ERR_NICKNAMEINUSE,
				client, params[0], "Nickname is already in use");
		return ;
	}
	/* 변경 성공! */
	// client가 속한 채널에 대해 브로드캐스트 로직.
	if (client.getUserName().size()
		&& client.isAuthed()
		&& client.getNickName() != "*"
		)
		client.setRegistered();
}

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

static bool isNumeric( const std::string &str )
{
	for (size_t i = 0; str[i] != '\0'; i++)
	{
		if ( !std::isdigit(str[i]) )
			return (false);
	}
	return (true);
}

static void setUserMode( Client &client, const std::string &str )
{
	unsigned int mode = 0;
	if (isNumeric(str))
	{
		mode = *(const_cast<std::string &>(str).rbegin()) - '0';
		std::cout << mode << std::endl;
	}

	if (hasMode(mode, UMODE_INVISIBLE))
		client.setInvisible(true);
	else
		client.setInvisible(false);

	if (hasMode(mode, UMODE_WALLOPS))
		client.setWallops(false);
	else
		client.setWallops(false);
}

void    IRCServer::handleUser(Client& client, const paramVector& params)
{
	MsgBuilder msg;
	if (params.size() < 4)
	{
		msg.buildErrMsg(ERR_NEEDMOREPARAMS,
				client, "USER", "Not enough parameters");
		return ;
	}
	else if (client.isRegistered())
	{
		msg.buildErrMsg(ERR_ALREADYREGISTRED, client,
				"already registered");
		return ;
	}
	std::string userName = getUserName(params[0]);
	if (userName.empty())
		client.setUserName( "unknownUserName" );
	else
		client.setUserName(userName);
	setUserMode(client, params[1]);
	client.setRealName(params[3]);

	std::cout << "nickname: " << client.getNickName() << std::endl;
	std::cout << "username: " << client.getUserName() << std::endl;
	std::cout << "realname: " << client.getRealName() << std::endl;

	if (client.getUserName().size()
		&& client.isAuthed()
		&& client.getNickName() != "*"
		)
		client.setRegistered();
}

void    IRCServer::handleQuit(Client& client, const paramVector& params)
{
	softDisconnect(client);
}

void    IRCServer::handleOper(Client& client, const paramVector& params)
{
	MsgBuilder msg;
	if (params.size() < 2)
	{
		msg.buildErrMsg(ERR_NEEDMOREPARAMS,
				client, "OPER", "Not enough parameters");
		return ;
	}
}

// ==============================================================================
// 2. 메시지 전송 (Message Sending)
// ==============================================================================

void    IRCServer::handlePrivmsg(Client& client, const paramVector& params)
{
}

void    IRCServer::handleNotice(Client& client, const paramVector& params)
{
}

// ==============================================================================
// 3. 채널 조작 (Channel Operations)
// ==============================================================================

extern bool splitUntilChar(std::string &line, std::string seperator, std::string &result);

static bool splitTrailing(std::string str , strVect &vect)
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

void IRCServer::broadcastToChannel( Channel &channel )
{
	std::cout << "hihihihihih" << std::endl;
	std::map<std::string, Client *> members = channel.getChannelMembers();
	channel.isChannelEmpty();
	for(std::map<std::string, Client *>::iterator iter = members.begin(); iter != members.end(); iter++)
	{
		Client &client = *(iter->second);
		std::cout << "\tsend msg to " << client.getNickName() << std::endl;
		if (MyLibft::sendMsg(iter->second->getFd(), "HELLO!\r\n"))
			hardDisconnect(*(iter->second));
	}
}

void IRCServer::joinProcess(Client &client, paramVector &servers, paramVector &keys)
{
	paramVector::iterator servIter = servers.begin();
	paramVector::iterator keyIter = keys.begin();
	MsgBuilder msg;

	std::cout << servers.size() << std::endl;
	std::cout << keys.size() << std::endl;

	while (servIter != servers.end())
	{
		Channel *channel;
		if (m_channelManager.getChannel( *servIter, channel ))
		{ // 서버 찾음
			if (!channel->findMember( client ))
			{ // 멤버 못찾음 추가요망.
				if (!channel->getPasswd().empty())
				{ // 패스워드 있음.
					if (keyIter == keys.end() || *keyIter != channel->getPasswd())
					{ // 패스워드 불일치.
						MyLibft::sendMsg(client.getFd(),
								msg.buildErrMsg(ERR_BADCHANNELKEY, client,
								*servIter, "Cannot join channel (+k) bad key")
								);
						msg.clear();
					}
					/* 피곤해서 이부분 당장 Continue로 했음.
					 * 서버 찾음 부분부터 통째로 함수로 나눔 될 듯.
					 */
					servIter++;
					if (keyIter != keys.end())
						keyIter++;
					continue ;
				}
				// 패스워드 없음. 채널에 그냥 추가.
				m_channelManager.addClientToChannel(client, *servIter);
				MyLibft::sendMsg(client.getFd(),
						msg.buildSendMsg(client, "JOIN", *servIter) + "\r\n");
			}
			// 멤버 찾음 -> 무시됨.
		}
		else
		{ // 서버 없음.
			m_channelManager.addClientToChannel(client, *servIter);
			if (keyIter != keys.end())
			{
				m_channelManager.getChannel( *servIter, channel );
				channel->assignPasswd( *keyIter );
			}
			MyLibft::sendMsg(client.getFd(), msg.buildSendMsg(client, "JOIN", *servIter) + "\r\n");
			// 성공 브로드캐스트 필요.
		}
		servIter++;
		if (keyIter != keys.end())
			keyIter++;
	}
}

void    IRCServer::handleJoin(Client& client, const paramVector& params)
{
	MsgBuilder msg;
	if (params.size() < 1)
	{
		msg.buildErrMsg(ERR_NEEDMOREPARAMS,
				client, "JOIN", "Not enough parameters");
		return ;
	}
	paramVector servers;
	paramVector keys;
	if (params[0] == "0")
	{ // 모든 채널에서 나가는 로직 넣어줘야 함.
		return ;
	}
	else
	{
		if (!splitTrailing(params[0], servers))
		{
			msg.buildErrMsg(ERR_BADCHANMASK,
					client, "bad channel name");
			return ;
		}
	}
	if (params.size() > 1)
	{
		if (!splitTrailing(params[1], keys))
		{
			msg.buildErrMsg(ERR_NEEDMOREPARAMS,
					client, "JOIN", "key param error");
			return ;
		}
	}
	/* 키가 채널보다 많으면 빈 값으로 취급할거임. */
	/* 이미 존재하는 채널이면 생성이 아닌 참여. */
	joinProcess(client, servers, keys);
}

void    IRCServer::handlePart(Client& client, const paramVector& params)
{
}

void    IRCServer::handleTopic(Client& client, const paramVector& params)
{
}

void    IRCServer::handleNames(Client& client, const paramVector& params)
{
}

void    IRCServer::handleList(Client& client, const paramVector& params)
{
}

void    IRCServer::handleInvite(Client& client, const paramVector& params)
{
}

void    IRCServer::handleKick(Client& client, const paramVector& params)
{
}

void    IRCServer::handleMode(Client& client, const paramVector& params)
{
}

// ==============================================================================
// 4. 서버 및 유저 정보 (Server Queries & User Info)
// ==============================================================================

void    IRCServer::handleWho(Client& client, const paramVector& params)
{
}

void    IRCServer::handleWhois(Client& client, const paramVector& params)
{
}

void    IRCServer::handleWhowas(Client& client, const paramVector& params)
{
}

void    IRCServer::handlePing(Client& client, const paramVector& params)
{
}

void    IRCServer::handlePong(Client& client, const paramVector& params)
{
}

// ==============================================================================
// 5. 기타 편의/보너스 (Miscellaneous / Optional)
// ==============================================================================

void    IRCServer::handleCap(Client& client, const paramVector& params)
{
}

void    IRCServer::handleAway(Client& client, const paramVector& params)
{
}

void    IRCServer::handleKill(Client& client, const paramVector& params)
{
}
