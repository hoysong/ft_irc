#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "Client.hpp"
# include <map>
# include <string>

class Channel
{
	private:
		std::string m_channelName;
		typedef std::map<std::string, Client *> memberMap;
		memberMap m_members;
		std::string m_passwd;
		// need channel modes.
	public:
		Channel( void );
		~Channel( void );
		Channel(const Channel &other);

		bool addMember( Client &client );
		bool removeMember( Client &client );
		bool findMember( Client &client );
		bool isChannelEmpty( void );

		std::string getPasswd( void );
		memberMap &getChannelMembers( void );

		Channel &setChannelName( const std::string &name );
		Channel &assignPasswd( const std::string &passwd );
};

#endif
