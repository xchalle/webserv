#include "../../include/HttpResponse.hpp"
#include "../../include/httpRequest.hpp"
#include "../../include/utils.hpp"
#include "../../include/cgi.hpp"

/**
 * @brief Extract the headers from the given string
 * @param header_part The string containing the headers
 * of an HTTP request / response
 * @return A map containing the headers
*/
std::map<std::string, std::vector<std::string> >	extract_headers(std::string header_part)
{
	std::map<std::string, std::vector<std::string> > headers;
	std::vector<std::string> lines;
	std::vector<std::string> header;
	std::string key;
	std::vector<std::string> values;

	lines = split(header_part, "\r\n");
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++)
	{
		size_t pos = it->find(": ");
		if (pos == std::string::npos)
			continue;
		key = it->substr(0, pos);
		values = split(it->substr(pos + 2), ",");
		for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); it++)
			*it = trim(*it, " ");
		headers[key] = values;
	}
	return (headers);
}

/**
 * Verify if the request is valid, then
 * define the current location for the request.
*/
HttpResponse::HttpResponse(HttpRequest &request, Server &server)
	: _is_long_response(false), _request(request), _server(server), _current_location(NULL), _body_size(0),
	_status_code(200), _status_message("OK"), _response(),  _redirects(0), _file_read(false), _error_redirect_handled(false)
{
	(void)_redirects;
	validate_request();
	try {
		_current_location = _server.getLocation(this->_request.getCleanPath());
		debug("Current location block: `" + _current_location->getPath() + "`", INFO);
		validate_method();
	} catch (std::invalid_argument &e) {
		if (_status_code == 200){
			debug("No location block found for path `" + _request.getDecodedPath() + "`", INFO);
			_status_code = 404;
			_status_message = "Not Found";
		}
	} catch (...) {
		if (_status_code == 200) {
			_status_code = 500;
			_status_message = "Internal Server Error";
		}
	}
	
}
HttpResponse::~HttpResponse()
{
	if (_is_long_response && _file_stream.is_open())
		_file_stream.close();
}

void	HttpResponse::setStatus(unsigned int status_code, std::string status_message)
{
	_status_code = status_code;
	_status_message = status_message;
}

void	HttpResponse::setStatus(int status_code, std::string status_message)
{
	if (status_code < 0)
	{
		_status_code = 500;
		_status_message = "Internal Server Error";
		return ;
	}
	_status_code = (unsigned int)status_code;
	_status_message = status_message;
}

/**
 * Push Content-Length header if not present
*/
void	HttpResponse::handle_content_length(void)
{
	if (_body_size == 0)
		return ;
	if (_headers.find("Content-Length") == _headers.end())
	{
		_headers["Content-Length"] = std::vector<std::string>();
		_headers["Content-Length"].push_back(to_string(_body_size));
	}
}

std::vector<std::string> HttpResponse::getHeader(std::string key) const
{
	std::map<std::string, std::vector<std::string> >::const_iterator it = _headers.find(key);
	if (it == _headers.end())
		return std::vector<std::string>();
	return it->second;
}

/**
 * Validate the method of the request, if they are handled by the
 * web server.
*/
bool	HttpResponse::validate_handled_method(void)
{
	std::string requestMethod = _request.getMethod();

	if (requestMethod != "GET" && requestMethod != "POST" && requestMethod != "DELETE")
	{
		_status_code = 400;
		_status_message = "Bad Request";
		return (false);
	}
	return (true);
}

/**
 * Prevent directory traversal attacks
*/
bool	HttpResponse::validate_path(void)
{
	if (_request.getCleanPath().find("..") != std::string::npos)
	{
		_status_code = 403;
		_status_message = "Forbidden";
		return (false);
	} else if (_request.getPath().size() > DEFAULT_MAX_URI_SIZE)
	{
		_status_code = 414;
		_status_message = "URI Too Long";
		return (false);
	}
	return (true);
}

bool	HttpResponse::validate_body_size(void)
{
	if (_request.getClength() > _server.getMaxBodySize())
	{
		_status_code = 413;
		_status_message = "Request Entity Too Large";
		return (false);
	}
	return (true);
}

