#ifndef DEF_UTILS_HPP
# define DEF_UTILS_HPP
# define str_it std::vector<std::string>::iterator
# define LEVEL 3
# define FOLDER_SVG_PATH "assets/images/folder.svg"
# define FILE_SVG_PATH "assets/images/file.svg"
# define AUTO_INDEX_CSS_PATH "assets/css/autoindex.css"
# define READ_SIZE 8192
# include <iostream>
# include <string>
# include <algorithm>
# include <vector>
#include <sys/time.h>
# include "./server.hpp"

enum debug_level
{
	DEBUG = 0,
	INFO = 1,
	WARNING = 2,
	ERROR = 3
};

struct	file 
{
	std::string name;
	bool is_dir;
};

std::vector<Server> get_servers_from_config(std::string config_file_path);

std::string trim(std::string &str, std::string to_replace);
std::vector<std::string> split(std::string str, std::string to_replace);
std::vector<std::string> split_with_quotes(std::string str);
std::vector<std::string> split_quotes_tab(std::string str);
std::vector<std::string> split_first_of(std::string str, std::string to_replace);
std::vector<std::string> split_string(std::string str, std::string to_replace);
std::vector<std::string> split_first_occurence(std::string s, std::string delimiter);

std::string join(std::vector<std::string> vec, std::string separator);

std::string normstring(std::string _to_norm);

bool is_servername_valid(std::string server_name);
bool is_error_page_valid(Pair *error_page);
bool is_http_code_valid(unsigned int code);
bool is_valid_ipv4(const std::string &ip);

void debug(std::string message, debug_level level);

size_t get_matching_length(const std::string &str1, const std::string &str2);
size_t get_prefix_matching_length(const std::string &location, const std::string &target);

int		atoi_hex(std::string &str);

int	str_to_int(const std::string &s);
std::string	int_to_str(int n);

bool file_is_readable(const std::string &path);
bool	file_exists(const std::string &path);
bool	folder_exists_and_is_readable(const std::string &path);
bool is_directory(const std::string &path);
std::vector<file>	get_files_in_directory(const std::string &path);
std::string get_file_content(const std::string &path);

std::string to_string(int n);
std::string to_string(unsigned int n);
std::string	to_string(size_t n);
std::string to_string(long long n);
std::string to_string(ssize_t n);

std::string url_decode(const std::string & s);

std::string to_uppercase(std::string str);

long long int get_time();

#endif