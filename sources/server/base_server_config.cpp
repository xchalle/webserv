#include "../../include/location.hpp"
#include "../../include/utils.hpp"


BaseServerConfig::BaseServerConfig()
   :  _redirect_rules(), _errors_redirect(), _auto_index(false), _errors_redirect_size(0)
{
}

BaseServerConfig::~BaseServerConfig()
{
}

BaseServerConfig &BaseServerConfig::operator=(const BaseServerConfig &other)
{
	if (this != &other)
	{
		_redirect_rules.assign(other._redirect_rules.begin(), other._redirect_rules.end());
		_errors_redirect.assign(other._errors_redirect.begin(), other._errors_redirect.end());
		_root = other._root;
		_index.assign(other._index.begin(), other._index.end());
		_auto_index = other._auto_index;
		_errors_redirect_size = other._errors_redirect_size;
	}
	return (*this);
}

/**
 * Set the redirect rules for this location. Must contain a status code
 * followed by a URL
 * On Nginx url for error_code 301, 302, 303, 307, otherwise url is considered as text
 * @see https://serverfault.com/questions/782724/custom-status-code-for-nginx-default-page
 * @example return `301 /new_url`
 * @example return `302 /new_url`
 */
void	BaseServerConfig::setRedirectRules(Pair *location_child_value)
{
	if (location_child_value->key.compare("return") != 0)
		throw std::invalid_argument("Invalid key for redirect");
	if (location_child_value->values.size() != 2)
		throw std::invalid_argument("Redirect value must contain two args (status_code, url)");
	unsigned int status_code = str_to_int(location_child_value->values[0]);
	is_http_code_valid(status_code);
	push_redirect_rule(status_code, location_child_value->values[1]);
}

/**
 * Set the base root directory for this location
 * @example `root /var/www/html`
 */
void	BaseServerConfig::setRoot(Pair *location_child_value)
{
	if (location_child_value->key.compare("root") != 0)
		throw std::invalid_argument("Invalid key for root");
	if (location_child_value->values.size() != 1)
		throw std::invalid_argument("Root value must contain one arg (path)");
	_root = location_child_value->values[0];
}

/**
 * Define the file to be used as index when a directory is requested.
 * They are used in the order they are defined in the config file
 * @example index `index.html` `index.php`
*/
void	BaseServerConfig::setIndex(Pair *location_child_value)
{
	if (location_child_value->key.compare("index") != 0)
		throw std::invalid_argument("Invalid key for index");
	if (location_child_value->values.size() == 0)
		throw std::invalid_argument("Index value is empty");
	_index = location_child_value->values;
}

/**
 * Push a new error code and its redirection path
 */
void	BaseServerConfig::push_redirect_error(unsigned int code, std::string path)
{
	redirectRule redirect_error;

	redirect_error.code = code;
	redirect_error.to = path;
	_errors_redirect.push_back(redirect_error);
	_errors_redirect_size++;
}

void	BaseServerConfig::push_redirect_rule(unsigned int code, std::string value)
{
	redirectRule redirect_rule;

	redirect_rule.code = code;
	if (code == 301 || code == 302 || code == 303 || code == 307 || code == 308)
	{
		redirect_rule.to = value;
		redirect_rule.text = "";
	}
	else
	{
		redirect_rule.text = value;
		redirect_rule.to = "";
	}
	_redirect_rules.push_back(redirect_rule);
}

/**
 * Define the error pages to be displayed when an error 
 * occurs on the server.
 * Only update body of the response.
 * @example `error_page 404 /404.html`;
 */
void BaseServerConfig::setErrorsRedirect(Pair *pair)
{
	if (pair->key != "error_page")
		throw std::invalid_argument("error_page");
	is_error_page_valid(pair);
	size_t	valuesSize = pair->values.size();
	std::string error_path = pair->values[valuesSize - 1];

	for (str_it it = pair->values.begin(); *it != pair->values[valuesSize - 1]; ++it)
		push_redirect_error(str_to_int(*it), error_path);
}

std::vector<redirectRule>	BaseServerConfig::getRedirectRules() const
{
	return (_redirect_rules);
}

std::string	BaseServerConfig::getRoot() const
{
	return (_root);
}

std::vector<std::string> BaseServerConfig::getIndex() const
{
	return (_index);
}

std::vector<redirectRule> *BaseServerConfig::getErrorsRedirect()
{
	return (&_errors_redirect);
}