#ifndef DEF_PAIR_HPP
# define DEF_PAIR_HPP
# include <string>
# include <vector>

struct Pair {
	std::string 				key;
	std::vector<std::string>	values;
	std::vector<Pair*>			child_value;
};

#endif