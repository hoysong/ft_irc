#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "Client.hpp"
# include <map>
# include <string>

class Channel
{
	private:
		const std::string m_channelName;
		std::map<std::string, Client &> m_members;
		// need channel modes.
	public:
		Channel( const std::string &str );
		Channel( const std::string &str, const std::string &passwd );
		~Channel( void );
		void addMember( Client &client );
		void removeMember( Client &client );
		bool isChannelEmpty( void );
};

#endif
