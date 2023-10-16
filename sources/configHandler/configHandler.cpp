#include "../../include/configHandler.hpp"
#include "../../include/utils.hpp"

ConfigHandler::ConfigHandler()
{
}

ConfigHandler::ConfigHandler(std::string config_file_path)
{
	this->loadConfig(config_file_path);
}

ConfigHandler::~ConfigHandler()
{
	this->deleteConfig();
}

void	ConfigHandler::deletePair(Pair *pair)
{
	for (std::vector<Pair *>::iterator it = pair->child_value.begin(); it != pair->child_value.end(); ++it)
		deletePair(*it);
	delete pair;
}

void ConfigHandler::deleteConfig()
{
	for (std::vector<Pair *>::iterator it = _config.begin(); it != _config.end(); ++it)
		deletePair(*it);
	_config.clear();
}

/**
 * Check if the current key instuction has child instructions
 * contained by { ... }
 * @param config_file_content The config file content
 * @param current_line The current line
 * @return true if the current key instuction has child instructions
 */
bool ConfigHandler::currentLineHasChild(std::vector<std::string> config_file_content, int current_line)
{
	std::string line(trim(config_file_content[current_line + 1], " \t"));

	if (line.compare("{") == 0)
		return (true);
	return (false);
}

std::vector<std::string> ConfigHandler::extractBracketContent(std::vector<std::string>::iterator &bracket_begin, str_it file_end)
{
	std::vector<std::string> bracket_content;

	int bracket_count = 1;
	while (bracket_count > 0 && bracket_begin != file_end)
	{
		std::string line(trim(*bracket_begin, " \t"));
		if (line.compare("{") == 0)
			bracket_count++;
		else if (line.compare("}") == 0)
			bracket_count--;
		bracket_content.push_back(line);
		bracket_begin++;
	}
	if (bracket_count > 0 && bracket_begin == file_end)
		throw std::runtime_error("Error: Missing bracket");
	return (bracket_content);
}

/**
 * Last character of line may be a semicolon
 * if so, remove it
 * @param values The list of values contained in the current line
 */
void ConfigHandler::cleanValues(std::vector<std::string> &values)
{
	if (values.size() == 0)
		return;
	std::string last_value = values[values.size() - 1];
	if (last_value[last_value.size() - 1] == ';')
		values[values.size() - 1] = last_value.substr(0, last_value.size() - 1);
}


/**
 * Trim whitespace around string, and comments after a #
 * @param instr The instruction lines
 */
std::string cleanString(std::string s)
{
	std::string newLine = trim(s, " \t");
	char	is_in_quotes(' ');

	for (size_t i = 0; i < newLine.size(); i++){
        if (newLine[i] == '#'){
            if (is_in_quotes == ' '){
                newLine = newLine.substr(0, i);
                break;
            }
        }
        if (newLine[i] == '"' || newLine[i] == '\''){
           if (is_in_quotes != ' ' && is_in_quotes != newLine[i])
				continue;
            else if (is_in_quotes == newLine[i])
                is_in_quotes = ' ';
			else
                is_in_quotes = newLine[i];
        }
    }
	return (trim(newLine, " \t"));
}

/**
 * @brief Parse the config file line by line and build a vector of Pair recursively
 * 
 * @param config_file_content 
 * @return std::vector<Pair *> 
 */
std::vector<Pair *> ConfigHandler::parseConfig(std::vector<std::string> config_file_content)
{
	std::vector<Pair *> config;
	str_it it = config_file_content.begin();
	str_it end = config_file_content.end();
	std::string skipChars = "{}#";

	while (it != end)
	{
		std::string line = cleanString(*it);
		if (skipChars.find(line[0]) != std::string::npos || line.empty())
		{
			++it;
			continue;
		}
		std::vector<std::string> lineWords(split_with_quotes(line));
		if (lineWords.size() == 0)
		{
			++it;
			continue;
		}
		std::size_t lineIndex = std::distance(config_file_content.begin(), it);
		std::string key = lineWords[0];
		std::vector<std::string> values = std::vector<std::string>(lineWords.begin() + 1, lineWords.end());
		cleanValues(values);
		bool	hasChild = currentLineHasChild(config_file_content, lineIndex);
		if (hasChild)
		{
			it += 2;
			std::vector<std::string> bracket_content = this->extractBracketContent(it, end);
			config.push_back(this->buildPair(key, parseConfig(bracket_content), values));
		} else if ((key == "server" || key == "location") && !hasChild) {
			throw std::runtime_error("Error: Missing bracket at key " + key);
		} else
		{
			config.push_back(this->buildPair(key, values));
			++it;
		}
	}
	return (config);
}

void ConfigHandler::loadConfig(std::string config_file_path)
{
	std::ifstream config_file(config_file_path.c_str());
	std::vector<std::string> config_file_content;
	size_t	line_count = 0;

	if (!config_file.is_open())
		throw std::runtime_error("Config file not found");
	while (config_file.good())
	{
		std::string line;
		std::getline(config_file, line);
		if (line.size() > MAX_LINE_SIZE)
			throw std::runtime_error("Line " + to_string(line_count) + " is too long");
		config_file_content.push_back(line);
		line_count++;
	}
	config_file.close();
	_config = this->parseConfig(config_file_content);
}

std::vector<Pair *> ConfigHandler::getConfig() const
{
	return (_config);
}

Pair *ConfigHandler::buildPair(std::string key, std::vector<std::string> values)
{
	if (key.empty() || values.empty())
		return (NULL);
	Pair *pair = new Pair;
	pair->key = key;
	pair->values = values;
	return (pair);
}

Pair *ConfigHandler::buildPair(std::string key, std::vector<Pair *> child_value, std::vector<std::string> values)
{
	if (key.empty())
		return (NULL);
	Pair *pair = new Pair;
	pair->key = key;
	pair->child_value = child_value;
	pair->values = values;
	return (pair);
}

void ConfigHandler::printPair(Pair pair, int level) const
{
	std::string tabsByLevel = "";

	for (int i = 0; i < level; i++)
		tabsByLevel += "\t";
	std::cout << tabsByLevel << "key: `" << pair.key << "`" << std::endl;
	std::cout << tabsByLevel << "values: `";
	for (str_it it = pair.values.begin(); it != pair.values.end(); ++it)
		std::cout << *it << "`, `";
	std::cout << "`" << std::endl;
	std::cout << tabsByLevel << "child_values: " << std::endl;
	for (std::vector<Pair *>::iterator it = pair.child_value.begin(); it != pair.child_value.end(); ++it)
		printPair(**it, level + 1);
	std::cout << std::endl;
	std::cout << std::endl;
}

void ConfigHandler::printConfig() const
{
	for (std::vector<Pair *>::const_iterator it = _config.begin(); it != _config.end(); ++it)
		printPair(**it, 0);
}