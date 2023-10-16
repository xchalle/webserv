#ifndef DEF_SERVER_HPP
# define DEF_SERVER_HPP
# define DEFAULT_MAX_BODY_SIZE 10000000
# include <string>
# include <vector>
# include <map>
# include <stdexcept>
# include "./Pair.hpp"
# include "./location.hpp"

struct DefinedError
{
	unsigned int	http_code;
	std::string	path;
};

class Server : public BaseServerConfig
{
private:
	std::string		_host;
	unsigned int	_port;
	std::vector<std::string>	_server_names;
	std::vector<Location> 	_locations;
	long 			_max_body_size;

	void	handle_child_value(Pair *current_pair);

public:
	Server();
	Server(Pair* current_server);
	~Server();
	Server(const Server &other);
	Server &operator=(const Server &other);

	/** SETTERS **/

	void setHost(Pair *server_child_value);
	void setHost(std::string host);
	void setPort(Pair *server_child_value);
	void setServerNames(Pair *server_child_value);
	void setLocations(Pair *server_child_value);
	void setMaxBodySize(Pair *server_child_value);

	/** GETTERS **/

	std::string getHost() const;
	unsigned int getPort() const;
	std::vector<std::string> getServerName() const;
	std::vector<Location> getLocations() const;
	Location *getLocation(std::string path);
	long getMaxBodySize() const;
};

#endif