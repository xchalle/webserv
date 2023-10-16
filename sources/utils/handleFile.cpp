#include "../../include/utils.hpp"
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

bool file_is_readable(const std::string &path)
{
	std::ifstream file(path.c_str());
	bool		is_readable = file.good();
	file.close();
	return (is_readable);
}

bool	file_exists(const std::string &path)
{
	struct stat buf;
	return (stat(path.c_str(), &buf) == 0);
}

bool	folder_exists_and_is_readable(const std::string &path)
{
	return (access(path.c_str(), F_OK) == 0 && access(path.c_str(), R_OK) == 0);
}

bool is_directory(const std::string &path)
{
	struct stat buf;

	stat(path.c_str(), &buf);
	return (S_ISDIR(buf.st_mode));
}

std::vector<file>	get_files_in_directory(const std::string &path)
{
	std::vector<file> files;
	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(path.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] != '.')
			{
				file f;
				f.name = ent->d_name;
				f.is_dir = ent->d_type == DT_DIR;
				files.push_back(f);
			}
		}
		closedir(dir);
	}
	return (files);
}

/**
 * Read the file content. Watch out, this function is not optimized
 * to read big files
*/
std::string get_file_content(const std::string &path)
{
	std::ifstream file(path.c_str());
	std::string content;
	
	if (!file.good())
		throw std::runtime_error("Error while opening file " + path);
	debug("Reading file " + path, DEBUG);
	while (file.good())
	{
		std::string line;
		std::getline(file, line);
		content += line + "\n";
	}
	file.close();
	return (content);
}