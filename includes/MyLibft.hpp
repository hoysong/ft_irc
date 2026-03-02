#ifndef MYLIBFT_HPP
# define MYLIBFT_HPP
# include <stdexcept>
# include <sstream>
# include <iostream>
# include <cerrno>
# include <sys/socket.h>

class MyLibft
{
	private:
		MyLibft( void );
		~MyLibft( void );
		MyLibft( const MyLibft &ref );
		MyLibft operator = ( const MyLibft &ref );
	public:
		static int myAtoi(const std::string &s)
		{
			std::stringstream ss(s);
			int value = 0;
			char c;
		
			ss >> value;
			if (ss.fail())
				throw(std::runtime_error(std::string("[MyLibft::myAtoi()]: failed to ascii to intager") + s));
			ss >> c;
			if (ss.fail() != true) // 뒤에 문자가 남아있지 않아야 하는 상황.
				throw(std::runtime_error(std::string("[MyLibft::myAtoi()]: failed to ascii to intager") + s));
			return (value);
		}

		static void myMemset(void *b, int c, size_t len)
		{
			unsigned char *ptr = static_cast<unsigned char *>(b);
			unsigned char val = static_cast<unsigned char>(c);
			for (size_t i = 0; i < len; ++i)
				ptr[i] = val;
		}
		static void setLingerZero( int fd )
		{
			struct linger ling = {1, 0};
			setsockopt(fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
		}
		static void showBuffer( std::string &str )
		{
			std::string::iterator iter = str.begin();
			std::string::iterator iter_end = str.end();

			while ( iter != iter_end )
			{
				if (*iter == '\r')
					std::cout << "\\r";
				else if (*iter == '\n')
					std::cout << "\\n\n";
				else
					std::cout << *iter;
				iter++;
			}
			std::cout << std::endl;
		}
		static bool sendMsg( int fd, const std::string &buf )
		{
			size_t	total = 0;
			size_t	len = buf.size();
		
			while (total < len)
			{
				ssize_t	n = send(fd, buf.data() + total, len - total, MSG_NOSIGNAL);
				if (n > 0)
					total += static_cast<size_t>(n);
				else if (n < 0)
				{
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						return (false); // 타임아웃 또는 에러.
					else if (errno == EINTR)
						continue; // 시그널로 방해받음 다시 시도.
					else
						return (false); // 실패
				}
				else
					break; // 끝?
			}
			return (true);
		}
};

#endif