void HttpResponse::validate_request(void)
{
	if (_request.getPath() == "")
	{
		_status_code = 400;
		_status_message = "Bad Request";
		return ;
	}
	if (_request.getClength() == -1 && true == false)
	{
		_status_code = 411;
		_status_message = "Length Required";
		return ;
	}
	if (!this->validate_handled_method())
		return ;
	if (!this->validate_path())
		return ;
	if (!this->validate_body_size())
		return ;
}

/**
 * Validate the method of the request for current location. If
 * the methods aren't defined, they are allowed by default.
*/
bool HttpResponse::validate_method(void)
{
	if (_current_location == NULL)
		return (true);
	std::vector<std::string> methods = _current_location->getAllowedMethods();
	if (methods.size() == 0)
		return (true);
	if (std::find(methods.begin(), methods.end(), _request.getMethod()) == methods.end())
	{
		_status_code = 405;
		_status_message = "Method Not Allowed";
		return (false);
	}
	return (true);
}

/**
 * @brief Handle max body size for defining the response,
 * If hit send error 500.
 * 
 * @param new_size 
 */
void	HttpResponse::handleMaxBodySize(long new_size)
{
	if (new_size > _server.getMaxBodySize())
	{
		debug("Max body size exceeded", INFO);
		_status_code = 500;
		_status_message = "Internal Server Error";
		_is_long_response = false;
		_response.clear();
		_body_size = 0;
		throw HttpResponse::MaxBodySizeExceeded();
	}
}

/**
 * Erase current response and set the new body inside the response.
*/
void	HttpResponse::setBody(std::string new_body)
{
	_response.clear();
	this->handleMaxBodySize(new_body.size());
	if (new_body.size() > 0)
	{
		_response.insert(_response.end(), '\r');
		_response.insert(_response.end(), '\n');
		_response.insert(_response.end(), new_body.begin(), new_body.end());
	}
	_body_size = new_body.size();
}

/**
 * Insert given string at the end of the response, update the body size.
*/
void	HttpResponse::pushBody(std::vector<char> new_str, size_t size)
{
	this->handleMaxBodySize(_body_size + size);
	_response.insert(_response.end(), new_str.begin(), new_str.begin() + size);
	_body_size += size;
}

/**
 * Stop the execution of the request and return the response
*/
const char *HttpResponse::FileUploadFailed::what() const throw()
{
	return ("File upload failed");
}


const char *HttpResponse::MaxBodySizeExceeded::what() const throw()
{
	return ("Max body size exceeded");
}

/**
 * Get the status line of the HTTP response
 * @return <HTTP_version> <status_code> <status_message>
*/
std::string HttpResponse::getStatusLine(void) const
{
	std::string status_code = to_string(_status_code);
	return ("HTTP/1.1 " + status_code + " " + _status_message + "\r\n");
}

/**
 * Get the headers of the HTTP response
 * @return <key>: <value>\r\n
*/
std::string HttpResponse::getHeaders(void) const
{
	std::string headers_http_response = "";

	for (std::map<std::string, std::vector<std::string> >::const_iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		headers_http_response += it->first + ": ";
		for (std::vector<std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			headers_http_response += *it2;
			if (it2 + 1 != it->second.end())
				headers_http_response += ", ";
		}
		headers_http_response += "\r\n";
	}
	return (headers_http_response);
}

/**
 * Return the root to use if it is defined in the location
 * else return the root of the server.
 * Must be run after the location is found
*/
std::string HttpResponse::getCurrentRoot(void) const
{
	if (this->_current_location == NULL || this->_current_location->getRoot().empty())
		return (_server.getRoot());
	return (this->_current_location->getRoot());
}

std::string HttpResponse::getCurrentServerName(void) const
{
	std::vector<std::string> server_names = _server.getServerName();
	if (server_names.size() == 0)
		return ("");
	return (server_names[0]);
}

std::string HttpResponse::testIndex(std::vector<std::string> indexes, std::string path, std::string root) const
{
	for (std::vector<std::string>::iterator it = indexes.begin(); it != indexes.end(); it++)
	{
		debug("Testing index: " + root + path + *it, DEBUG);
		std::string index_path = root + path + *it;
		if (file_is_readable(index_path))
		{
			debug("Found file: " + index_path, DEBUG);
			return (index_path);
		}
	}
	return ("");
}

