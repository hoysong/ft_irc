#ifndef MYLIBFT_HPP
# define MYLIBFT_HPP
# include <stdexcept>
# include <sstream>

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
};

#endif
