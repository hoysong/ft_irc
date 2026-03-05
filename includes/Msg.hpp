#ifndef MSG_HPP
# define MSG_HPP
# include "Client.hpp"
# include <string>
# include <vector>
# define SERVER_PREFIX "ft_irc.local"

bool sendMsg( int fd, const std::string &buf );

class Msg
{
	private:
		std::string m_prefix;
		int m_numeric;
		std::vector<std::string> m_params;
		bool m_trailing;
	public:
		/*직접 메시지를 빌드하는 경우 사용가능.*/
		Msg( void );
		Msg & setPrefix( const std::string &prefix );
		Msg & numeric( int errOrRpl );
		Msg & addParam( const std::string &param );
		Msg & trailing( bool flag );
		Msg & clear( void );
		bool sendTo( int fd );
		/*조립한 내용들을 메시지로 이어붙이기.*/
		std::string serialize( void );

		/*미리 지정한 함수들.
		 * 사용하기 편하기 위해 작성된 함수들.
		 * 내부적으로 setPrefix, numeric, addParam 등을 호출.
		 */
		//ERR_UNKNOWNCOMMAND (421)
		//:servername 421 <nick> <command> :Unknown command
		Msg & errUnknownCmd(const std::string &nick, const std::string &cmd);

		//ERR_NEEDMOREPARAMS (461)
		//:servername 461 <nick> <command> :Not enough parameters
		Msg & errNotEnoughParam(const std::string &nick, const std::string &cmd);

		//ERR_NOSUCHNICK (401)
		//:servername 401 <nick> <target> :No such nick/channel
		Msg & errNoSuchNick(const std::string &nick, const std::string &target);

		// ERR_NONICKNAMEGIVEN (431)
		// :servername 431 <target> :No nickname given
		Msg & errNoNickGiven(const std::string &target);
		
		// ERR_ERRONEUSNICKNAME (432)
		// :servername 432 <target> <nick> :Erroneous nickname
		Msg & errOneousNick(const std::string &target,
				const std::string &nick);
		
		// ERR_NICKNAMEINUSE (433)
		// :servername 433 <target> <nick> :Nickname is already in use
		Msg & errNickInUse(const std::string &target,
				const std::string &nick);

		//ERR_NOSUCHCHANNEL (403)
		//:servername 403 <nick> <channel> :No such channel
		Msg & errNoSuchChannel(const std::string &nick, const std::string &channel);

		//ERR_CANNOTSENDTOCHAN (404)
		//:servername 404 <nick> <channel> :Cannot send to channel
		Msg & errCantSendToChan(const std::string &nick, const std::string &channel);

		//ERR_NOTONCHANNEL (442)
		//:servername 442 <nick> <channel> :You're not on that channel
		Msg & errNotOnChannel(const std::string &nick, const std::string &channel);

		//ERR_USERONCHANNEL (443)
		//:servername 443 <nick> <user> <channel> :is already on channel
		Msg & errUserOnChannel(
				const std::string &nick,
				const std::string &user,
				const std::string &channel);
		//ERR_NOTREGISTERED (451)
		//:servername 451 <nick> :You have not registered
		Msg & errNotRegistered( const std::string &nick );

		//ERR_ALREADYREGISTRED (462)
		//:servername 462 <nick> :You may not reregister
		Msg & errAlreadyRegist( const std::string &nick );

		// ERR_PASSWDMISMATCH (464)
		// :servername 464 <nick> :Password incorrect
		Msg & errPasswdMismatch(const std::string &nick);
		
		// ERR_CHANNELISFULL (471)
		// :servername 471 <nick> <channel> :Cannot join channel (+l)
		Msg & errChannelIsFull(const std::string &nick,
				const std::string &channel);
		
		// ERR_INVITEONLYCHAN (473)
		// :servername 473 <nick> <channel> :Cannot join channel (+i)
		Msg & errInviteOnlyChan(const std::string &nick,
				const std::string &channel);
		
		// ERR_BADCHANNELKEY (475)
		// :servername 475 <nick> <channel> :Cannot join channel (+k)
		Msg & errBadChannelKey(const std::string &nick,
				const std::string &channel);
		
		// ERR_CHANOPRIVSNEEDED (482)
		// :servername 482 <nick> <channel> :You're not channel operator
		Msg & errChanOpPrivsNeeded(const std::string &nick,
				const std::string &channel);

		// =========================================================================================
		// RPL
		// =========================================================================================

		// RPL_WELCOME (001)
		// :servername 001 <nick> :Welcome to the Internet Relay Network <nick>!<user>@<host>
		Msg & rplWelcome(const std::string &nick,
				const std::string &user,
				const std::string &host);
		
		// RPL_YOURHOST (002)
		// :servername 002 <nick> :Your host is <servername>, running version <ver>
		Msg & rplYourHost(const std::string &nick,
				const std::string &serverName,
				const std::string &version);
		
		// RPL_CREATED (003)
		// :servername 003 <nick> :This server was created <date>
		Msg & rplCreated(const std::string &nick,
				const std::string &date);
		
		// RPL_MYINFO (004)
		// :servername 004 <nick> <servername> <version> <usermodes> <chanmodes>
		Msg & rplMyInfo(const std::string &nick,
				const std::string &serverName,
				const std::string &version,
				const std::string &userModes,
				const std::string &channelModes);
		
		// RPL_NOTOPIC (331)
		// :servername 331 <nick> <channel> :No topic is set
		Msg & rplNoTopic(const std::string &nick,
				const std::string &channel);
		
		// RPL_TOPIC (332)
		// :servername 332 <nick> <channel> :<topic>
		Msg & rplTopic(const std::string &nick,
				const std::string &channel,
				const std::string &topic);
		
		// RPL_INVITING (341)
		// :servername 341 <nick> <channel> <target>
		Msg & rplInviting(const std::string &nick,
				const std::string &channel,
				const std::string &target);
		
		// RPL_NAMREPLY (353)
		// :servername 353 <nick> = <channel> :<names>
		Msg & rplNamReply(const std::string &nick,
				const std::string &channel,
				const std::string &names);
		
		// RPL_ENDOFNAMES (366)
		// :servername 366 <nick> <channel> :End of NAMES list
		Msg & rplEndOfNames(const std::string &nick,
				const std::string &channel);
		// 324
		Msg & rplChannelModeIs(const std::string &nick,
				const std::string &channelName,
				const std::string &modes);
};

class SendFailExcept : public std::exception
{
	private:
		const std::string m_msg;
	public :
		SendFailExcept( void ) : m_msg("failed to send.") { };
		SendFailExcept( Client &client ) : m_msg(client.getNickName()) { };
		SendFailExcept( const std::string &whatMsg ) : m_msg(whatMsg) { };
		~SendFailExcept( void ) throw() { };
		const char * what() const throw()
		{
			return (m_msg.c_str());
		};
};
#endif
