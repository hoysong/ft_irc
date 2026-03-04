#include "MyLibft.hpp"
#include <climits>
#include <cctype>

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
