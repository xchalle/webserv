#ifndef DEF_HTTP_REQUEST_HPP
# define DEF_HTTP_REQUEST_HPP
# include "server.hpp"
# include "utils.hpp"
# include "Pair.hpp"
# include <utility>
# include <sys/time.h>
# include <cstring>
# include <string>
# include <stdlib.h>
# include <limits.h>

# define STR(x) "x"
# define NB_ELEM_SL 3
# define HTTP_VERSION "HTTP/1.1"
# define DIGITS "0123456789"
# define BOUNDARY_KEY "boundary="
# define NOT_SET -1
# define END_OF_HEADERS "\r\n\r\n"
# define END_OF_CHUNKED "0\r\n\r\n"
# define SPLIT_OF_CHUNKED "\r\n\r\n"
# define EOL "\r\n"
# define OWS " \t"
# define OBS_FOLD "\r\n\t"
# define OBS_FOLD2 "\r\n "
# define SP " "
# define CR "\r"

std::string decode_url(const std::string & s);

class HttpRequest
{
private:
    /* data */
    //std::vector<Server>                                 *servers;
    std::string                                         _ip;
    int                                                 _port; 

    Server                                              to_use;
    std::vector<Server>                                 potential;

    std::vector<std::string>                            _split;
    std::vector<std::string>                            _mpp;
    std::map<std::string, std::vector<std::string> >    _headers;
    std::string                                         _body; //can be change for vector to split the multipart
    std::string                                         _chunked_body; //can be change for vector to split the multipart


    bool                                                _headers_bool;
    bool                                                _mpp_bool;
    bool                                                _body_bool;
    bool                                                _complete;
    bool                                                _bad_request;

    long unsigned                                       _max_size;
    long long                                           _clen;
    bool                                                _chunked;
    //long long int                                       _time;
    bool                                                _close;
    int                                                 _bdread;
    int                                                 status_code;
    std::string                                         status_message;

    std::string                                         _boundary;
    std::string                                         _string;
    std::string                                         _keeper;
public:
    HttpRequest();
    HttpRequest(char* buffer, std::vector<Server> *_servers, std::string ip, unsigned int port);
    HttpRequest(char* buffer, std::string ip, unsigned int port);
    HttpRequest(const HttpRequest& rhs);
    HttpRequest& operator=(const HttpRequest& rhs);
    ~HttpRequest();

    void            reset();
    bool            isComplete();
    void            fill();
    void            setStatus(const int code, const std::string &message);

private:

    void            construct();
    void            DefineHTReadBody();
    bool            ImplementedEncoding(std::string to_cmp);
    bool            checkTransferEncoding();
    bool            checkTransferEncoding(size_t end);
    void            TransferEncodingParsing();
    void            LengthParsing();
    void            assignSplit();
    void            fillMpp();
    void            fillBody(int pos);
    int             fillHeaders();
    int             beginWildcard(std::string name);
    int             endWildcard(std::string name);
    void            checkStartLine();
    void            checkHeaders();
    void            checkServerName();
    Server          checkServerName(std::vector<Server> &server);
    bool            checkHeaderHost();
    bool            methodAllowed();
    bool            RequestTargetTooLong();

public:

    void            printMpp();
    void            printHeaders();
    void            printBody();

    void            parseHeaders();
    void            parseBody();
    void            checkHost(std::string ip);
    Server          checkHost(int port, std::string ip, std::vector<Server> *server);
    bool            isEmpty();
    /** SETTERS **/
    void            setBoundary();
    void            setClength();
    //void            setKalive();
    void            setMaxSize(long unsigned size);
    void            setServer(std::vector<Server> *_servers);
    void            setString(const char *_buffer);
    void            setString(const std::string _buffer);
    void            setIp(const std::string ip);
    void            setPort(const int port);

    /** GETTERS **/

    bool                                                getMppBool() const;
    bool                                                getBodyBool() const;
    bool                                                getHeadersBool() const;
    bool                                                getKalive() const;
    bool                                                getStatus() const;
    bool                                                getClose() const;
    int                                                 getStatusCode() const;
    int                                                 getBodyParsing() const;
    long unsigned                                       getMaxSize() const;
    long long int                                       getClength() const;
    std::string                                         getBoundary() const;
    std::string                                         getMethod() const;
    std::string                                         getUri() const;
    std::string                                         getVersion() const;
    std::string                                         getPath() const;
	std::string											getDecodedPath() const;
	std::string 										getCleanPath() const;
	std::string                                         getQuery() const;
	std::string											getIp() const;
    std::string                                         getStatusMessage() const;
    std::string                                         getBody() const;
    std::vector<std::string>                            getMpp() const;
    std::map<std::string, std::vector<std::string> >    getHeaders() const;
    Server                                              getServerToUse() const;
};

#endif