#include "IRCServer.hpp"
#include <vector>

void getPrefix( std::string &line )
{
	size_t pos = line.find(' ');
	if ( pos == std::string::npos )
		return ;
	line.erase(0, pos + 1);
	return ;
}

bool splitUntilChar( std::string &line, std::string seperator, std::string &result )
{
	size_t pos;

	pos = line.find( seperator );
	if ( pos == std::string::npos )
		return (false);
	result = line.substr(0, pos);
	line.erase(0, pos + seperator.size());
	return (true);
}

bool getToken( std::string &line, std::string &result )
{
	size_t pos;

	pos = line.find(' ');
	if ( pos == std::string::npos )
		return (false);
	result = line.substr(0, pos);

	size_t charPos;
	charPos = line.find_first_not_of(' ', pos);
	if ( pos == std::string::npos)
		return (false);
	line.erase(0, charPos);
	return (true);
}

void tokenizeLine( std::string &line, t_message &result )
{
	/* 처음 공백 스킵. */
	size_t pos = line.find_first_not_of(' ');
	if ( pos != std::string::npos )
		line.erase(0, pos);

	/* 전체 line 토크나이징. */
	for( std::string buff; getToken(line, buff); )
	{
		result.params.push_back(buff);
		if ( line[0] == ':')
		{
			line.erase(0, 1);
			result.params.push_back(line);
			line.clear();
			break ;
		}
	}
	if (!line.empty())
	{
		result.params.push_back(line);
		line.clear();
	}

	/* 첫 둘 토큰들 prefix/command에 담기. */
	if (!result.params.empty())
	{ // prefix 넣기.
		if (result.params.front()[0] == ':')
		{
			result.prefix = result.params.front();
			result.prefix.erase(0, 1);
			result.params.erase( result.params.begin() );
		}
	}
	if (!result.params.empty())
	{ // command 넣기.
		result.command = result.params.front();
		result.params.erase( result.params.begin() );
	}
	std::cout << "prefix: \"" << result.prefix << "\"" << std::endl;
	std::cout << "Command: \"" << result.command << "\"" << std::endl;
	std::cout << "[params]" << std::endl;
	for(std::vector<std::string>::iterator iter = result.params.begin(); iter != result.params.end(); iter++)
		std::cout << "\"" << *iter << "\""<< std::endl;
}

void IRCServer::dispatch( Client &client, t_message &message )
{
	std::cout << "\t[IRCServer::dispatch()]" << std::endl;
	std::vector<std::string> &params = message.params;

	if (message.command == "QUIT")
	{
		handleQuit(client, params);
		return ;
	}

	/*테스트 하는동안 주석처리.*/
	if (!client.isRegistered()
		&& message.command != "PASS"
		&& message.command != "USER"
		&& message.command != "NICK"
		)
	{
		sendMsg(client.getFd(), errMsg(ERR_NOTREGISTERED, client, "You have not registered yet"));
		return ;
	}
	/* 이제 command에 따른 적절한 dispatch. */
	std::map<std::string, commandHandler>::iterator \
		iter = m_commands.find(message.command);
	if (iter == m_commands.end())
	{ // 없는 명령이거나 못찾음.
		std::string str;

		sendMsg(client.getFd(), errMsg(ERR_UNKNOWNCOMMAND, client, message.command, "Unknown command"));
		return ;
	}
	std::cout << "command found: " << message.command << std::endl;
	(this->*(iter->second))(client, message.params);
}

void IRCServer::processLine( Client &client, std::string &line)
{
	std::cout << "\t[IRCServer::processLine()]" << std::endl;
	t_message message;

	tokenizeLine(line, message);
	if (message.command.empty())
		return ; // 비어있으면 조용히 지나가기.
	dispatch(client, message);
}
