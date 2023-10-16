#include "../../include/cgi.hpp"


CGIHandler::CGIHandler(std::string cgi_path, std::vector<std::string> avalaible_ext)
    : _cgi_path(cgi_path), _compatible_extensions(avalaible_ext)
{
}

CGIHandler::CGIHandler(std::string cgi_path, std::string avalaible_ext)
	: _cgi_path(cgi_path)
{
	_compatible_extensions.push_back(avalaible_ext);
}

CGIHandler::~CGIHandler()
{
}

void	CGIHandler::setBody(std::string body)
{
	_body = body;
}

char **CGIHandler::_getEnv()
{
	char **env = new char *[_env.size() + 1];

	debug("CGI variables :", DEBUG);
	for (size_t i = 0; i < _env.size(); i++)
	{
		env[i] = new char[_env[i].size() + 1];
		for (size_t j = 0; j < _env[i].size(); j++)
			env[i][j] = _env[i][j];
		env[i][_env[i].size()] = '\0';
		debug("env[" + to_string(i) + "] = " + env[i], DEBUG);
	}
	debug("_____________________________", DEBUG);
	env[_env.size()] = NULL;
	return env;
}


void	CGIHandler::pushEnv(std::string env_line)
{
	_env.push_back(env_line);
}

void	CGIHandler::pushEnv(std::string key, std::string value)
{
	_env.push_back(to_uppercase(key) + "=" + value);
}

/**
 * @brief Check the HTTP response from the CGI, and 
 * insert the Content-Length header if needed.
 */
void	CGIHandler::_analyzeResponse()
{
	if (_lastResponse.length() == 0)
		return;
	std::size_t headers_end = _lastResponse.find("\r\n\r\n");
	if (headers_end == std::string::npos)
	{
		debug("Possible invalid HTTP response from CGI", ERROR);
		throw CGIHandler::CGIException();
	}
	headers_end += 2;
	std::string content = _lastResponse.substr(headers_end + 2);
	_response_length = content.length();
	_lastResponse.insert(headers_end, "Content-Length: " + to_string(_response_length) + "\r\n");
}

/**
 ** @brief Send the body to the CGI trough STDIN.
 **/
void	CGIHandler::_sendBody()
{
	if (_body.empty() == false)
	{
		int	fds[2];

		pipe(fds);
		if (dup2(fds[0], STDIN_FILENO) < 0)
			throw std::runtime_error("dup2 failed");
		write(fds[1], _body.c_str(), _body.length());
		close(fds[0]);
		close(fds[1]);
	}
}

void	CGIHandler::_handleChildRedirection(int *pipe_fds)
{
	char **envp = this->_getEnv();
	char *argv[] = {NULL};
	
	if (!_body.empty())
		debug("sending body `" + _body + "` to cgi", DEBUG);
	if (dup2(pipe_fds[1], STDOUT_FILENO) < 0)
		throw std::runtime_error("dup2 failed");
	close(pipe_fds[0]);
	close(pipe_fds[1]);
	this->_sendBody();
	execve(_cgi_path.c_str(), argv, envp);
	exit(-1);
}

/**
 * @brief Read the response of the CGI script from the pipe
 * @throws May throw std::runtime_error if read failed
 */
void	CGIHandler::_readCGIResponse(int *pipe_fds)
{
	char buffer[8192];

	_lastResponse = "";
	ssize_t	read_res = 1;
	while ((read_res = read(pipe_fds[0], buffer, 8192)) > 0)
	{
		if (read_res < 0)
			throw std::runtime_error("read failed");
		buffer[read_res] = '\0';
		_lastResponse.append(buffer, read_res);
	}
	close(pipe_fds[0]);
}

/**
 * @brief Read the response of the CGI script from the pipe, and 
 * wait for the child process to finish and complete CGI response.
 * 
 * @param pipe_fds Pipe file descriptors
 * @param child_pid Pid of the child process
 * @return The completed response of the CGI script
 */
std::string CGIHandler::_handleParentRedirection(int *pipe_fds, int child_pid)
{
	int status;

	close(pipe_fds[1]);
	this->_readCGIResponse(pipe_fds);
	waitpid(child_pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		throw CGIHandler::CGIException();
	this->_analyzeResponse();
	return (_lastResponse);
}


/**
 ** @brief Return the response of the CGI script
 ** 
 ** @throws May throw CGIHandler::CGIException if the CGI script failed
 ** @param path 
 ** @return std::string 
 **/
std::string CGIHandler::getResponse(void)
{
	int fd[2];
	int res = pipe(fd);
	if (res < 0)
		throw std::runtime_error("Pipe failed");
	pid_t pid = fork();

	if (pid < 0)
		throw std::runtime_error("Fork failed");
	if (pid == 0)
		_handleChildRedirection(fd);
	if (pid > 0)
		return (_handleParentRedirection(fd, pid));
	return "";
}