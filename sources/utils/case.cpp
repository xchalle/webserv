#include "../../include/utils.hpp"
#include <cctype>

std::string to_uppercase(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		*it = toupper(*it);
	}
	return str;
}