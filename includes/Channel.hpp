#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "Client.hpp"
# include <map>
# include <set>
# include <string>

class Client;
class IRCServer;

class Channel
{
	private:
		std::string m_channelName;
		typedef std::map<std::string, Client *> memberMap;
		memberMap m_members;
		std::set<Client *> m_opers;

		// need channel modes.
		bool m_inviteOnly;
		bool m_topicOpOnly;
		std::string m_passwd; // 비어있으면 -k
		int m_maxMembers;
	public:
		Channel( void );
		~Channel( void );

		bool addMember( Client &client, const std::string &passwd, IRCServer &server);
		bool removeMember( Client &client, const std::string &msg, IRCServer &server );
		bool addChannelOper( Client &client);
		bool removeChannelOper( Client &client);
		bool findMember( Client &client );
		bool isChannelEmpty( void );
		bool isChannelOper( Client &client );

		void broadcastNickChanged( Client &client, const std::string &newNick);

		std::string getPasswd( void );
		std::string getChannelName( void );
		memberMap &getChannelMembers( void );

		Channel &setChannelName( const std::string &name );
		Channel &assignPasswd( const std::string &passwd );
};

#endif
