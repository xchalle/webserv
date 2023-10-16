#include "../../include/HttpResponse.hpp"
#include "../../include/httpRequest.hpp"
#include "../../include/utils.hpp"
#include <fstream>

/**
 * Parse values of file headers like `form-data; name="avatar"; filename="a.txt"`
 * to a map of key/value [{key, value},{key, value}]
*/
std::map<std::string, std::string> parse_semicolon_values(std::string values)
{
	std::map<std::string, std::string> result;
	std::vector<std::string> values_list = split(values, ";");

	for (std::vector<std::string>::iterator it = values_list.begin(); it != values_list.end(); it++)
	{
		std::vector<std::string> key_value = split(*it, "=");
		std::string key = trim(key_value[0], " ");
		if (key_value.size() > 1)
		{
			std::string value = trim(key_value[1], " ");
			if (value[0] == '"' && value[value.size() - 1] == '"')
				value = value.substr(1, value.size() - 2);
			result[key] = value;
		} else {
			result[key] = "";
		}
	}
	return (result);
}

/**
 * Read the file headers and fill the HTTPFileInfo struct with the values
 * of ` Content-Disposition` header
 * @param info the HTTPFileInfo struct to fill
 * @param content_disposition the value of the `Content-Disposition` header `form-data; name="avatar"; filename="a.txt"`
*/
void	handle_content_disposition(HTTPFileInfo & info, std::string content_disposition)
{
	std::map<std::string, std::string>	content_disposition_parts
		= parse_semicolon_values(content_disposition);
	info.name = content_disposition_parts["name"];
	info.extension = content_disposition_parts["filename"].substr(content_disposition_parts["filename"].find_last_of(".") + 1);
	info.filename = content_disposition_parts["filename"].substr(0, content_disposition_parts["filename"].find_last_of("."));
}


/**
 * @throw std::runtime_error if content-disposition header is not found
*/
HTTPFileInfo getFileInfo(std::map<std::string, std::vector<std::string> > file_headers)
{
	HTTPFileInfo				info;
	std::vector<std::string>	content_disposition = file_headers["Content-Disposition"];

	if (content_disposition.size() == 0)
		throw std::runtime_error("No Content-Disposition header found");
	handle_content_disposition(info, content_disposition[0]);
	return (info);
}

/**
 * Extract the file number from the file name if 
 * - the number is at the end of the file name
 * - the number is preceded by a `-`
 * - the number is under max int
*/
std::string	update_file_number(std::string filename)
{
	int		number = 0;
	size_t	pos = filename.find_last_of("-");

	if (pos == std::string::npos)
		return (filename + "-1");
	std::string number_str = filename.substr(pos + 1);
	if (number_str.size() == 0)
		return (filename + "-1");
	try {
		number = str_to_int(number_str);
		return (filename.substr(0, pos + 1) + to_string(number + 1));
	} catch (std::exception & e) {
		debug("exception on read number", DEBUG);
		return (filename + "-1");
	}
}

/**
 * @brief Determine the file name of a file inside a multipart/form-data request.
 * The name is determined in the following order:
 * 1. The filename parameter of the Content-Disposition header
 * 2. The name parameter of the Content-Disposition header
 * 3. Default name: `DEFAULT_FILE_NAME`
 * If the file already exists, a number is appended to the file name `file-1.ext`
 * @param info The HTTPFileInfo struct containing the file information
 * @param insertion_folder The folder where the file will be inserted
 * @return The file name
*/
std::string HttpResponse::get_file_name(HTTPFileInfo info, std::string insertion_folder)
{
	std::string filename;
	std::string tmp_filename;

	if (info.filename.size() > 0)
		filename = info.filename;
	else if (info.name.size() > 0)
		filename = info.name;
	else
		filename = DEFAULT_FILE_NAME;
	insertion_folder = (insertion_folder[insertion_folder.size() - 1] == '/') ? insertion_folder : insertion_folder + "/";
	if (folder_exists_and_is_readable(insertion_folder) == false)
		throw HttpResponse::FileUploadFailed();
	if (file_exists(insertion_folder + filename + "." + info.extension) == false)
		return (filename + "." + info.extension);
	tmp_filename = filename;
	debug("File saving : file already exists, updating file number", INFO);
	while (file_exists(insertion_folder + tmp_filename + "." + info.extension))
	{
		tmp_filename = update_file_number(tmp_filename);
		debug("File saving : Trying to save file with name " + tmp_filename + "." + info.extension, DEBUG);
	}
	return (tmp_filename + "." + info.extension);
}

