#include "../../include/location.hpp"
#include "../../include/utils.hpp"

Location::Location()
	: BaseServerConfig(), _upload()
{
}

Location::~Location()
{
}

Location &Location::operator=(const Location &other)
{
	if (this != &other)
	{
		BaseServerConfig::operator=(other);
		_path = other._path;
		_allowed_methods.assign(other._allowed_methods.begin(), other._allowed_methods.end());
		_root = other._root;
		_cgi_list.assign(other._cgi_list.begin(), other._cgi_list.end());
		_upload = other._upload;
	}
	return *this;
}

void Location::handle_child_value(Pair *current_pair)
{
	if (!current_pair)
		throw std::runtime_error("empty key");
	if (current_pair->key == "allow") {
		setAllowedMethods(current_pair);
	} else if (current_pair->key == "root") {
		setRoot(current_pair);
	} else if (current_pair->key == "index") {
		setIndex(current_pair);
	} else if (current_pair->key == "autoindex") {
		setAutoIndex(current_pair);
	} else if (current_pair->key == "cgi_pass") {
		setCGI(current_pair);
	} else if (current_pair->key == "upload_store") {
		setUploadStore(current_pair);
	} else if (current_pair->key == "upload") {
		setUploadEnable(current_pair);
	} else if (current_pair->key == "return") {
		setRedirectRules(current_pair);
	} else if (current_pair->key == "error_page") {
		setErrorsRedirect(current_pair);
	}  else {
		throw std::runtime_error("undefined key : `" + current_pair->key+"`");
	}
}

Location::Location(Pair *location_config, std::string location_path)
	: BaseServerConfig(), _upload()
{
	initUpload();
	if (location_config->key.compare("location") != 0)
		throw std::invalid_argument("Location config is invalid");
	setPath(location_path);
	for (std::vector<Pair*>::iterator it = location_config->child_value.begin(); it != location_config->child_value.end(); ++it)
	{
		Pair	*current_pair = *it;
		try {
			handle_child_value(current_pair);
		} catch (std::exception &e) {
			std::cerr << "Error: " << e.what() << std::endl;
			throw std::runtime_error("Invalid location "+location_path+" config");
		}
	}
}

void Location::initUpload()
{
	_upload.enabled = false;
	_upload.store_path = "";
}

void Location::setPath(std::string location_path)
{
	_path = location_path;
}

/**
 * Set the allowed methods for this location, allowed methods 
 * are `GET`, `POST`, `DELETE`
 * @example limit_except `GET`
*/
void Location::setAllowedMethods(Pair *location_child_value)
{
	std::vector<std::string> allowed_methods;

	allowed_methods.push_back("GET");
	allowed_methods.push_back("POST");
	allowed_methods.push_back("DELETE");
	if (location_child_value->key.compare("allow") != 0)
		throw std::invalid_argument("Invalid key for allow");
	if (location_child_value->values.size() == 0)
		throw std::invalid_argument("allow value is empty");
	if (std::find(allowed_methods.begin(), allowed_methods.end(), location_child_value->values[0]) == allowed_methods.end())
		throw std::invalid_argument("Invalid value for allow");
	_allowed_methods = location_child_value->values;
}



/**
 * Define if files in this location should be listed when a directory is requested
 * @example autoindex `on`
 * @example autoindex `off`
 */
void	BaseServerConfig::setAutoIndex(Pair *location_child_value)
{
	if (location_child_value->key.compare("autoindex") != 0)
		throw std::invalid_argument("Invalid key for autoindex");
	if (location_child_value->values.size() != 1)
		throw std::invalid_argument("Autoindex value must contain one arg (on/off)");
	if (location_child_value->values[0].compare("on") != 0 && location_child_value->values[0].compare("off") != 0)
		throw std::invalid_argument("Autoindex value must be on or off");
	_auto_index = location_child_value->values[0].compare("on") == 0;
}

void	Location::pushCGI(std::string extension, std::string path)
{
	CGI	new_cgi;

	new_cgi.extension = extension;
	new_cgi.path = path;
	_cgi_list.push_back(new_cgi);
}

/**
 * Define wich CGI to use for a specific file extension
 * @example `cgi .php /usr/bin/php-cgi`
 */
void Location::setCGI(Pair *location_child_value)
{
	if (location_child_value->key != "cgi_pass")
		throw std::invalid_argument("cgi_pass");
	if (location_child_value->values.size() != 2)
		throw std::invalid_argument("cgi_pass value must contain two args (extension, path)");
	if (location_child_value->values[0][0] != '.')
		throw std::invalid_argument("cgi_pass extension must start with a dot");
	if (this->getCGIPath(location_child_value->values[0]) != "")
		throw std::invalid_argument("cgi_pass already defined for "+location_child_value->values[0]);
	pushCGI(location_child_value->values[0], location_child_value->values[1]);
}

/**
 * Specifies a directory to which output files will be saved to.
 * @example upload_path "upload/directory/path"
 */
void Location::setUploadStore(Pair *location_child_value)
{
	if (location_child_value->key != "upload_store")
		throw std::invalid_argument("upload_store");
	if (location_child_value->values.size() != 1)
		throw std::invalid_argument("upload_store value must contain one arg (path)");
	_upload.store_path = location_child_value->values[0];
	if (_upload.store_path[_upload.store_path.size() - 1] != '/')
		_upload.store_path += '/';
}

/**
 ** @brief Enable or disable upload for this location
 ** @example upload on
 **/
void	Location::setUploadEnable(Pair *location_child_value)
{
	if (location_child_value->key != "upload")
		throw std::invalid_argument("upload");
	if (location_child_value->values.size() != 1)
		throw std::invalid_argument("upload value must contain one arg (on/off)");
	if (location_child_value->values[0].compare("on") != 0 && location_child_value->values[0].compare("off") != 0)
		throw std::invalid_argument("upload value must be on or off");
	_upload.enabled = location_child_value->values[0].compare("on") == 0;
}

std::string	Location::getPath() const
{
	return (_path);
}

std::vector<std::string>	Location::getAllowedMethods() const
{
	return (_allowed_methods);
}


bool BaseServerConfig::getAutoindex() const
{
	return (_auto_index);
}

std::vector<CGI> Location::getCGI() const
{
	return (_cgi_list);
}

/**
 * Return the CGI script path for a specific file extension.
 * @param extension The file extension to search for `.php` `php` `js`
 * @return The path to the CGI script or an empty string if no CGI is found
 */
std::string	Location::getCGIPath(std::string extension) const
{
	if (extension[0] != '.')
		extension = "." + extension;
	for (std::vector<CGI>::const_iterator it = _cgi_list.begin(); it != _cgi_list.end(); ++it)
	{
		if (it->extension == extension)
			return (it->path);
	}
	return ("");
}

bool Location::getUploadEnabled() const
{
	return (_upload.enabled);
}

std::string Location::getUploadStore() const
{
	return (_upload.store_path);
}