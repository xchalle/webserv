#ifndef DEF_LOOP_HPP
# define DEF_LOOP_HPP
# include <vector>
# include <utility>
# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <signal.h>
# include <string.h>
# include <fcntl.h>
# include <sstream>
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include "configHandler.hpp"
# include "server.hpp"
# include "fdHandler.hpp"
# include "utils.hpp"

# define NO_SOCKET -1
# define NOT_SET -1
# define MAX_CLIENTS 40
# define BUFFER_SIZE 100024
# define MIN_IP 2130706432
# define MAX_IP 2147483648
# define TIMEOUT 30000
# define BAN_TIME 60000
# define CHECK_BAN_TIME 1000
# define MAX_REQ 500
# define MAX_STRAIGHT_REDIRECT 3
# define IP_NUMBER "0123456789."
# define LINUX_HOST "/etc/hosts"
# define COUT(x) std::cout << x << std::endl

class Loop
{
private:
           /**VARIABLES**/
    std::vector<Server>                   *servers;
    std::vector<addrinfo *>               vec_addrinfo;
    /**
     * reference on Servers created with the config file
     */
    std::map<in_addr_t, int>              reqPerIp;
    std::map<in_addr_t, long long int>              banPerIp;
    std::vector<FdHandler>                connections;
    std::vector<FdHandler>                listen_sock;
    std::map<std::string, std::string>    localhosts;
   // std::map<int, HttpRequest>            incomplete_request;
    /**
     * different with webserv cause multiple webserv can be on the same socket;
     */
    fd_set                                read_socks, write_socks, except_socks;
    /**
     * set of socket respectively to read/write in and throw except;
     */              
    int                                   max_sock, save_sock;
    /**
     * the highest socket usefull for select()
     */
    int                                   ret_slct, ret_recv, ret_send;
    /**
     * return value of select()
     */
    struct timeval                        time;

    long long int                         updateTime;

           /**FUNCTION**/
    bool      findHostInVector(std::string &host);
    bool      isIPAddress(const std::string &host);
    int       fillLocalhosts(std::string line);
    void      openHostsFile();
    int       startListenSocket(Server *, const int pos);
    bool      sameBind(const int pos, const uint32_t ip);
    bool      samePort(const int i, const int pos);
    bool      sameHost(const int i, const int pos, const uint32_t ip);
    int       findMaxSock();
    int       closeConnection(const int pos);
    int       closeConnection(FdHandler *client);
    int       resetConnection(const int pos);
    int       handleRequest(FdHandler *client);
    int       acceptNewConnection(int server_sock, const int _port, const std::string _ip);
    int       getPos(const int fd);
    bool      requestInProgress(int fd);
    void      timeoutChecker();
    in_addr_t getAddr(std::string host);
    void      responseHandler(FdHandler *client);
    bool      isIpBanned(FdHandler *client);
    bool      isTooManyRedirect(FdHandler *client);
    bool      checkNumReqRedir(FdHandler *client);
    bool      checkRecv();
    void      checkRedirect(FdHandler *client);
    void      sendResponse(int pos);

public:
    Loop(std::vector<Server> *webservs);
    ~Loop();
    bool      verifServer();
    bool    initSocket();
    int     resetFdSet();
    void    setupMaxSock();
    void    muxingIostream();
    void    connectionChecker();
    void    connectionHandler();
    void    checkIncompleteRequest();
    int     shutdown();
    void    closeSocket();

   // class SystemErrorException : public std::exception
   // {
   //     public:
   //         SystemErrorException(std::string _error)
   //         {
   //             except = _error;
   //         }
   //         virtual const char* what() const throw()
   //         {
   //             return(except.c_str());
   //         }
   //         virtual ~SystemErrorException()
   //         {}
   //     private:
   //         std::string except;
   // };
};


#endif
