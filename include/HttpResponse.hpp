#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP
# define DEFAULT_FILE_NAME "file"
# define DEFAULT_MAX_URI_SIZE 2048
# define DEFAULT_MAX_REDIRECTS 10
# include <map>
# include <vector>
# include <iostream>
# include <fstream>
# include "./redirectRules.hpp"
# include "./utils.hpp"

class HttpRequest;
class Server;
class Location;
class CGIHandler;

struct HTTPFileInfo
{
	std::string filename;
	std::string name;
	std::string extension;
	std::string content_type;
};

HTTPFileInfo getFileInfo(HttpRequest &request);
std::string getErrorPage(unsigned int code, const std::string& message);
std::map<std::string, std::vector<std::string> >	extract_headers(std::string header_part);

class HttpResponse
{
public:

	bool	_is_long_response;

	HttpResponse(HttpRequest  &request, Server &server);
	~HttpResponse();

	std::vector<char>	getResponse(void);
	bool				setLongResponse(void);
	std::vector<char>	getLongResponse(void);

	unsigned int	getStatusCode(void);

	/** SETTERS */
	void	setStatus(unsigned int status_code, std::string status_message);
	void	setStatus(int status_code, std::string status_message);

private:
	HttpRequest &_request;
	Server 		&_server;
	Location	*_current_location;
	
	size_t		_body_size;
	std::map<std::string, std::vector<std::string> > _headers;

	unsigned int	_status_code;
	std::string		_status_message;
	/**
	 * @brief Response to be sent to the client,
	 * if it is empty, the response will be built
	*/
	std::vector<char>	_response;

	unsigned int	_redirects;

	std::string		_file_path;
	std::ifstream	_file_stream;
	bool			_file_read;
	bool			_error_redirect_handled;

	/** SETTERS */

	void 		setBody(std::string new_body);
	void		pushBody(std::vector<char> new_str, size_t size);
	void		handleMaxBodySize(long new_size);

	/** GETTERS */
	std::vector<std::string> getHeader(std::string key) const;


	/** Validation of request */

	void		validate_request(void);
	bool		validate_handled_method(void);
	bool		validate_method(void);
	bool		validate_path(void);
	bool		validate_body_size(void);

	/** Functionnal during response **/
	bool		isValidUpload(void);
	void		handleUploads(size_t pos);
	std::string get_file_name(HTTPFileInfo info, std::string insertion_folder);
	void		saveFile(std::map<std::string, std::vector<std::string> > headers, size_t sof, size_t eof);

	/** Creation of response */

	std::string getStatusLine(void) const;
	std::string getHeaders(void) const;
	void		buildResponse(void);
	std::vector<char>	handleResponse(void);
	void		build_body(void);
	void		build_classic_body(void);
	void		setFailedLongResponse(void);

	void		handle_content_length(void);

	std::vector<std::string>	get_cgi_handle_headers(void) const;
	void		build_cgi_other_headers(CGIHandler & cgi) const;
	bool	    handle_cgi(std::string file_path);
	CGIHandler	init_cgi(std::string cgi_path, std::string file_ext, std::string file_path);
	void 		handle_cgi_delete(void);

	void		get_long_file_content(void);

	std::string getFilePath(void);
	std::string getCurrentRoot(void) const;
	std::string getCurrentServerName(void) const;

	bool	handleRedirection(std::string current_path);
	bool	handleAutoindex(std::string current_path);
	void	handleErrorRedirect(void);

	std::string	testIndex(std::vector<std::string> index, std::string path, std::string root) const;
	redirectRule *getRedirectRuleForCode(unsigned int code) const;
	redirectRule *getRedirectRuleForPath(void) const;

	void	pushHeader(std::string key, std::string value);
	void	setHeader(std::string key, std::string value);

	bool	isErrorCode(unsigned int code) const;

	std::string getStatusMessage(unsigned int code) const;

	/** Exceptions **/

	class FileUploadFailed : public std::exception
	{
		virtual const char *what() const throw();
	};

	class MaxBodySizeExceeded : public std::exception
	{
		virtual const char *what() const throw();
	};
};

#endif