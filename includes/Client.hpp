#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <vector>

class Client
{
	private:
		const int m_fd;
//		char dummmmy[1024 * 1024];
	public:
		bool recvBuffer( void );
		int getFd( void ) const;
		Client( int fd );
		~Client( void );
};

#endif
