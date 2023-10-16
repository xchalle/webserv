#include "../../include/HttpResponse.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

/**
 * @brief Read the file content till READ_SIZE octet, if is_long is set to 
 * false, stop and close file. Otherwise read till the end
 * @details Result of read is correct for small files, big files, image
 * and vids.
 * @throw std::runtime_error if problem while reading file
 * @return std::string 
 */
void	HttpResponse::get_long_file_content(void)
{
	std::vector<char>			buffer(READ_SIZE);
	
	if (!_file_stream.is_open() && _file_read == false)
		_file_stream.open(_file_path.c_str());

	if (_file_stream.bad())
		throw std::runtime_error("Error while reading file");
	if (!_file_stream.eof())
	{
		_file_stream.read(&buffer[0], READ_SIZE);
		if (_file_stream.bad())
			throw std::runtime_error("Error while reading file");
		this->pushBody(buffer, _file_stream.gcount());
		buffer.clear();
	}
	if (!_file_stream.eof() && _is_long_response == false)
	{
		debug("File is heavy, switching to long response", DEBUG);
		_is_long_response = true;
	}
	else if (_file_stream.eof())
	{
		debug("End of response", DEBUG);
		_is_long_response = false;
		_file_stream.close();
		_file_read = true;
		_response.insert(_response.begin(), '\n');
		_response.insert(_response.begin(), '\r');
	}
}