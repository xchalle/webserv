#include "../include/utils.hpp"
#include "../include/loop.hpp"

bool terminated = false;

void exitloop(int signum)
{
	(void)signum;
	terminated = true;
	std::cout << "Exiting..." << std::endl;
}

void print_active_servers(std::vector<Server> servers)
{
	std::cout << "Number of servers: " << servers.size() << std::endl;
	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		std::cout << "server : "
				  << "Host : " << it->getHost() << " Port : " << it->getPort() << std::endl;
		std::cout << "http://" << it->getHost() << ":" << it->getPort() << std::endl;
		std::vector<std::string> server_names = it->getServerName();
		for (std::vector<std::string>::iterator it2 = server_names.begin(); it2 != server_names.end(); it2++)
		{
			std::cout << " ServerName : " << *it2;
			std::cout << "server autoIndex : " << it->getAutoindex() << std::endl;
		}
		std::cout << std::endl;
	}
}

int main(int argc, char **argv)
{
	signal(SIGINT, exitloop);
	try {
		std::vector<Server> servers;
	
		if (argc == 1)
		{
			std::cout << "webserv: usage: config file needed as argument: ./webserv file_path" << std::endl;
			servers = get_servers_from_config("./config/simple.conf");
		}
		else if (argc == 2)
			servers = get_servers_from_config(std::string(argv[1]));
		else
		{
			std::cout << "webserv: usage: config file needed as argument: ./webserv file_path" << std::endl;
			return (EXIT_SUCCESS);
		}
		print_active_servers(servers);
		Loop loop(&servers);
		if (!(loop.verifServer()))
			return (EXIT_SUCCESS);
		if (!loop.initSocket())
			return (EXIT_SUCCESS);
		while (!terminated)
		{
			// loop.checkIncompleteRequest();
			loop.resetFdSet();
			loop.muxingIostream();
		}
		loop.closeSocket();
		servers.clear();
	} catch (std::runtime_error &e) {
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}