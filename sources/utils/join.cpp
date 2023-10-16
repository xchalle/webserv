# include "../../include/utils.hpp"

std::string join(std::vector<std::string> vec, std::string separator)
{
	std::string result = "";
	
	for (size_t i = 0; i < vec.size(); i++)
	{
		result += vec[i];
		if (i < vec.size() - 1)
			result += separator;
	}
	return (result);
}