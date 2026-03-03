#include "MyLibft.hpp"

#include <climits>
#include <cctype>
#include <iostream>

bool	MyLibft::aToInt(const std::string &s, int &out)
{
	const char	*p = s.c_str();

	while (*p && std::isspace(static_cast<unsigned char>(*p)))
		++p;

	bool	negative = false;
	if (*p == '+' || *p == '-')
	{
		if (*p == '-')
			negative = true;
		++p;
	}

	if (!std::isdigit(static_cast<unsigned char>(*p)))
		return false;

	int	n = 0;

	while (*p && std::isdigit(static_cast<unsigned char>(*p)))
	{
		int	d = *p - '0';

		if (!negative)
		{
			if (n > INT_MAX / 10 || (n == INT_MAX / 10 && d > INT_MAX % 10))
				return false;
			n = n * 10 + d;
		}
		else
		{
			if (n < INT_MIN / 10 || (n == INT_MIN / 10 && -d < INT_MIN % 10))
				return false;
			n = n * 10 - d;
		}
		++p;
	}

	out = n;
	return true;
}

void MyLibft::myMemset(void *b, int c, size_t len)
{
	unsigned char *ptr = static_cast<unsigned char *>(b);
	unsigned char val = static_cast<unsigned char>(c);
	for (size_t i = 0; i < len; ++i)
		ptr[i] = val;
}
void MyLibft::setLingerZero( int fd )
{
	struct linger ling = {1, 0};
	setsockopt(fd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
}
void MyLibft::showBuffer( std::string &str )
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
