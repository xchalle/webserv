#ifndef DEF_REDIRECTRULES_HPP
# define DEF_REDIRECTRULES_HPP
# include <string>

struct redirectRule
{
	unsigned int code;
	std::string to;
	std::string text;
};

#endif