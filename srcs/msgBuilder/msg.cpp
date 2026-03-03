#include "Msg.hpp"
#include <sstream>
#include <vector>
#include <cerrno>
#include <sys/socket.h>

bool sendMsg( int fd, const std::string &buf )
{
	size_t	total = 0;
	size_t	len = buf.size();

	while (total < len)
	{
		ssize_t	n = send(fd, buf.data() + total, len - total, MSG_NOSIGNAL);
		if (n > 0)
			total += static_cast<size_t>(n);
		else if (n < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return (false); // 타임아웃 또는 에러.
			else if (errno == EINTR)
				continue; // 시그널로 방해받음 다시 시도.
			else
				return (false); // 실패
		}
		else
			break; // 끝?
	}
	return (true);
}
/*기본 함수들.*/
Msg & Msg::setPrefix( const std::string &prefix )
{
	m_prefix = prefix;
	return (*this);
}
Msg & Msg::numeric( int errOrRpl )
{
	m_numeric = errOrRpl;
	return (*this);
}
Msg & Msg::addParam( const std::string &param )
{
	m_params.push_back(param);
	return (*this);
}
Msg & Msg::trailing( bool flag )
{
	m_trailing = flag;
	return (*this);
}
std::string Msg::serialize( void )
{
	std::stringstream ss;
	ss << ":" + m_prefix + " ";
	if (m_numeric > -1)
		ss << m_numeric << " ";
	if (!m_params.empty())
	{
		for(size_t i = 0; i < m_params.size() - 1; i++)
			ss << m_params[i] << " ";
		if(m_trailing)
			ss << ":" << *m_params.rbegin();
		else
			ss << *m_params.rbegin();
	}
	ss << "\r\n";
	return (ss.str());
}
Msg & Msg::clear( void )
{
	m_prefix.clear();
	m_numeric = -1;
	m_params.clear();
	m_trailing = true;
	return (*this);
}
bool Msg::sendTo( int fd )
{
	return (sendMsg(fd, serialize()));
}
/*constructor*/
Msg::Msg( void ) : m_trailing(true), m_numeric(-1) { }

// =========================================================================================
// 미리 지정된 함수들.
// =========================================================================================

Msg & Msg::errUnknownCmd(const std::string &nick, const std::string &cmd)
{
	setPrefix(SERVER_PREFIX);
	numeric(421);
	addParam(nick);
	addParam(cmd);
	addParam("Unknown command");
	return (*this);
}
Msg & Msg::errNotEnoughParam(const std::string &nick, const std::string &cmd)
{
	setPrefix(SERVER_PREFIX);
	numeric(461);
	addParam(nick);
	addParam(cmd);
	addParam("Not enough parameters");
	return (*this);
}
Msg & Msg::errNoSuchNick(const std::string &nick, const std::string &target)
{
	setPrefix(SERVER_PREFIX);
	numeric(401);
	addParam(nick);
	addParam(target);
	addParam("No such nick/channel");
	return (*this);
}
Msg & Msg::errNoNickGiven(const std::string &target)
{
    setPrefix(SERVER_PREFIX);
    numeric(431);
    addParam(target);
    addParam("No nickname given");
    return (*this);
}
Msg & Msg::errOneousNick(const std::string &target,
                                  const std::string &nick)
{
    setPrefix(SERVER_PREFIX);
    numeric(432);
    addParam(target);
    addParam(nick);
    addParam("Erroneous nickname");
    return (*this);
}
Msg & Msg::errNickInUse(const std::string &target,
                              const std::string &nick)
{
    setPrefix(SERVER_PREFIX);
    numeric(433);
    addParam(target);
    addParam(nick);
    addParam("Nickname is already in use");
    return (*this);
}
Msg & Msg::errNoSuchChannel(const std::string &nick, const std::string &channel)
{
	setPrefix(SERVER_PREFIX);
	numeric(403);
	addParam(nick);
	addParam(channel);
	addParam("No such channel");
	return (*this);
}
Msg & Msg::errCantSendToChan(const std::string &nick, const std::string &channel)
{
	setPrefix(SERVER_PREFIX);
	numeric(404);
	addParam(nick);
	addParam(channel);
	addParam("Cannot send to channel");
	return (*this);
}
Msg & Msg::errNotOnChannel(const std::string &nick, const std::string &channel)
{
	setPrefix(SERVER_PREFIX);
	numeric(442);
	addParam(nick);
	addParam(channel);
	addParam("You're not on that channel");
	return (*this);
}
Msg & Msg::errUserOnChannel(const std::string &nick, const std::string &user, const std::string &channel)
{
	setPrefix(SERVER_PREFIX);
	numeric(443);
	addParam(nick);
	addParam(user);
	addParam(channel);
	addParam("is already on channel");
	return (*this);
}
Msg & Msg::errNotRegistered( const std::string &nick )
{
	setPrefix(SERVER_PREFIX);
	numeric(451);
	addParam(nick);
	addParam("You have not registered");
	return (*this);
}
Msg & Msg::errAlreadyRegist(const std::string &nick)
{
    setPrefix(SERVER_PREFIX);
    numeric(462);
    addParam(nick);
    addParam("You may not reregister");
    return (*this);
}
Msg & Msg::errPasswdMismatch(const std::string &nick)
{
    setPrefix(SERVER_PREFIX);
    numeric(464);
    addParam(nick);
    addParam("Password incorrect");
    return (*this);
}
Msg & Msg::errChannelIsFull(const std::string &nick,
                                  const std::string &channel)
{
    setPrefix(SERVER_PREFIX);
    numeric(471);
    addParam(nick);
    addParam(channel);
    addParam("Cannot join channel (+l)");
    return (*this);
}
Msg & Msg::errInviteOnlyChan(const std::string &nick,
                                 const std::string &channel)
{
    setPrefix(SERVER_PREFIX);
    numeric(473);
    addParam(nick);
    addParam(channel);
    addParam("Cannot join channel (+i)");
    return (*this);
}
Msg & Msg::errBadChannelKey(const std::string &nick,
                                  const std::string &channel)
{
    setPrefix(SERVER_PREFIX);
    numeric(475);
    addParam(nick);
    addParam(channel);
    addParam("Cannot join channel (+k)");
    return (*this);
}
Msg & Msg::errChanOpPrivsNeeded(const std::string &nick,
                                      const std::string &channel)
{
    setPrefix(SERVER_PREFIX);
    numeric(482);
    addParam(nick);
    addParam(channel);
    addParam("You're not channel operator");
    return (*this);
}

