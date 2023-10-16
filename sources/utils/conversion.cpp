#include <string>
#include <stdexcept>
#include <iostream>

int str_to_int(const std::string &s)
{
	bool	negative = false;
	int		result = 0;
	size_t	i = 0;

	if (s.empty())
		throw std::invalid_argument("Empty string argument");
	negative = (s[i] == '-');
	if (negative)
		i++;
	if (s[i] == '+' || s[i] == '-')
		++i;
	if (s[i] == '\0')
		throw std::invalid_argument("string argument contains only a sign");
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			throw std::invalid_argument("invalid input string");
		result = result * 10 - (s[i] - '0');
		++i;
	}
	return negative ? result : -result;
}

std::string	int_to_str(int n)
{
	std::string	result;
	bool		negative = false;

	if (n == 0)
		return "0";
	if (n < 0)
	{
		negative = true;
		n = -n;
	}
	while (n)
	{
		result = static_cast<char>(n % 10 + '0') + result;
		n /= 10;
	}
	if (negative)
		result = '-' + result;
	return result;
}