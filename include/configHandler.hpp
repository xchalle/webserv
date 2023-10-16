#ifndef DEF_CONFIGHANDLER_HPP
# define DEF_CONFIGHANDLER_HPP
# define str_it std::vector<std::string>::iterator
# define MAX_LINE_SIZE 8190
# include <vector>
# include <string>
# include <fstream>
# include <iterator>
# include <iostream>
# include "./Pair.hpp"

class ConfigHandler
{
public:
	ConfigHandler();
	ConfigHandler(std::string config_file_path);
	~ConfigHandler();

	void				loadConfig(std::string config_file_path);
	std::vector<Pair*>	parseConfig(std::vector<std::string> config_file_content);
	std::vector<Pair*>	getConfig() const;

	void	printConfig(void) const;

private:
	std::vector<Pair*>			_config;

	Pair* 	buildPair(std::string key, std::vector<std::string> values);
	Pair*	buildPair(std::string key, std::vector<Pair*> child_value, std::vector<std::string> values);

	void	deleteConfig();
	void	deletePair(Pair *pair);
	
	void	printPair(Pair pair, int level) const;
	void	cleanValues(std::vector<std::string> &values);
	void	pop_comments(std::vector<std::string> &values);
	bool	currentLineHasChild(std::vector<std::string> config_file_content, int current_line);
	std::vector<std::string>    extractBracketContent(std::vector<std::string>::iterator &bracket_begin, str_it file_end);
};
#endif