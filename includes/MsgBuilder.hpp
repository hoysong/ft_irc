#ifndef MSGBUILDER_HPP
# define MSGBUILDER_HPP
# include "ircError.hpp"
# include "Client.hpp"
# include <sstream>
# include <vector>

class MsgBuilder
{
	protected:
		int m_errno;
		std::string m_clientName;
		std::vector<std::string> m_params;
		std::string m_msg;

		MsgBuilder &setErrno( int errCode );
		MsgBuilder &setClientName( const std::string &clientName );
		MsgBuilder &addParam( const std::string &param );
		MsgBuilder &setMessage( const std::string &message );
		std::string toString( void ) const;
		void clear(void); // must clear before use.

	public:
		/*static class 생성 금지.*/
		MsgBuilder( void );

		/* 편의함수(내부에서 자동조립). */
		static std::string buildErrMsg( e_ircError errCode,
				Client &client,
				std::string param );
		static std::string buildErrMsg( e_ircError errCode,
				Client &client,
				std::string param1,
				std::string param2);
		static std::string buildErrMsg( e_ircError errCode,
				Client &client,
				std::string param1,
				std::string param2,
				std::string param3);
		static std::string buildSendMsg( Client &client,
				const std::string &command,
				const std::string &param1);
		static std::string notEnoughParam(Client &client, const std::string &command);
};

#endif
