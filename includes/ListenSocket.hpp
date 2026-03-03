#ifndef LISTENSOCKET_HPP
# define LISTENSOCKET_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>
# include <string>

class ListenSocket
{
	private:
		int					m_sockfd;
		const int					m_port;
		const std::string			m_ip;
		struct sockaddr_in	m_addr;

		// 복사 방지
		ListenSocket(const ListenSocket& other);
		ListenSocket& operator=(const ListenSocket& other);

	public:
		int acceptClient( void );

		/*getters.*/
		int getFd() const;
		int getPort() const;
		std::string getIp() const;

		/***************************/
		/* constructor/destructor. */
		/***************************/
		ListenSocket(const std::string& ip, int port);
		~ListenSocket();
};

#endif
