#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "IServerController.hpp"
# include <sys/socket.h> // recv(), ssize_t.
# include <string>
# include <map>

class Channel;

class Client
{
	private:
		IServerController &m_server;
		const int m_fd;
		std::string m_buffer;
		void appendBuffer( char *buffer, ssize_t size );
		std::map<std::string, Channel *> m_channels;

		bool m_authed;
		bool m_registered;
		bool m_IRCOperaotr;
		bool m_modInvisible;
		bool m_modWallops;

		std::string m_nickName;
		std::string m_userName;
		std::string m_realName;
	public:
		Client( int fd, IServerController &ircServer );
		~Client( void );

		bool recvFd( void );
		int getFd( void ) const;

		bool popLine( std::string &line );// CRLF기반라인(\r\n 제외)을 line에 넣습니다.
		void assignNickName( const std::string &name ); // 메니저로부터 이름 부여받음.

		bool isAuthed( void ); // client가 등록되었는지 확인합니다.
		bool isRegistered( void );
		bool isInvisible( void );
		bool isWallopos( void );
		bool isInChannel( const std::string &channelName );

		std::string getHost( void );
		std::string getNickName( void );
		std::string getUserName( void );
		std::string getRealName( void );
		std::string getMsgPrefix( void );
		std::map<std::string, Channel *> getJoinedChannel( void );

		void setAuthed( void );
		void setRegistered( void );

		void setUserName( const std::string &str );
		void setInvisible( bool flag );
		void setWallops( bool flag );
		void setRealName( const std::string &name );
		void addJoinedChannel( Channel &channel );
		bool removeJoinedChannel( Channel &channel );
		void quitAllChannels( void );
		bool operator < (const Client &ref ) const;
		void announce( void );
};

#endif
