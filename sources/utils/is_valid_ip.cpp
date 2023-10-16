#include "../../include/utils.hpp"
#include <string>
#include <vector>

bool is_valid_ipv4(const std::string &ip)
{
	if (ip == "localhost")
		return true;
	std::vector<std::string> parts;
	std::string part;

	parts = split(ip, ".");
	if (parts.size() != 4)
		return false;
	for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it)
	{
		char *endptr;
		long num = strtol(it->c_str(), &endptr, 10);
		
		if (endptr == it->c_str())
			return false;
		if (num < 0 || num > 255)
			return false;
	}
	return true;
}
