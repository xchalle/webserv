#include "../../include/server.hpp"
#include "../../include/utils.hpp"
#include <iostream>


void	Server::handle_child_value(Pair *current_pair)
{
	if (!current_pair)
		return;
	if (current_pair->key == "root") {
		setRoot(current_pair);
	} else if (current_pair->key == "listen") {
		setPort(current_pair);
	} else if (current_pair->key == "host") {
		setHost(current_pair);
	} else if (current_pair->key == "server_name") {
		setServerNames(current_pair);
	} else if (current_pair->key == "error_page") {
		setErrorsRedirect(current_pair);
	} else if (current_pair->key == "location") {
		setLocations(current_pair);
	} else if (current_pair->key == "client_max_body_size") {
		setMaxBodySize(current_pair);
	} else if (current_pair->key == "return") {
		setRedirectRules(current_pair);
	} else if (current_pair->key == "autoindex") {
		setAutoIndex(current_pair);
	}  else {
		throw std::invalid_argument(current_pair->key.c_str());
	}
}

Server::Server()
	: BaseServerConfig(), _host(""), _port(0), _max_body_size(-1)
{
}

Server::Server(Pair* current_server)
	: BaseServerConfig(), _host("0.0.0.0"), _port(80), _max_body_size(-1)
{
	if (current_server->key.compare("server") != 0)
		throw std::invalid_argument("Server config is invalid");
	size_t instruction_index(0);

	_root = "/var/www";
	for (std::vector<Pair*>::iterator it = current_server->child_value.begin(); it != current_server->child_value.end(); ++it)
	{
		Pair *current_pair = *it;
		try {
			handle_child_value(current_pair);
		} catch (std::invalid_argument &e) {
			std::cerr << "An error has occurred while parsing server config, affected key : `" << e.what() << "`. Instructions #" << instruction_index + 1 << std::endl;
			throw std::runtime_error("Invalid server config");
		}
		instruction_index++;
	}
}