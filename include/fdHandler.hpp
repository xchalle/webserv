#ifndef DEF_FDHANDLER_HPP
# define DEF_FDHANDLER_HPP
# include <string>
# include "httpRequest.hpp"
# include "HttpResponse.hpp"
# include <netinet/in.h>

class FdHandler
{
private:
    bool            alive;
    int             fd;
    unsigned int    port;
    std::string     ip;
    in_addr_t       clientIp;
    long long int   time;
    long long int   creaTime;

    bool            requestInProgress();
    void            construct();

public:

    HttpResponse            *response_ptr;
    std::vector<char>       response;
    std::vector<Server>     to_use;
    HttpRequest             request;
    std::size_t             recv_bit;
    std::size_t             send_bit;
    unsigned int            nb_request;
    unsigned int            straightredirect;

    /*FUNCTIONS*/

    FdHandler();
    FdHandler(const int _fd);
    FdHandler(const int _fd, std::vector<Server> *_server);
    FdHandler(const int _fd, const unsigned int _port, std::string _ip);
    FdHandler(const FdHandler &rhs);
    FdHandler &operator=(const FdHandler &rhs);
    ~FdHandler();

    bool toClose() const;
    void resetRequest();
    void fill(std::string _buffer, std::vector<Server> *_server);

    /*GETTERS*/

    bool            getAlive() const;
    int             getFd() const;
    int             getPort() const;
    std::string     getIp() const;
    in_addr_t       getClientIp() const;
    long long int   getTime() const;
    long long int   getCreaTime() const;

    /*SETTERS*/

    void    setAlive(const bool _alive);
    void    setFd(const int _fd);
    void    setPort(const int _port);
    void    setIp(const std::string _ip);
    void    setClientIp(const in_addr_t _ip);
    void    setTime();
    void    setCreaTime();
    void    setStatus(const int code, const std::string &message);
};

#endif