/**
 * Determine if a http redirection is asked for the given path, with Location config
 * and Server config.
 * @attention the caller must delete the returned pointer
 * @return a pointer to the redirect rule for the given path, or NULL if none
*/
redirectRule	*HttpResponse::getRedirectRuleForPath(void) const
{
	if (this->_current_location) {
		std::vector<redirectRule> location_rules = this->_current_location->getRedirectRules();

		if (!location_rules.empty())
		return (new redirectRule(location_rules[0]));
	}

	std::vector<redirectRule> server_rules = this->_server.getRedirectRules();

	if (!server_rules.empty())
	   return (new redirectRule(server_rules[0]));
	return (NULL);
}

/**
 * Determine if a http redirection is asked for the given error code, with Location config
 * and Server config.
 * @return a path to the redirect rule for the given error code, or NULL if none
*/
redirectRule	*HttpResponse::getRedirectRuleForCode(unsigned int code) const
{
	if (_current_location) {
		std::vector<redirectRule> *location_rules = this->_current_location->getErrorsRedirect();
		
		if (_current_location->_errors_redirect_size > 0 && location_rules){
			for (std::vector<redirectRule>::iterator it = location_rules->begin(); it != location_rules->end(); it++)
			{
				if ((unsigned int)it->code == code)
					return (&(*it));
			}
		}
	}
	std::vector<redirectRule> *server_rules = this->_server.getErrorsRedirect();
	if (_server._errors_redirect_size > 0 && server_rules){
		for (std::vector<redirectRule>::iterator it = server_rules->begin(); it != server_rules->end(); it++)
		{
			if ((unsigned int)it->code == code)
				return (&(*it));
		};
	}
	return (NULL);
}

std::string HttpResponse::getStatusMessage(unsigned int code) const
{
	if (code == 301)
		return ("Moved Permanently");
	if (code == 302)
		return ("Moved Temporarily");
	if (code == 303)
		return ("See Other");
	if (code == 307)
		return ("Temporary Redirect");
	if (code == 308)
		return ("Permanent Redirect");
	return ("");
}

/**
 * Determine if current Location config or Server config has a redirection rule.
 * If the redirection is set to an url and `->to` is not starting with a slash, it is
 * considered as a relative path and is appended to the current path.
 * @return True if a redirection rule is found, false otherwise
*/
bool	HttpResponse::handleRedirection(std::string current_path)
{
	(void) current_path;
	redirectRule *redirect = getRedirectRuleForPath();
	std::string	redirection_str = "";

	if (redirect != NULL)
	{
		if (redirect->to.empty()) {
			debug("Body is defined from directive return [text]", DEBUG);
			setBody(redirect->text);
		} else {
			if (redirect->to[0] != '/' && redirect->to.find("http://") == std::string::npos && redirect->to.find("https://") == std::string::npos)
				redirection_str = _request.getPath() + "/" + redirect->to;
			else
				redirection_str = redirect->to;
			debug("Visibile redirection to: " + redirection_str, DEBUG);
			setBody("<html><head><title>Redirecting...</title></head><body><h1>Redirecting...</h1><p>Redirecting to <a href=\"" + redirection_str + "\">here</a></p></body></html>");
			pushHeader("Location", redirection_str);
		}
		_status_code = redirect->code;
		_status_message = getStatusMessage(redirect->code);
		delete redirect;
		return (true);
	}
	delete redirect;
	return (false);
}

bool	HttpResponse::handleAutoindex(std::string current_path)
{
	std::string root = this->getCurrentRoot();
	if (this->_current_location->getAutoindex() == false
		&& this->_server.getAutoindex() == false)
		return (false);
	debug("Autoindex is enabled", INFO);
	std::vector<file> files = get_files_in_directory(root + current_path);
	std::string fileHTML = "";
	for (std::vector<file>::iterator it = files.begin(); it != files.end(); it++)
	{
		std::string icon = (it->is_dir) ? get_file_content(FOLDER_SVG_PATH) : get_file_content(FILE_SVG_PATH);
		std::string	decodedPath = this->_request.getDecodedPath();
		if (decodedPath[decodedPath.size() - 1] != '/')
			decodedPath += "/";
		std::string file_path = decodedPath + it->name;//request path not working
		fileHTML += "<div class='row'><div class='icon'>" + icon + "</div><div class='title'><a href='" + file_path + "'>" + it->name + "</a></div></div>";
	}
	setBody("<html><head><title>Index of " + current_path + "</title></head><body><h1>Index of " + current_path + "</h1><div class='fileList'>"+fileHTML+"</div><footer><style>.row a,body{color:#adbac7}.row{display:flex;column-gap:16px;padding:8px 16px}.row a{text-decoration:none;font-size: 18px;}.row svg{fill:#768390;height:20px;width:20px;}body{background-color:#22272e}.fileList{border-radius:5px}.row:not(:last-child){border-bottom:1px solid #444c56}</style></footer></body></html>");
	return (true);
}

