#include "MsgBuilder.hpp"
#include <iostream>

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
	MsgBuilder msg;
	msg.m_errno = -1;
	msg.m_clientName.clear();
	msg.m_params.clear();
	msg.m_msg.clear();
}

std::string MsgBuilder::buildErrMsg( e_ircError errCode,
		Client &client,
		std::string param )
{
	MsgBuilder msg;
	msg.clear();
	msg.setErrno(errCode);
	msg.setClientName(client.getNickName());
	msg.setMessage(param);
	return (msg.toString());
}
std::string MsgBuilder::buildErrMsg( e_ircError errCode,
		Client &client,
		std::string param1,
		std::string param2)
{
	MsgBuilder msg;
	msg.clear();
	msg.setErrno(errCode);
	msg.setClientName(client.getNickName());
	msg.addParam(param1);
	msg.setMessage(param2);
	return (msg.toString());
}
std::string MsgBuilder::buildErrMsg( e_ircError errCode,
		Client &client,
		std::string param1,
		std::string param2,
		std::string param3)
{
	MsgBuilder msg;
	msg.clear();
	msg.setErrno(errCode);
	msg.setClientName(client.getNickName());
	msg.addParam(param1);
	msg.addParam(param2);
	msg.setMessage(param3);
	return (msg.toString());
}

std::string MsgBuilder::buildSendMsg( Client &client,
		const std::string &command,
		const std::string &param1)
{
	std::string msg;
	msg += client.getMsgPrefix() + " ";
	msg += command;
	msg += " :" + param1;
	return (msg);
}

std::string MsgBuilder::notEnoughParam(Client &client, const std::string &command)
{
	return (buildErrMsg(ERR_NEEDMOREPARAMS, client, command, "Not enough parameters"));
}
MsgBuilder::MsgBuilder(void)
{
}