// =========================================================================================
// RPL
// =========================================================================================

// 001 RPL_WELCOME
Msg & Msg::rplWelcome(const std::string &nick,
                            const std::string &user,
                            const std::string &host)
{
    setPrefix(SERVER_PREFIX);
    numeric(1);
    addParam(nick);
    addParam("Welcome to the Internet Relay Network "
             + nick + "!" + user + "@" + host);
    return (*this);
}

// 002 RPL_YOURHOST
Msg & Msg::rplYourHost(const std::string &nick,
                             const std::string &serverName,
                             const std::string &version)
{
    setPrefix(SERVER_PREFIX);
    numeric(2);
    addParam(nick);
    addParam("Your host is " + serverName + ", running version " + version);
    return (*this);
}

// 003 RPL_CREATED
Msg & Msg::rplCreated(const std::string &nick,
                            const std::string &date)
{
    setPrefix(SERVER_PREFIX);
    numeric(3);
    addParam(nick);
    addParam("This server was created " + date);
    return (*this);
}

// 004 RPL_MYINFO
Msg & Msg::rplMyInfo(const std::string &nick,
                           const std::string &serverName,
                           const std::string &version,
                           const std::string &userModes,
                           const std::string &channelModes)
{
    setPrefix(SERVER_PREFIX);
    numeric(4);
    addParam(nick);
    addParam(serverName);
    addParam(version);
    addParam(userModes);
    addParam(channelModes);
    return (*this);
}

// 331 RPL_NOTOPIC
Msg & Msg::rplNoTopic(const std::string &nick,
                            const std::string &channel)
{
    setPrefix(SERVER_PREFIX);
    numeric(331);
    addParam(nick);
    addParam(channel);
    addParam("No topic is set");
    return (*this);
}

// 332 RPL_TOPIC
Msg & Msg::rplTopic(const std::string &nick,
                          const std::string &channel,
                          const std::string &topic)
{
    setPrefix(SERVER_PREFIX);
    numeric(332);
    addParam(nick);
    addParam(channel);
    addParam(topic); // trailing으로 나감
    return (*this);
}

// 341 RPL_INVITING
Msg & Msg::rplInviting(const std::string &nick,
                             const std::string &channel,
                             const std::string &target)
{
    setPrefix(SERVER_PREFIX);
    numeric(341);
    addParam(nick);
    addParam(channel);
    addParam(target);
    return (*this);
}

// 353 RPL_NAMREPLY
// 여기서는 간단히 RFC1459 스타일: "= <channel> :names"
Msg & Msg::rplNamReply(const std::string &nick,
                             const std::string &channel,
                             const std::string &names)
{
    setPrefix(SERVER_PREFIX);
    numeric(353);
    addParam(nick);
    addParam("=");
    addParam(channel);
    addParam(names); // "nick1 nick2" 같은 리스트
    return (*this);
}

// 366 RPL_ENDOFNAMES
Msg & Msg::rplEndOfNames(const std::string &nick,
                               const std::string &channel)
{
    setPrefix(SERVER_PREFIX);
    numeric(366);
    addParam(nick);
    addParam(channel);
    addParam("End of NAMES list");
    return (*this);
}

