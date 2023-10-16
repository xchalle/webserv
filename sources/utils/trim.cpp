#include "../../include/utils.hpp"

std::string ltrim(const std::string &s, std::string to_replace)
{
	std::string str = s;
	str.erase(0, str.find_first_not_of(to_replace));
	return (str);
}

std::string rtrim(const std::string &s, std::string to_replace)
{
	std::string str = s;
	str.erase(str.find_last_not_of(to_replace) + 1);
	return (str);
}

std::string trim(std::string &str, std::string to_replace)
{
	return (rtrim(ltrim(str, to_replace), to_replace));
}