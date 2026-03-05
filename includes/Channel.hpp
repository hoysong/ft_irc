#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "IServerController.hpp"
# include "Client.hpp"
# include <map>
# include <set>
# include <string>

class Client;
class IRCServer;

class Channel
{
	private:
		IServerController &m_server;
		std::string m_channelName;
		typedef std::map<std::string, Client *> memberMap;
		memberMap m_members;
		std::set<Client *> m_opers;
		std::string m_topic;

		// need channel modes.
		std::string m_passwd; // 비어있으면 -k
		bool m_inviteOnly; // mode i
		bool m_topicOpOnly; // mode t
		int m_maxMembers; // mode l(limit).
	public:
		Channel( IServerController &ircServer );
		~Channel( void );

		bool addMember( Client &client, const std::string &passwd);
		bool removeMember( Client &client );
		bool removeMember( const std::string &target );
		bool inviteMember( Client &client, Client &invited );

		bool addChannelOper( Client &client);
		bool addChannelOper( const std::string &nickName );
		bool removeChannelOper( Client &client);
		bool removeChannelOper( const std::string &nickName );

		bool findMember( Client &client );
		bool findMember( const std::string &name );

		bool isChannelEmpty( void );
		bool isChannelOper( Client &client );
		bool isChannelOper( const std::string &nickName );
		bool isInviteMode( void );
		bool isTopicMode( void );
		bool isKeyMode( void );
		bool isLimitMode( void );

		void setInviteMode( bool );
		void setTopic( const std::string &topic );
		void setTopicMode( bool );
		void setKeyMode( const std::string &value );
		void setLimitMode( int value );
		bool setLimitMode( const std::string &value );

		void syncNick( Client &client, const std::string &newNick);
		bool broadcastPrivmsg(Client &client, const std::string &msg);
		bool broadcastMsg( const std::string &msg );

		std::string getPasswd( void );
		std::string getChannelName( void );
		memberMap &getChannelMembers( void );
		Client &getChannelMember( const std::string &nickName );
		std::string getStringChannelLimit( void );
		int getIntChannelLimit( void );
		std::string getTopic( void );
		std::string modeToString( void );

		Channel &setChannelName( const std::string &name );
		Channel &assignPasswd( const std::string &passwd );

		void newMemberBroadcast(Client &client);
		void quitBroadcast(Client &client, const std::string &msg);
		void announce( void );
};

#endif
