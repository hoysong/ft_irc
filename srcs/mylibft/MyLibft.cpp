#include "MyLibft.hpp"
#include <climits>
#include <cctype>
#include <sstream>

bool	MyLibft::aToInt(const std::string &s, int &out)
{
	std::istringstream iss(s);

	if (!(iss >> out))
		return (false);
	else if (!iss.eof())
		return (false);
	return (true);
}
