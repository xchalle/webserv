# include "../../include/utils.hpp"

void    debug(std::string message, debug_level level = DEBUG)
{
	if (level >= LEVEL) {
	   std::cout << message << std::endl;
	}
}