/**
 * Return the path of the file to send to the client,
 * file path is relative to the root of the server and
 * the location where the request was made
 * @return The path of the file asked by the Request. Empty string
 * if the file is not found
*/
std::string HttpResponse::getFilePath(void)
{
	std::string path = this->_request.getCleanPath();
	std::string root = this->getCurrentRoot();
	std::string possible_path;
	
	debug("Request path: `" + path + "`", ERROR);
	if (handleRedirection(path))
		return ("");
	try {
		if (file_exists(root + path)) {
			if (is_directory(root + path)) {
				if (path[path.size() - 1] != '/')
					path += "/";
				debug("Path is a directory", DEBUG);
				possible_path = this->testIndex(_current_location->getIndex(), path, root);
				if (!possible_path.empty())
					return (possible_path);
				possible_path = this->testIndex(_server.getIndex(), path, root);
				if (!possible_path.empty())
					return (possible_path);
				if (this->handleAutoindex(path))
					return ("");
			} else {
				debug("Path is a file", DEBUG);
				if (file_is_readable(root + path))
				{
					debug("Found file with root + path", ERROR);
					return (root + path);
				} else {
					debug("File not found with root + path at: `" + root + path + "`", ERROR);
				}
			}
		}
	} catch (...) {
		if (_status_code != 200)
			return ("");
	}
	debug("Error: File not found at path: `" + path + "`", ERROR);
	_status_code = 404;
	_status_message = "Not Found";
	return ("");
}

void HttpResponse::handle_cgi_delete(void)
{
	std::string cgi_headers_end = "\r\n\r\n";
	std::vector<char>::iterator headers_end = std::search(this->_response.begin(), this->_response.end(), cgi_headers_end.begin(), cgi_headers_end.end());
	
	if (headers_end == this->_response.end())
		return ;
	std::string header_part(this->_response.begin(), headers_end);
	std::map<std::string, std::vector<std::string> > headers = extract_headers(header_part);
	if (headers["Status"].empty())
		return ;
	_status_code = atoi(headers["Status"][0].substr(0, 3).c_str());
	_status_message = headers["Status"][0].substr(4);
}

std::vector<std::string>	HttpResponse::get_cgi_handle_headers(void) const
{
	std::vector<std::string> handled_headers;
	handled_headers.push_back("Content-Type");
	handled_headers.push_back("Content-Length");
	return (handled_headers);
}

void	HttpResponse::build_cgi_other_headers(CGIHandler & cgi) const
{
	std::map<std::string, std::vector<std::string> > headers = this->_request.getHeaders();
	std::vector<std::string> handled_headers = get_cgi_handle_headers();

	for (std::map<std::string, std::vector<std::string> >::iterator it = headers.begin(); it != headers.end(); it++)
	{
		if (std::find(handled_headers.begin(), handled_headers.end(), it->first) != handled_headers.end())
			continue;
		std::string header_value = join(it->second, ", ");
		cgi.pushEnv("HTTP_" + it->first, header_value);
	}
}

CGIHandler	HttpResponse::init_cgi(std::string cgi_path, std::string file_ext, std::string file_path)
{
	CGIHandler	cgi(cgi_path, file_ext);
	long long	content_length = _request.getClength();

	content_length = (content_length == -1) ? 0 : content_length;
	std::map<std::string, std::vector<std::string> > request_headers = _request.getHeaders();
	std::vector<std::string> content_type_values = request_headers["Content-Type"];
	std::string content_type = (content_type_values.size() > 0) ? content_type_values[0] : "";
	cgi.pushEnv("SERVER_SOFTWARE", "Webserv/1.0");
	cgi.pushEnv("SERVER_NAME", this->getCurrentServerName());
	cgi.pushEnv("GATEWAY_INTERFACE", "CGI/1.1");
	cgi.pushEnv("SERVER_PROTOCOL", "HTTP/1.1");
	cgi.pushEnv("SERVER_PORT", to_string(_server.getPort()));
	cgi.pushEnv("REQUEST_METHOD", _request.getMethod());
	cgi.pushEnv("REQUEST_URI", _request.getPath());
	cgi.pushEnv("PATH_INFO", _request.getCleanPath());
	cgi.pushEnv("PATH_TRANSLATED", file_path);
	cgi.pushEnv("CONTENT_TYPE", content_type);
	cgi.pushEnv("CONTENT_LENGTH", to_string(content_length));
	cgi.pushEnv("REDIRECT_STATUS", to_string(_status_code));
	cgi.pushEnv("SCRIPT_NAME", cgi_path);
	cgi.pushEnv("QUERY_STRING", _request.getQuery());
	cgi.setBody(_request.getBody());
	this->build_cgi_other_headers(cgi);
	return (cgi);
}

