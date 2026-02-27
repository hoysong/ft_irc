#ifndef MSGHDLER_HPP
# define MSGHDLER_HPP
# include "ircError.hpp"
# include <string>

class Client;

bool sendMsg( int fd, const std::string &buf );

std::string errMsg( e_ircError errCode,
		Client &client,
		std::string param );

std::string errMsg( e_ircError errCode,
		Client &client,
		std::string param1,
		std::string param2);

std::string errMsg( e_ircError errCode,
		Client &client,
		std::string param1,
		std::string param2,
		std::string param3);

std::string goodMsg( Client &client,
		const std::string &command,
		const std::string &param1);

std::string goodMsg( Client &client,
		const std::string &param1,
		const std::string &param2,
		const std::string &param3);

std::string notEnoughParam(Client &client, const std::string &command);

#endif
