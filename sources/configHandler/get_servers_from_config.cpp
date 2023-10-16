#include "../../include/configHandler.hpp"
#include "../../include/server.hpp"
#include "../../include/utils.hpp"

std::vector<Server> init_servers(std::vector<Pair *> config)
{
	Pair *config_pair;
	std::vector<Server> servers;

	for (size_t index = 0; index < config.size(); index++)
	{
		config_pair = config[index];
		if (config_pair->key == "server")
		{
			try
			{
				Server server(config_pair);
				servers.push_back(server);
				debug("Server #" + int_to_str(index + 1) + " created with success", INFO);
			}
			catch (const std::exception &e)
			{
				std::cerr << "An error occured while creating server #"
						  << index++ << " : " << e.what() << std::endl;
				throw std::runtime_error("Error while creating server");
			}
		}
		else
			throw std::runtime_error("Invalid key `" + config_pair->key + "`");
	}
	return (servers);
}

std::vector<Server> get_servers_from_config(std::string config_file_path)
{
	debug("Loading config file at `" + config_file_path + "`", INFO);
	try
	{
		ConfigHandler configHandler(config_file_path);
		std::vector<Pair *> config = configHandler.getConfig();
		std::vector<Server> servers = init_servers(config);
		return (servers);
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << "An error occured while loading `"
				  << config_file_path << "` ";
		std::cerr << e.what() << std::endl;
		throw std::runtime_error("Error while loading config file");
	}
	catch (...)
	{
		std::cerr << "An unknown error occured while loading `"
				  << config_file_path << "`" << std::endl;
		throw std::runtime_error("Error while loading config file");
	}
}