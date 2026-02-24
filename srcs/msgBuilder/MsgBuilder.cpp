#include "MsgBuilder.hpp"
#include <iostream>

//MsgBuilder &MsgBuilder::setServerName( const std::string &serverName )
//{
//	m_serverName = serverName;
//	return (*this);
//}
MsgBuilder &MsgBuilder::setErrno( int errCode )
{
	m_errno = errCode;
	return (*this);
}
MsgBuilder &MsgBuilder::setClientName( const std::string &clientName )
{
	m_clientName = clientName;
	return (*this);
}
MsgBuilder &MsgBuilder::addParam( const std::string &param )
{
	m_params.push_back(param);
	return (*this);
}
MsgBuilder &MsgBuilder::setMessage( const std::string &message )
{
	m_msg = message;
	return (*this);
}

std::string MsgBuilder::toString( void ) const
{
	std::stringstream ss;
	ss << ':'
		<< "irc.ft_irc.42Gyeongsan.kr" << ' '
		<< m_errno << ' '
		<< m_clientName << ' ';

	std::vector<std::string>::const_iterator iter = m_params.begin();
	std::vector<std::string>::const_iterator iter_end = m_params.end();

	while (iter != iter_end)
	{
		ss << *iter << ' ';
		iter++;
	}
	ss << ':' << m_msg;
	std::string result = ss.str();
	std::cout << "[toString()]: " << result << std::endl;
//	return (ss.str());
	return (result);
}

void MsgBuilder::clear(void)
{
//	m_serverName.clear();
	m_errno = -1;
	m_clientName.clear();
	m_params.clear();
	m_msg.clear();
}

std::string MsgBuilder::buildErrMsg( e_ircError errCode,
		Client &client,
		std::string param )
{
	setErrno(errCode);
	setClientName(client.getNickName());
	setMessage(param);
	return (toString());
}
std::string MsgBuilder::buildErrMsg( e_ircError errCode,
		Client &client,
		std::string param1,
		std::string param2)
{
	setErrno(errCode);
	setClientName(client.getNickName());
	addParam(param1);
	setMessage(param2);
	return (toString());
}
std::string MsgBuilder::buildErrMsg( e_ircError errCode,
		Client &client,
		std::string param1,
		std::string param2,
		std::string param3)
{
	setErrno(errCode);
	setClientName(client.getNickName());
	addParam(param1);
	addParam(param2);
	setMessage(param3);
	return (toString());
}