void	HttpResponse::saveFile(std::map<std::string, std::vector<std::string> > headers, size_t sof, size_t eof)
{
	HTTPFileInfo info = getFileInfo(headers);
	std::string filename;

	sof = this->_request.getBody().find("\r\n\r\n", sof) + 4;
	try {
		filename = get_file_name(info, this->_current_location->getUploadStore());
	} catch (HttpResponse::FileUploadFailed & e) {
		debug("File saving : Failed to save file, folder is not accessible", ERROR);
		_status_code = 409;
		_status_message = "Conflict";
		throw std::exception();
	}
	std::string file_path = this->_current_location->getUploadStore() + filename;
	std::ofstream file(file_path.c_str(), std::ios::binary);
	debug("Saving file: " + file_path, DEBUG);
	file << this->_request.getBody().substr(sof, eof - sof);
	file.close();
	_status_code = 201;
	_status_message = "Created";
}

/**
 * @brief Read body from starting boundary till the end of the body,
 * and write each part to a file.
*/
void	HttpResponse::handleUploads(size_t pos)
{
	size_t	boundary_len = this->_request.getBoundary().size();
	size_t	next_boundary_pos;
	size_t 	sof;
	std::vector<std::string> part_header;

	if (pos > 2 && this->_request.getBody().substr(pos - 2, boundary_len + 4 + 2) == "\r\n--"+ this->_request.getBoundary() + "--") {
		debug("Boundary: end boundary found", DEBUG);
		return;
	}
	if (this->_request.getBody().find("--" + this->_request.getBoundary()) == std::string::npos)
		throw std::runtime_error("Invalid boundary");
	sof = pos + boundary_len + 2;
	next_boundary_pos = this->_request.getBody().find("--" + this->_request.getBoundary(), sof);
	if (next_boundary_pos == std::string::npos)
		throw std::runtime_error("Invalid boundary");
	std::string header_part = this->_request.getBody().substr(pos, this->_request.getBody().find("\r\n\r\n", pos));
	this->saveFile(extract_headers(header_part), sof, next_boundary_pos);
	this->handleUploads(next_boundary_pos);
}

/**
 ** @brief Check if the request is a valid upload request
 ** - Check if the request is a POST request
 ** - Check if the location is enabled for uploads
 ** - Check if the request contains a body
 ** - Check if the request contains a Content-Type header
 ** - Check if the first Content-Type header contains a boundary
 ** 
 ** @return {bool} true if the request is a POST request and the location is enabled for uploads
 **/
bool	HttpResponse::isValidUpload(void)
{
	if (this->_request.getMethod() != "POST")
		return (false);
	if (this->_current_location->getUploadEnabled() == false)
		return (false);
	if (this->_request.getBody().empty())
	{
		debug("Request body was empty, upload quit", DEBUG);
		return (false);
	}
	if (this->_request.getBoundary().empty())
	{
		debug("Request boundary was empty, upload quit", DEBUG);
		return (false);
	}
	try {
		this->handleUploads(0);
		setBody("File(s) uploaded successfully");
	} catch (std::exception &e) {
		debug("Error while saving files: " + std::string(e.what()), ERROR);
		if (this->_status_code == 200 || this->_status_code == 201)
		{
			_status_code = 400;
			_status_message = "Bad Request";
		}
		return (true);
	}
	return (true);
}