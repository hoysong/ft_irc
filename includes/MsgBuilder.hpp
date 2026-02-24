#ifndef MSGBUILDER_HPP
# define MSGBUILDER_HPP
# include "ircError.hpp"
# include "Client.hpp"
# include <sstream>
# include <vector>

class MsgBuilder
{
	protected:
//		std::string m_serverName;
		int m_errno;
		std::string m_clientName;
		std::vector<std::string> m_params;
		std::string m_msg;
	public:
//		MsgBuilder &setServerName( const std::string &serverName );
		MsgBuilder &setErrno( int errCode );
		MsgBuilder &setClientName( const std::string &clientName );
		MsgBuilder &addParam( const std::string &param );
		MsgBuilder &setMessage( const std::string &message );
		std::string toString( void ) const;
		void clear(void); // must clear after use.

		/* 편의함수(내부에서 자동조립). */
		std::string buildErrMsg( e_ircError errCode,
				Client &client,
				std::string param );
		std::string buildErrMsg( e_ircError errCode,
				Client &client,
				std::string param1,
				std::string param2);
		std::string buildErrMsg( e_ircError errCode,
				Client &client,
				std::string param1,
				std::string param2,
				std::string param3);
};

#endif
