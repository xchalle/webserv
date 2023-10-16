#include "../../include/server.hpp"
#include "../../include/utils.hpp"

Server::~Server()
{
}

Server::Server(const Server &other)
{
	*this = other;
}

Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		BaseServerConfig::operator=(other);
		_host = other._host;
		_port = other._port;
		_server_names.assign(other._server_names.begin(), other._server_names.end());
		_locations.assign(other._locations.begin(), other._locations.end());
		_max_body_size = other._max_body_size;
		
	}
	return *this;
}

void Server::setHost(Pair *server_child_value)
{
	if (server_child_value->key != "host")
		throw std::invalid_argument("host");
	if (server_child_value->values.size() != 1)
		throw std::invalid_argument("Host must have only one value");
	_host = server_child_value->values[0];
}

void Server::setHost(std::string host)
{
	_host = host;
}

/**
 * @brief Define the port on which the server will listen
 * Port must be between 0 and 65535
 */
void Server::setPort(Pair *server_child_value)
{
	if (server_child_value->key != "listen")
		throw std::invalid_argument("listen");
	if (server_child_value->values.size() != 1)
		throw std::invalid_argument("Port must be defined with a single value");
	unsigned int port = 0;
	if (server_child_value->values[0].find(":") != std::string::npos)
	{
		std::string ip = server_child_value->values[0].substr(0, server_child_value->values[0].find(":"));
		if (!is_valid_ipv4(ip))
			throw std::invalid_argument("Invalid IP address format : %d.%d.%d.%d");
		_host = ip;
		port = str_to_int(server_child_value->values[0].substr(server_child_value->values[0].find(":") + 1));
	} else {
		_host = "127.0.0.1";
		port = str_to_int(server_child_value->values[0]);
	}
	if (port > 65535)
		throw std::invalid_argument("Port must be between 0 and 65535");
	_port = port;
}

/**
 * Define the server name
 */
void Server::setServerNames(Pair *server_child_value)
{
	if (server_child_value->key != "server_name")
		throw std::invalid_argument("server_name");
	if (server_child_value->values.size() != 1)
		throw std::invalid_argument("Server name must be defined with a single value");
	if (!is_servername_valid(server_child_value->values[0]))
		throw std::invalid_argument("Invalid server name");
	_server_names.push_back(server_child_value->values[0]);
}

/**
 * Define a configuration for a defined url path
 * @example `location /my-path { ... }`
 */
void Server::setLocations(Pair *server_child_value)
{
	if (server_child_value->key != "location")
		throw std::invalid_argument("location");
	if (server_child_value->values.size() != 1)
		throw std::invalid_argument("Location must contain a path followed by a block");
	_locations.push_back(Location(server_child_value, server_child_value->values[0]));
}

/**
 * Define the maximum size of the body of a request
 * @example client_max_body_size 1000000;
 */
void Server::setMaxBodySize(Pair *server_child_value)
{
	if (server_child_value->key != "client_max_body_size")
		throw std::invalid_argument("client_max_body_size");
	if (server_child_value->values.size() != 1)
		throw std::invalid_argument("Max body size must be defined with a single value");
	try {
		int	tmp = str_to_int(server_child_value->values[0]);
		if (tmp < 0)
			throw std::invalid_argument("Max body size must be a positive integer");
		_max_body_size = tmp;
	} catch (std::invalid_argument &e) {
		throw std::invalid_argument("Second argument must be an valid integer");
	}
}

std::string Server::getHost() const
{
	return (_host);
}

unsigned int Server::getPort() const
{
	return (_port);
}

std::vector<std::string> Server::getServerName() const
{
	return (_server_names);
}

std::vector<Location> Server::getLocations() const
{
	return (_locations);
}

/**
 * Loop trough each Location and return the one with the 
 * the longest matching prefix. If find exact match, return it.
 * If no match found, return the first location
 **/
Location *Server:: getLocation(std::string path)
{
	if (_locations.size() == 0)
		throw std::invalid_argument("No location defined");
	if (path.empty())
		throw std::invalid_argument("Path is empty");
	std::pair<unsigned int, size_t>	maxLength = std::make_pair(0, 0);
	for (std::vector<Location>::iterator locationIterator = _locations.begin(); locationIterator != _locations.end(); ++locationIterator)
	{
		if (path.compare(locationIterator->getPath()) == 0)
			return (&(*locationIterator));
		size_t matchingLength = get_prefix_matching_length(locationIterator->getPath(), path);
		if (matchingLength > maxLength.first)
			maxLength = std::make_pair(matchingLength, locationIterator - _locations.begin());
	}
	if (maxLength.first == 0 && _locations.size() == 0)
		return (NULL);
	return (&(_locations[maxLength.second]));
}

long Server::getMaxBodySize() const
{
	if (_max_body_size == -1)
		return (DEFAULT_MAX_BODY_SIZE);
	return (_max_body_size);
}