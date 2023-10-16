#include "../../include/httpRequest.hpp"

void HttpRequest::construct()
{
    _clen = NOT_SET;
    _bdread = NOT_SET;
    _bad_request = false;
    _headers_bool = false;
    _body_bool = false;
    _mpp_bool = false;
    _chunked = false;
    _complete = false;
    _close = false;
    status_code = 0;
   _max_size = 1000000;
}

HttpRequest::HttpRequest()
{
   /* _string.clear();
    potential.clear();
    _split.clear();
    _mpp.clear();
    _headers.clear();
    _body.clear();
    _chunked_body.clear();
    _ip.clear();
    _boundary.clear();
    status_message.clear();*/
    _port = NOT_SET;
    this->construct();
/*    _clen = NOT_SET;
    _bdread = NOT_SET;
    _bad_request = false;
    _headers_bool = false;
    _body_bool = false;
    _mpp_bool = false;
    _chunked = false;
    _complete = false;
    _close = false;
    status_code = 0;
   // _time = 0;
   _max_size = 1000000;*/
}

HttpRequest::HttpRequest(char *buffer, std::string ip, unsigned int port) : _ip(ip), _port(port)
{
//    status_message.clear();
    _string = buffer;
//    _time = 0;
    this->construct();
    /*_clen = NOT_SET;
    _bdread = NOT_SET;
    _bad_request = false;
    _headers_bool = false;
    _mpp_bool = false;
    _body_bool = false;
    _chunked = false;
    _complete = false;
    _close = false;
   _max_size = 1000000;
    status_code = 0;*/
}

HttpRequest::HttpRequest(char *buffer, std::vector<Server> *_servers, std::string ip, unsigned int port) : _ip(ip), _port(port)
{
    (void)_servers;
    _string = buffer;
//    status_message.clear();
//    _time = 0;
    this->construct();
    /*_clen = NOT_SET;
    _bdread = NOT_SET;
    _bad_request = false;
    _headers_bool = false;
    _mpp_bool = false;
    _body_bool = false;
    _chunked = false;
    _complete = false;
    _close = false;
    _max_size = 1000000;
    status_code = 0;*/
}

HttpRequest::~HttpRequest()
{
}

HttpRequest::HttpRequest(const HttpRequest &rhs)
{
    *this = rhs;
}

HttpRequest &HttpRequest::operator=(const HttpRequest &rhs)
{
    // servers = rhs.servers;
    to_use = rhs.to_use;
    potential = rhs.potential;
    _split = rhs._split;
    _mpp = rhs._mpp;
    _headers = rhs._headers;
    _body = rhs._body;
    _chunked_body = rhs._chunked_body;
    _ip = rhs._ip;
    _port = rhs._port;
    _headers_bool = rhs._headers_bool;
    _body_bool = rhs._body_bool;
    _mpp_bool = rhs._mpp_bool;
    _clen = rhs._clen;
//    _time = rhs._time;
    _string = rhs._string;
    _boundary = rhs._boundary;
    _chunked = rhs._chunked;
    _close = rhs._close;
    _complete = rhs._complete;
    _bdread = rhs._bdread;
   _max_size = rhs._max_size;
   status_code = rhs.status_code;
   status_message = rhs.status_message;

    return (*this);
}

/** SETTERS**/

// void    HttpRequest::setKalive()
//{
//     std::map<std::string, std::vector<std::string> >::iterator  it;
//     if ((it = _headers.find("Connection")) != _headers.end())
//     {
//         std::vector<std::string>::iterator it2 = it->second.begin();
//         if (it->second.size() != 1)
//         {
//             _kalive = "keep-alive";
//             return ;
//         }
//         if(it2->compare("close") == 0)
//         {
//             _kalive = "close";
//             return ;
//         }
//     }
//     _kalive = "keep-alive";
//     return ;
// }

