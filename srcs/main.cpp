#include "IRCServer.hpp"
#include "MyLibft.hpp"
#include <iostream>

/*분산 책임 클래스 설계.
 * ListenSocket은 정말 연결요청만 받는 클래스로 설계합니다.
 * Client는 연결 요청 후 생성되며 Config로부터 추종해야 하는 서버 설정을 받아냅니다.
 * + 그럼 여기서 Client는 Server 클래스를 'Server *'와 같이 포인터로 기억하도록 하면 편할 듯 합니다.
 * + ListenSocket은.. Server블록 기반으로 생성되니 생성과 동시에 서버블록을 기억하도록 설계, Client accept시 Client의 Serverptr에 박아두면 될 듯 합니다.
 * + ClientManager::addNewClient( Server server, fd ); 이런식으로 하면 되나?
 */

static void startErrMsg( void )
{
	std::cerr << "서버 구동: ./ircserv [port] [passwd]" << std::endl;
	std::cout << "[passwd]를 구성할 수 있는 문자들은 다음과 같습니다." << std::endl;
	std::cout << "├─ A ~ Z" << std::endl;
	std::cout << "├─ a ~ z" << std::endl;
	std::cout << "└─ 0 ~ 9" << std::endl;
}

#include <cctype>
static bool paswdVldChk(const std::string &passwd)
{
	for (size_t i = 0; passwd[i] != '\0'; i++)
	{
		if (!std::isalnum(static_cast<unsigned char>(passwd[i])))
			return (false);
	}
	return (true);
}

int	main(int argc, char **argv)
{
	/* 인자 유효성 체크가 추후 필요합니다.
	 * 1. 오버플로우.
	 * 2. 숫자가 아닌 문자의 감지.
	 * 3. 인자는 오직 port, passwd만 받습니다.
	 */
	int port;
	if (argc != 3)
	{ 
		startErrMsg();
		return (1);
	}
	else if (std::string(argv[1]).empty() || std::string(argv[2]).empty())
	{
		startErrMsg();
		return (1);
	}
	else if (!MyLibft::aToInt(argv[1], port))
	{
		startErrMsg();
		return (1);
	}
	else if (!paswdVldChk(argv[2]))
	{
		startErrMsg();
		return (1);
	}

	try
	{
		IRCServer ircServ("0.0.0.0", port, argv[2]);
		ircServ.serverLoop();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	std::cout << "ft_irc 서버가 종료되었습니다." << std::endl;
}
