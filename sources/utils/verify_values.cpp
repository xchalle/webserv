#include <string>
#include "../../include/server.hpp"
#include "../../include/utils.hpp"

/**
 * Verify if the given string is a valid server_name
 * - Can contain a wildcard at start before the first dot
 * - Can contain a wildcard at end after the last dot
 * - must not contin a wildcard in the middle of the string
 * @see http://nginx.org/en/docs/http/server_names.html
 */
bool	is_servername_valid(std::string server_name)
{
	if (server_name.size() == 0)
		throw std::invalid_argument("Server name is empty");
	if (server_name.size() > 64)
		throw std::invalid_argument("Server name is too long. Max length is 64");
	if (server_name == "*")
		throw std::invalid_argument("Server name is a wildcard");
	if (server_name[0] == '*' && server_name[1] != '.')
		throw std::invalid_argument("Invalid server_name `" + server_name + "` : wildcard must be followed by a dot");
	if (server_name[server_name.size() - 1] == '*' && server_name[server_name.size() - 2] != '.')
		throw std::invalid_argument("Invalid server_name `" + server_name + "` : wildcard must be preceded by a dot");
	if (server_name.find("*") != std::string::npos && server_name.find("*") != 0 && server_name.find("*") != server_name.size() - 1)
		throw std::invalid_argument("Invalid server_name `" + server_name + "` : wildcard must be at start or end of the string");
	return (true);
}
/**
 * Determine if given string is a valid HTTP Code.
 * Only check range.
 * Not checking each code validity.
*/
bool	is_http_code_valid(unsigned int code)
{
	if (code < 100 || code > 599)
		throw std::invalid_argument("Error code must be between 100 and 599");
	return (true);
}

/**
 * - Key Pair must be `error_page`
 * - Values must include one or multiple code and a path
 */
bool	is_error_page_valid(Pair *error_page)
{
	if (error_page->values.size() < 2)
		throw std::invalid_argument("Error page must include at least one code and a path");
	for (str_it it = error_page->values.begin(); *it != error_page->values[error_page->values.size() - 1]; ++it)
	{
		unsigned int code = str_to_int(*it);
		if (!is_http_code_valid(code))
			throw std::invalid_argument("Invalid error code");
	}
	if (error_page->values[ error_page->values.size() - 1 ].size() == 0)
		throw std::invalid_argument("Invalid path");
	return (true);
}