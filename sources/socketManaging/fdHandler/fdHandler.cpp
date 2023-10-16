#include "../../../include/fdHandler.hpp"
#include <iostream>

void FdHandler::construct()
{
    fd = -1;
    port = 0;
    alive = true;
    recv_bit = 0;
    send_bit = 0;
    nb_request = 0;
    straightredirect = 0;
    response_ptr = NULL;
    setCreaTime();
}

FdHandler::FdHandler(const int _fd)
{
    this->construct();
    fd = _fd;
    /*response_ptr = NULL;
    port = 0;
    alive = true;
    recv_bit = 0;
    send_bit = 0;
    nb_request = 0;
    straightredirect = 0;
    response_ptr = NULL;
    setCreaTime();*/
}
FdHandler::FdHandler(const int _fd, std::vector<Server> *_server)
{
    (void)_server;
    this->construct();
    fd = _fd;
   // request.setServer(_server);
 /*   response_ptr = NULL;
    port = 0;
    alive = true;
    recv_bit = 0;
    send_bit = 0;
    nb_request = 0;
    straightredirect = 0;
    response_ptr = NULL;
    setCreaTime();*/
}
FdHandler::FdHandler(const int _fd, const unsigned int _port, std::string _ip)
{
    this->construct();
    fd = _fd;
    ip = _ip;
    port = _port;
/*    response_ptr = NULL;
    alive = true;
    recv_bit = 0;
    send_bit = 0;
    nb_request = 0;
    straightredirect = 0;
    response_ptr = NULL;
    setCreaTime();*/
}
FdHandler::FdHandler(const FdHandler& rhs)
{
    *this = rhs;
}

FdHandler& FdHandler::operator=(const FdHandler& rhs)
{
    fd = rhs.fd;
    port = rhs.port;
    ip = rhs.ip;
    alive = rhs.alive;
    request = rhs.request;
    response = rhs.response;
    recv_bit = rhs.recv_bit;
    send_bit = rhs.send_bit;
    nb_request = rhs.nb_request;
    creaTime = rhs.creaTime;
    straightredirect = rhs.straightredirect;
    response_ptr = rhs.response_ptr;
    return (*this);
}
FdHandler::~FdHandler()
{
}

void    FdHandler::resetRequest()
{
    request.reset();
    setTime();
}

bool FdHandler::getAlive() const
{
    return (alive);
}
int FdHandler::getFd() const
{
    return (fd);
}
int FdHandler::getPort() const
{
    return (port);
}
std::string FdHandler::getIp() const
{
    return (ip);
}

in_addr_t FdHandler::getClientIp() const
{
    return (clientIp);
}

long long int   FdHandler::getTime() const
{
    return (time);
}

long long int   FdHandler::getCreaTime() const
{
    return (creaTime);
}

void    FdHandler::setTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    time = (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void    FdHandler::setCreaTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    creaTime = (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void    FdHandler::setAlive(const bool _alive)
{
    alive = _alive;
}
void    FdHandler::setFd(const int _fd)
{
    fd = _fd;
}
void    FdHandler::setPort(const int _port)
{
    port = _port;
}
void    FdHandler::setIp(const std::string _ip)
{
    ip = _ip;
}

void    FdHandler::setClientIp(const in_addr_t _ip)
{
    clientIp = _ip;
}
void    FdHandler::setStatus(const int code, const std::string &message)
{
    request.setStatus(code, message);
}

bool    FdHandler::toClose() const
{
    return (request.getClose());
}
bool    FdHandler::requestInProgress()
{
    if (request.isEmpty())
        return false;
    return true;
}

void    FdHandler::fill(std::string _buffer,  std::vector<Server> *_server)
{
    request.setPort(this->port);
    request.setIp(this->ip);
    request.setString(_buffer);
    int save = request.getBodyParsing();
    int request_complete = request.getStatus(); 
    if (save == -1 && !request_complete)
        request.parseHeaders();
    if (!request_complete && request.getBodyParsing() > 0)
    {
        if (request.getBodyParsing() != save)
        {
            Server to_use;
            to_use = request.checkHost(port, ip, _server);
            request.setMaxSize(to_use.getMaxBodySize());
        }
        request.parseBody();
    }
}