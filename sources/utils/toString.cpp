#include "../../include/utils.hpp"

std::string to_string(int n)
{
	std::string s = "";
	if (n == 0)
		return ("0");
	else if (n == -2147483648)
		return ("-2147483648");
	else if (n < 0)
		n = -n;
	while (n > 0)
	{
		s += (n % 10) + '0';
		n /= 10;
	}
	std::reverse(s.begin() + (s[0] == '-'), s.end());
	return (s);
}

std::string to_string(unsigned int n)
{
	std::string s = "";

	if (n == 0)
		return ("0");
	while (n > 0)
	{
		s += (n % 10) + '0';
		n /= 10;
	}
	std::reverse(s.begin(), s.end());
	return (s);
}

std::string	to_string(size_t n)
{
	std::string s = "";

	if (n == 0)
		return ("0");
	while (n > 0)
	{
		s += (n % 10) + '0';
		n /= 10;
	}
	std::reverse(s.begin(), s.end());
	return (s);
}

std::string to_string(long long n)
{
	std::string s = "";

	if (n == 0)
		return ("0");
	else if (n == -9223372036854775807)
		return ("-9223372036854775807");
	else if (n < 0)
		n = -n;
	while (n > 0)
	{
		s += (n % 10) + '0';
		n /= 10;
	}
	std::reverse(s.begin() + (s[0] == '-'), s.end());
	return (s);
}

std::string to_string(ssize_t n)
{
	std::string s = "";

	if (n == 0)
		return ("0");
	else if (n == -9223372036854775807)
		return ("-9223372036854775807");
	else if (n < 0)
		n = -n;
	while (n > 0)
	{
		s += (n % 10) + '0';
		n /= 10;
	}
	std::reverse(s.begin() + (s[0] == '-'), s.end());
	return (s);
}