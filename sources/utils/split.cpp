#include "../../include/utils.hpp"
#include <vector>
#include <string>

/**
 * @brief Split a string into a vector of strings, using each 
 * character of to_replace as a separator
*/
std::vector<std::string> split(std::string str, std::string to_replace)
{
	std::vector<std::string> result;
	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while ((pos = str.find_first_of(to_replace, pos)) != std::string::npos)
	{
		std::string substring(str.substr(prev, pos - prev));
		if (substring.length() > 0)
			result.push_back(substring);
		prev = ++pos;
	}
	result.push_back(str.substr(prev, pos - prev));
	return (result);
}

void skip_quotes(std::string &str, size_t &i, char seek_quote)
{
	while (str[i] != seek_quote && i < str.length())
		i++;
	i++;
}

std::vector<std::string> split_with_quotes(std::string str)
{
	std::vector<std::string> result;
	size_t i = 0;
	size_t prev = 0;
    int    in_quote = 0;

	while (str[i])
	{
		if (str[i] == '"' || str[i] == '\'')
		{
            in_quote = 1;
			std::string substring(str.substr(prev, i - prev));
			if (substring.length() > 0)
				result.push_back(substring);
			prev = ++i;
			skip_quotes(str, i, str[i - 1]);
		}
		else if (str[i] == ' ' || str[i] == '\t')
		{
			std::string substring(str.substr(prev, i - prev - in_quote));
            in_quote = 0;
			if (substring.length() > 0)
				result.push_back(substring);
			prev = ++i;
		}
		else
			i++;
	}
	result.push_back(str.substr(prev, i - prev - (2 * in_quote)));
	return (result);
}

std::vector<std::string> split_quotes_tab(std::string str)
{
	std::vector<std::string> result;
	size_t i = 0;
	size_t prev = 0;
    int    in_quote = 0;

	while (str[i])
	{
		if (str[i] == '"' || str[i] == '\'')
		{
            in_quote = 1;
			std::string substring(str.substr(prev, i - prev));
			if (substring.length() > 0)
				result.push_back(substring);
			prev = ++i;
			skip_quotes(str, i, str[i - 1]);
		}
		else if (str[i] == ' ' || str[i] == '\t')
		{
			std::string substring(str.substr(prev, i - prev - in_quote));
            in_quote = 0;
			if (substring.length() > 0)
				result.push_back(substring);
			prev = ++i;
		}
		else
			i++;
	}
	result.push_back(str.substr(prev, i - prev));
	return (result);
}

void					split_headers_value(std::vector<std::string> *result, std::string str)
{
	std::vector<std::string>	tmp;
	tmp = split(str, ";");
	for (std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); it++)
		result->push_back(*it);
}

std::vector<std::string> split_first_of(std::string str, std::string to_replace)
{
	std::vector<std::string> result;
	std::string::size_type pos = 0;
	if (str.empty() || (pos = str.find_first_of(to_replace, pos)) == std::string::npos)
		return (result);
	else
	{
		std::string substring(str.substr(0, pos));
		if (substring.length() > 0)
			result.push_back(substring);
		pos++;
		result.push_back(normstring(str.substr(pos)));
		return (result);
	}
}

std::vector<std::string> split_string(std::string str, std::string to_replace)
{
	std::vector<std::string> result;
	std::string::size_type pos = 0;
	std::string::size_type prev = 0;
	while (((pos = str.find_first_of(to_replace, pos)) != std::string::npos) && (str.compare(pos, to_replace.size(), to_replace) == 0))
	{
		std::string substring(str.substr(prev, pos - prev));
		if (substring.length() > 0)
			result.push_back(substring);
		pos += to_replace.size();
		prev = pos;
	}
	result.push_back(str.substr(prev, pos - prev));
	return (result);
}

std::vector<std::string> split_first_occurence(std::string s, std::string delimiter) 
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    if ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }
    res.push_back (s.substr (pos_start));
    return res;
}