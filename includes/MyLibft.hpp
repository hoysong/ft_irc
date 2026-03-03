#ifndef MYLIBFT_HPP
# define MYLIBFT_HPP
# include <string>
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
		static bool aToInt(const std::string &s, int &out);
		static void myMemset(void *b, int c, size_t len);
		static void setLingerZero( int fd );
		static void showBuffer( std::string &str );
};

#endif