/**
 * Handle the request if the file is a CGI script.
 * send the response of the CGI script to the client
 * @return True if the file is a CGI script, false otherwise
*/
bool	HttpResponse::handle_cgi(std::string file_path)
{
	std::string file_ext = file_path.substr(file_path.find_last_of(".") + 1);
	if (file_ext.empty())
		return (false);
	debug("Seeking cgi for extension: " + file_ext, DEBUG);
	std::string cgi_path = _current_location->getCGIPath(file_ext);
	if (cgi_path.empty())
		return (false);
	debug("Found cgi for extension: `" + file_ext + "` at: `" + cgi_path + "`", DEBUG);
	CGIHandler	cgi = init_cgi(cgi_path, file_ext, file_path);
	
	try {
		std::string cgi_response = cgi.getResponse();
		this->_response = std::vector<char>(cgi_response.begin(), cgi_response.end());
		if (_request.getMethod() == "DELETE")
			handle_cgi_delete();
		if (this->_response.empty())
			return (false);
		return (true);
	} catch (CGIHandler::CGIException & e) {
		_status_code = 502;
		_status_message = "Bad Gateway";
	} catch (...) {
		_status_code = 500;
		_status_message = "Internal Server Error";
	}
	debug("CGI failed, reset of _response", DEBUG);
	_response = std::vector<char>();
	_body_size = 0;
	return (true);
}

void	HttpResponse::setFailedLongResponse(void)
{
	_status_code = 500;
	_status_message = "Internal Server Error";
	_response = std::vector<char>();
	_is_long_response = false;
}	

/**
 * @brief Build the body of the response from a file
 * 
 */
void	HttpResponse::build_classic_body(void)
{
	try {
		this->get_long_file_content();
	} catch (HttpResponse::MaxBodySizeExceeded & e) {
		_is_long_response = false;
		return ;
	} catch (...) {
		this->setFailedLongResponse();
		return ;
	}
	if (_is_long_response == true)
		return ;
	debug("Body has been defined from file: " + _file_path, DEBUG);
}

void	HttpResponse::build_body(void)
{
	_file_path = this->getFilePath();

	if (_file_path.empty())
		return ;
	if (this->handle_cgi(_file_path) == true)
		return ;
	this->build_classic_body();
}

/**
 * Define an HTTP response for the given request
 * @return string containing the HTTP response
 * <HTTP version> <status code> <status message>\r\n
 * <headers>\r\n
 * <body>
*/
void	HttpResponse::buildResponse(void)
{
	this->handle_content_length();
	std::string	statusLine = this->getStatusLine();
	std::string	headers = this->getHeaders();
	std::string	lineBreak = "\r\n";

	debug("Defining response head", DEBUG);
	_response.insert(_response.begin(), headers.begin(), headers.end());
	_response.insert(_response.begin(), statusLine.begin(), statusLine.end());
	if (_body_size == 0)
		_response.insert(_response.end(), lineBreak.begin(), lineBreak.end());
}

bool	HttpResponse::isErrorCode(unsigned int code) const
{
	if (code >= 400 && code < 600)
		return (true);
	return (false);
}

