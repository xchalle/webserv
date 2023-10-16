#include "../../include/utils.hpp"
#define BASE "0123456789abcdef"

int		atoi_hex(std::string &str)
{
    size_t i = 0;
	size_t j = 0;
	size_t base_size = 16;
    int result = 0;
    std::string base = BASE;

    if(str.size() > 4)
        return (-1);
    if(str.find_first_not_of(BASE)!= std::string::npos)
        return (-1);

    while (str[i])
	{
		j = 0;
		while (base[j])
		{
			if (str[i] == base[j])
				result = result * base_size + j;
			j++;
		}
		i++;
	}
	return (result);
}