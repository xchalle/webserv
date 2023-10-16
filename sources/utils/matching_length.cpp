#include "../../include/utils.hpp"

size_t get_matching_length(const std::string &str1, const std::string &str2)
{
	size_t i(0);
	while (i < str1.size() && i < str2.size() && str1[i] == str2[i])
		i++;
	return i;
}

/**
 * @brief Get the prefix matching length between two strings
 * The location path must be at the beginning of the target request and
 * there should be a '/' after the location path in order for it to
 * be a prefix match. 
*/
size_t get_prefix_matching_length(const std::string &location, const std::string &target)
{
	if (target.find(location) == 0 && target.size() > location.size()
		&& target[location.size()] == '/') {
		return (location.size());
	}
	return (0);
}