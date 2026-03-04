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
};

#endif
