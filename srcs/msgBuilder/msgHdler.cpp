//#include "Client.hpp"
//#include "Channel.hpp"
//#include "ircError.hpp"
//#include "msgHdler.hpp"
//#include <iostream>
//#include <string>
//#include <iostream>
//#include <sstream>
//#include <cerrno>
//
//bool sendMsg( int fd, const std::string &buf )
//{
//	size_t	total = 0;
//	size_t	len = buf.size();
//
//	while (total < len)
//	{
//		ssize_t	n = send(fd, buf.data() + total, len - total, MSG_NOSIGNAL);
//		if (n > 0)
//			total += static_cast<size_t>(n);
//		else if (n < 0)
//		{
//			if (errno == EAGAIN || errno == EWOULDBLOCK)
//				return (false); // 타임아웃 또는 에러.
//			else if (errno == EINTR)
//				continue; // 시그널로 방해받음 다시 시도.
//			else
//				return (false); // 실패
//		}
//		else
//			break; // 끝?
//	}
//	return (true);
//}
//std::string errMsg( e_ircError errCode,
//		Client &client,
//		std::string param )
//{
//	std::stringstream ss;
//	ss << ":irc.ft_irc.42Gyeongsan.kr "
//		<< errCode << " "
//		<< client.getNickName() + " "
//		<< ":" + param
//		<< "\r\n";
//	return (ss.str());
//}
//std::string errMsg( e_ircError errCode,
//		Client &client,
//		std::string param1,
//		std::string param2)
//{
//	std::stringstream ss;
//	ss << ":irc.ft_irc.42Gyeongsan.kr "
//		<< errCode << " "
//		<< client.getNickName() + " "
//		<< param1 + " "
//		<< ":" + param2
//		<< "\r\n";
//	return (ss.str());
//}
//std::string errMsg( e_ircError errCode,
//		Client &client,
//		std::string param1,
//		std::string param2,
//		std::string param3)
//{
//	std::stringstream ss;
//	ss << ":irc.ft_irc.42Gyeongsan.kr "
//		<< errCode << " "
//		<< client.getNickName() + " "
//		<< param1 + " "
//		<< param2 + " "
//		<< ":" + param3
//		<< "\r\n";
//	return (ss.str());
//}
//
//
//std::string rplUserMode(Client &client)
//{
//	std::stringstream ss;
//	ss << ":irc.ft_irc.42Gyeongsan.kr "
//		<< RPL_UMODEIS << " "
//		<< client.getNickName() + " "
//		<< ":+";
//	if (client.isInvisible())
//		ss << 'i';
//	if (client.isWallopos())
//		ss << 'w';
//	ss << "\r\n";
//	return (ss.str());
//}
//
//std::string rplChannelMode(Client &client, Channel &channel)
//{
//	std::stringstream ss;
//	ss << ":irc.ft_irc.42Gyeongsan.kr "
//		<< RPL_CHANNELMODEIS << " "
//		<< client.getNickName() + " "
//		<< channel.getChannelName() + " "
//		<< channel.modeToString();
//	ss << "\r\n";
//	return (ss.str());
//}
//
//std::string chanModeDone(e_rpl_numeric errCode,
//		Client &client,
//		const std::string &param1,
//		const std::string &param2)
//{
//	std::stringstream ss;
//	ss << ":irc.ft_irc.42Gyeongsan.kr "
//		<< errCode << " "
//		<< client.getNickName() + " "
//		<< param1 + " "
//		<< param2
//		<< "\r\n";
//	return (ss.str());
//}
//
//std::string noTrailingMsg(e_rpl_numeric errCode,
//		Client &client,
//		const std::string &param1,
//		const std::string &param2)
//{
//	std::stringstream ss;
//	ss << ":irc.ft_irc.42Gyeongsan.kr "
//		<< errCode << " "
//		<< client.getNickName() + " "
//		<< param1 + " "
//		<< param2
//		<< "\r\n";
//	return (ss.str());
//}
//
//std::string goodMsg( e_rpl_numeric errCode,
//		Client &client,
//		const std::string &param1,
//		const std::string &param2)
//{
//	std::stringstream ss;
//	ss << ":irc.ft_irc.42Gyeongsan.kr "
//		<< errCode << " "
//		<< client.getNickName() + " "
//		<< param1 + " "
//		<< ":" + param2
//		<< "\r\n";
//	return (ss.str());
//}
//
//std::string goodMsg( e_rpl_numeric errCode,
//		Client &client,
//		const std::string &param1,
//		const std::string &param2,
//		const std::string &param3)
//{
//	std::stringstream ss;
//	ss << ":irc.ft_irc.42Gyeongsan.kr "
//		<< errCode << " "
//		<< client.getNickName() + " "
//		<< param1 + " "
//		<< param2 + " "
//		<< ":" + param3
//		<< "\r\n";
//	return (ss.str());
//}
//
//std::string goodMsg( Client &client,
//		const std::string &param1,
//		const std::string &param2)
//{
//	std::string msg;
//	msg += client.getMsgPrefix() + " ";
//	msg += param1 + " ";
//	msg += ":" + param2;
//	msg += "\r\n";
//	return (msg);
//}
//
//std::string goodMsg( Client &client,
//		const std::string &param1,
//		const std::string &param2,
//		const std::string &param3)
//{
//	std::string msg;
//	msg += client.getMsgPrefix() + " ";
//	msg += param1 + " ";
//	msg += param2 + " ";
//	msg += ":" + param3;
//	msg += "\r\n";
//	return (msg);
//}
//
//std::string goodMsg( Client &client,
//		const std::string &param1,
//		const std::string &param2,
//		const std::string &param3,
//		const std::string &param4)
//{
//	std::string msg;
//	msg += client.getMsgPrefix() + " ";
//	msg += param1 + " ";
//	msg += param2 + " ";
//	msg += param3 + " ";
//	msg += ":" + param4;
//	msg += "\r\n";
//	return (msg);
//}
//std::string notEnoughParam(Client &client, const std::string &command)
//{
//	return (errMsg(ERR_NEEDMOREPARAMS, client, command, "Not enough parameters"));
//}