/**
 * Handle the redirections for the error pages
 * if ErrorPage is defined in the server or location config
 * Only affect the body of the response
 * @throw std::runtime_error if _status_code is valid (200)(Only for dev purpose)
*/
void	HttpResponse::handleErrorRedirect(void)
{
	if (this->_status_code == 200)
		throw std::runtime_error("Error redirection must not be called with status code 200");
	redirectRule *errorRedirection = this->getRedirectRuleForCode(this->_status_code);
	try {
		if (errorRedirection != NULL)
		{
			std::string root = this->getCurrentRoot();

			if (root[root.size() - 1] != '/')
				root += '/';
			debug("Error redirection found for code: " + to_string(this->_status_code), DEBUG);
			std::string file_path = root + errorRedirection->to;
			if (file_is_readable(file_path))
			{
				_file_path = file_path;
				_error_redirect_handled = true;
				setBody("");
				this->get_long_file_content();
				return ;
			}
		}
		if (this->_response.empty() == false)
			return ;
	} catch (...) {
		if (_status_code == 200){ 
			_status_code = 500;
			_status_message = "Internal Server Error";
		}
	}
	debug("Default error page will be used for code: " + to_string(this->_status_code), DEBUG);
	try {
		setBody(getErrorPage(this->_status_code, this->_status_message));
	} catch (HttpResponse::MaxBodySizeExceeded & e) {
		debug("Error page is too long, 500 will be returned", DEBUG);
	}
	return ;
}

/**
 * Wrapper for the buildResponse method, used to
 * check final redirections.
 * If the response is already set, return it.
*/
std::vector<char>	HttpResponse::handleResponse(void)
{
	if ((this->isErrorCode(this->_status_code) || this->_status_code == 310)
		&& this->_error_redirect_handled == false)
	{
		this->handleErrorRedirect();
		setHeader("Content-Length", to_string(_body_size));
		if (this->_is_long_response == true)
			return (std::vector<char>());
	}
	this->buildResponse();
	return (this->_response);
}

std::string remove_boundary(std::string data, std::string boundary) {
	size_t pos;
	size_t end;
	size_t start;
	size_t i = 0;

	while ((pos = data.find(boundary)) != std::string::npos) {
		end = data.find("\r\n", pos);
		if (end == std::string::npos)
			break;
		start = data.rfind("\n", pos);
		if (start == std::string::npos)
		{
			if (i == 0)
				start = 0;
			else
				break;
		}
		debug("remove boundary from " + to_string(start) + " to " + to_string(end), DEBUG);
		data.erase(start, end - start + 2);
		pos = data.find(boundary);
		i++;
	}
	pos = data.find("\r\n\r\n");
	if (pos == std::string::npos) {
		return data;
	}
	pos += 4;
	data.erase(0, pos);
	return (data);
}

/**
 * Send the HTTP response to the client, if the Request
 * is valid.
*/
std::vector<char>	HttpResponse::getResponse(void)
{
	try {
		if (this->_status_code != 200)
			return (this->handleResponse());
		if (this->isValidUpload())
			return (this->handleResponse());
		if (this->_status_code != 200)
			return (this->handleResponse());
		if (this->_request.getMethod() == "GET"
			|| this->_request.getMethod() == "POST"
			|| this->_request.getMethod() == "DELETE")
			this->build_body();
		else
		{
			_status_code = 405;
			_status_message = "Method Not Allowed";
		}
		if (this->_is_long_response)
			return (std::vector<char>());
		return (this->handleResponse());
	} catch (HttpResponse::MaxBodySizeExceeded &e) {
		return (this->handleResponse());
	} catch (...) {
		_status_code = 500;
		_status_message = "Internal Server Error";
		return (this->handleResponse());
	}
}

/**
 * Set the body of the response from the defined `_file_path`
 * Only work if it's a long response
 * @return true if there is more data to read, false otherwise
*/
bool	HttpResponse::setLongResponse(void)
{
	if (_is_long_response == false)
		return (false);
	this->build_classic_body();
	if (_is_long_response == true)
		return (true);
	return (false);
}

/**
 * Must be used after `setLongResponse`
 * Create the final response from readed file
*/
std::vector<char>	HttpResponse::getLongResponse(void)
{
	return (this->handleResponse());
}

unsigned int HttpResponse::getStatusCode(void)
{
	return(this->_status_code);
}

void HttpResponse::pushHeader(std::string key, std::string value)
{
	if (this->_headers.find(key) == this->_headers.end())
		this->_headers[key] = std::vector<std::string>(1, value);
	else
		this->_headers[key].push_back(value);
}

void	HttpResponse::setHeader(std::string key, std::string value)
{
	this->_headers[key] = std::vector<std::string>(1, value);
}