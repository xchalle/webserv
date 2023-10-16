#ifndef DEF_CGI_HPP
#define DEF_CGI_HPP
#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include "utils.hpp"

/**
 * Class that aim to use any CGI from a single Class.
 */
class CGIHandler
{
public:
	CGIHandler(std::string cgi_path, std::vector<std::string> avalaible_ext);
	CGIHandler(std::string cgi_path, std::string avalaible_ext);
	~CGIHandler();

	/** Setters **/
	void	setBody(std::string body);

	/**
	 * @brief Get the HTTP response from the CGI.
	 */
	std::string getResponse(void);

	void	pushEnv(std::string env_line);
	void	pushEnv(std::string key, std::string value);

	class CGIException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "CGI Exception";
		}
	};

private:
	std::string _cgi_path;
	std::vector<std::string> _compatible_extensions;
	std::vector<std::string> _env;
	std::string _body;
	std::string _lastResponse;
	long long int _response_length;

	char **_getEnv();

	void	_analyzeResponse();
	void	_handleChildRedirection(int *pipe_fds);
	std::string _handleParentRedirection(int *pipe_fds, int child_pid);

	void	_sendBody();

	void	_readCGIResponse(int *pipe_fds);
};

#endif