void HttpRequest::setBoundary()
{
    std::map<std::string, std::vector<std::string> >::iterator it;
    if ((it = _headers.find("Content-Type")) != _headers.end())
    {
        std::vector<std::string>::iterator it2 = it->second.begin();
        for (; it2 != it->second.end(); it2++)
        {
            if (it2->compare(0, strlen(BOUNDARY_KEY), BOUNDARY_KEY) == 0)
            {
                _boundary = it2->substr(strlen(BOUNDARY_KEY));
                return ;
            }
        }
    }
    return;
}

void HttpRequest::setClength()
{
    std::map<std::string, std::vector<std::string> >::iterator it;
    if ((it = _headers.find("Content-Length")) != _headers.end())
    {
        std::vector<std::string>::iterator it2 = it->second.begin();
        for (; it2 != it->second.end(); it2++)
        {
            if (it2->find_first_not_of(DIGITS) == std::string::npos)
            {
                _clen = atoll(it2->c_str());
                return;
            }
        }
    }
    _clen = -1;
    return;
}

//TODO maybe delete
/*void HttpRequest::setServer(std::vector<Server> *_servers)
{
    (void)_servers;
    //servers = _servers;
}*/
void HttpRequest::setString(const char *_buffer)
{
    _string = _buffer;
}
void HttpRequest::setString(const std::string _buffer)
{
    _string = _buffer;
}
void HttpRequest::setPort(const int port) 
{
    _port = port;
}
void HttpRequest::setIp(const std::string ip) 
{
    _ip = ip;
}

void HttpRequest::setMaxSize(long unsigned size)
{
    _max_size = size;
}

void    HttpRequest::setStatus(const int code, const std::string &message)
{
    this->status_code = code;
    this->status_message = message;
    _complete = true;
    _close = true;
}

/** GETTERS **/

bool HttpRequest::getMppBool() const
{
    return (_mpp_bool);
}

bool HttpRequest::getBodyBool() const
{
    return (_body_bool);
}

bool HttpRequest::getHeadersBool() const
{
    return (_headers_bool);
}

std::vector<std::string> HttpRequest::getMpp() const
{
    if (_mpp.size() > 0)
        return (_mpp);
    return (std::vector<std::string>());
}

std::map<std::string, std::vector<std::string> > HttpRequest::getHeaders() const
{
    return (_headers);
}

std::string HttpRequest::getBody() const
{
    return (_body);
}

/*long long int HttpRequest::getTime() const
{
    return (_time);
}*/

long long int HttpRequest::getClength() const
{
    return (_clen);
}

std::string HttpRequest::getBoundary() const
{
    return (_boundary);
}

std::string HttpRequest::getMethod() const
{
	if (_mpp.size() < 1)
		return ("");
    return (_mpp[0]);
}

std::string HttpRequest::getPath() const
{
	if (_mpp.size() < 2)
		return ("");
	return (_mpp[1]);
}

std::string HttpRequest::getDecodedPath() const
{
	return (decode_url(this->getPath()));
}

/**
 * @brief Get the Clean Path object without the query string
*/
std::string HttpRequest::getCleanPath() const
{
	std::string path = this->getPath();

	size_t pos = path.find('?');
	if (pos != std::string::npos)
		path = path.substr(0, pos);
	return (decode_url(path));
}

std::string HttpRequest::getQuery() const
{
	std::string path = this->getPath();
	
	size_t pos = path.find('?');
	if (pos != std::string::npos)
		return (path.substr(pos + 1));
	return ("");
}

std::string HttpRequest::getVersion() const
{
	if (_mpp.size() < 3)
		return ("");
    return (_mpp[3]);
}

std::string HttpRequest::getIp() const
{
	return (_ip);
}

int     HttpRequest::getStatusCode() const
{
    return (status_code);
}

std::string     HttpRequest::getStatusMessage() const
{
    return (status_message);
}

bool    HttpRequest::getStatus() const
{
    return (_complete);
}

bool    HttpRequest::getClose() const
{
    return (_close);
}

int    HttpRequest::getBodyParsing() const
{
    return (_bdread);
}

long unsigned HttpRequest::getMaxSize() const
{
    return (_max_size);
}

Server HttpRequest::getServerToUse() const
{
    return (to_use);
}