#include "../../../include/loop.hpp"
#include "../../../include/HttpResponse.hpp"



Loop::Loop(std::vector<Server> *servs)
{
    servers = servs;
    updateTime = get_time();
}

Loop::~Loop()
{
}

/**
 * @brief close all still opened socket during the webserv
*/
void Loop::closeSocket()
{
    int fd;
    for(unsigned long i = 0; i < connections.size(); i++)
    {
        fd = connections[i].getFd();
        if (fd != NO_SOCKET)
            close(fd);
    }
    for(unsigned long i = 0; i < listen_sock.size(); i++)
    {
        fd = listen_sock[i].getFd();
        if (fd != NO_SOCKET)
            close(fd);
    }
    std::vector<addrinfo *>::iterator ite = vec_addrinfo.end();
    for(std::vector<addrinfo *>::iterator it = vec_addrinfo.begin(); it != ite; it++)
    {
        freeaddrinfo(*it);
    }
    for (size_t i = 0; (i < MAX_CLIENTS && i < connections.size()); i++)
        delete connections[i].response_ptr;
}

/**
 * @brief use to shutdown the server if an error occur
*/
int Loop::shutdown()
{
    closeSocket();
    localhosts.~map();
    listen_sock.~vector();
    connections.~vector();
    servers->~vector();
    banPerIp.~map();
    reqPerIp.~map();
    vec_addrinfo.~vector();
    return (EXIT_SUCCESS);
}

in_addr_t Loop::getAddr(std::string host)
{

    struct sockaddr_in *to_return = NULL;
    struct addrinfo sock_template, *sock_info;
    memset(&sock_template, 0, sizeof(sock_template));
    sock_template.ai_family = AF_INET;
    sock_template.ai_socktype = SOCK_STREAM;
    sock_template.ai_protocol = IPPROTO_TCP;
    if (getaddrinfo(host.c_str(), NULL, &sock_template, &sock_info) == 0)
    {
        if(sock_info->ai_addr->sa_family == AF_INET)
        {
            vec_addrinfo.push_back(sock_info);
            to_return = (sockaddr_in *)sock_info->ai_addr;
            return to_return->sin_addr.s_addr;
        }       
    }
    return 0;
}

bool Loop::verifServer()
{
    for (long unsigned int i = 0; i < servers->size(); i++)
    {
        for(long unsigned int j = i + 1; j < servers->size(); j++)
        {
            if (((*servers)[i]).getHost() == ((*servers)[j]).getHost() && ((*servers)[i]).getPort() == ((*servers)[j]).getPort())
            {
                std::vector<std::string> serv_i= ((*servers)[i]).getServerName(); 
                std::vector<std::string>::iterator it = serv_i.begin();
                std::vector<std::string>::iterator ite = serv_i.end();
                for(;it != ite; it++)
                {
                    std::vector<std::string> serv_j= ((*servers)[j]).getServerName(); 
                    std::vector<std::string>::iterator it2 = serv_j.begin();
                    std::vector<std::string>::iterator ite2 = serv_j.end();
                    for(; it2 != ite2; it2++)
                    {
                        if ((*it) == (*it2))
                        {
                            debug("webserv: error: conflicting server name " + (*it) + " on " + ((*servers)[i]).getHost() + ":" + to_string(((*servers)[i]).getPort()), ERROR);
                            return (false);
                        }
                    }
                }
            }
        }
    }
    return (true);
}

/**
 * @deprecated
 * @brief create a socket (if not bind yet) and fill an element of the socket vector
 * @param serv server to socketize
 * @param pos of the server in the vector
 * @return return 0 if everything is ok
 */
int Loop::startListenSocket(Server *serv, const int pos)
{
    std::string host = serv->getHost();
    int listen_sock;
    struct protoent *proto;
    struct sockaddr_in sin;
    int flags;
    int reuse = 1;

    proto = getprotobyname("tcp");

    if (proto == 0)
        return (-1);
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, proto->p_proto)) < 0)
    {
        debug("socket() failed for one of master server", ERROR);
        return (-1);
    }
    if (-1 == (flags = fcntl(listen_sock, F_GETFL, 0)))
        flags = 0;
    fcntl(listen_sock, F_SETFL, flags | O_NONBLOCK);
    if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0)
    {
        debug("setsocketopt() failed for one of master server", ERROR);
        close(listen_sock);
        return (-1);
    }
    memset(&sin, 0, sizeof(sin));
    if (findHostInVector(host))
        serv->setHost(host);
    sin.sin_family = AF_INET;
    if (!host.compare("0.0.0.0"))
        sin.sin_addr.s_addr = htonl(INADDR_ANY);
    else
    {
        getAddr(host);
        if ((sin.sin_addr.s_addr = getAddr(host)) == INADDR_NONE)
        {
            debug("host not found in \"" + host + ":" + to_string(serv->getPort()) + "\" of the \"listen\" directive", ERROR);
            close(listen_sock);
            return(-1);
        }
    }
    sin.sin_port = htons(serv->getPort());

    if (bind(listen_sock, (struct sockaddr *)&sin, sizeof(sin)) != 0)
    {
        if (!(sameBind(pos, ntohl(sin.sin_addr.s_addr))))
        {
            debug("bind() to " + serv->getHost() + ":" + to_string(serv->getPort()) + " failed", ERROR); 
            close(listen_sock);
            return (-1);
        }
    }

    if (listen(listen_sock, MAX_CLIENTS) != 0)
    {
        debug("listen on a master socket failed", ERROR);
        return (-1);
    }
    return (listen_sock);
}

bool Loop::initSocket()
{
    openHostsFile();
    for (long unsigned int i = 0; i < servers->size(); i++)
    {

        listen_sock.push_back(FdHandler(startListenSocket(&(*servers)[i], i), (*servers)[i].getPort(), (*servers)[i].getHost()));
        if (listen_sock.back().getFd() == -1)
        {
            debug("webserv: exit: error with a socket", ERROR);
            return (false);
           // exit(shutdown());
        }
    }
    save_sock = findMaxSock();
    FdHandler to_fill(NO_SOCKET, servers);
    for (int i = 0; i < MAX_CLIENTS; i++)
        connections.push_back(to_fill);
    time.tv_usec = 10;
    time.tv_sec = 0;
    return (true);
}

/**
 * @brief reset all fd_set structs for select() to work properly
 * @param connection_list reference on client object vector
 * @param listen_socket reference on server_socket vector
 */
int Loop::resetFdSet()
{
    long unsigned int i;

    FD_ZERO(&read_socks);
    for (i = 0; i < listen_sock.size(); i++)
        FD_SET(listen_sock[i].getFd(), &read_socks);
    for (i = 0; i < MAX_CLIENTS; i++)
        if (connections[i].getFd() != NO_SOCKET)
            FD_SET(connections[i].getFd(), &read_socks);

    FD_ZERO(&write_socks);
    for (i = 0; i < MAX_CLIENTS; i++)
        if (connections[i].getFd() != NO_SOCKET && connections[i].send_bit > 0 && connections[i].response.empty() == false)
            FD_SET(connections[i].getFd(), &write_socks);

    FD_ZERO(&except_socks);
    for (i = 0; i < listen_sock.size(); i++)
        FD_SET(listen_sock[i].getFd(), &except_socks);
    for (i = 0; i < MAX_CLIENTS; i++)
        if (connections[i].getFd() != NO_SOCKET)
            FD_SET(connections[i].getFd(), &except_socks);
    setupMaxSock();
    time.tv_usec = 7;
    time.tv_sec = 0;
    return (0);
}


void    Loop::timeoutChecker()
{
    long long int actualTime = get_time();

    if (actualTime - updateTime > CHECK_BAN_TIME)
    {
        updateTime = actualTime;
        for(std::map<in_addr_t, int>::iterator it = reqPerIp.begin(); it!= reqPerIp.end(); it++)
        {
            if (it->second > 0)
                it->second -= CHECK_BAN_TIME / 1000;
            if (it->second > MAX_REQ)
            {
                banPerIp[it->first] = get_time();
                it->second = 0;
            }

        }
    }
    for (long unsigned int i = 0; i < connections.size(); i++)
    {
        if (connections[i].getFd() != NO_SOCKET)
        {
            if (actualTime - connections[i].getTime() > TIMEOUT)
            {
                if( connections[i].request.isComplete())
                {
                    closeConnection(i);
                    return;
                }
                connections[i].setStatus(408, "Request Timeout");
            }
        }
    }
}

void Loop::muxingIostream()
{
    ret_slct = select(max_sock + 1, &read_socks, &write_socks, &except_socks,&time); // change timeval
    if (ret_slct == -1)
        exit(shutdown());
    else if (ret_slct == 0)
    {
        //silence is Golden...
    }
    else
    {
        connectionChecker();
        connectionHandler();
    }
    for (long unsigned int i = 0; i < connections.size(); i++)
        if (connections[i].getFd() != NO_SOCKET)
            responseHandler(&connections[i]);
    timeoutChecker();
}

/**
 * @brief accept connection on a listen_sock and create a socket for the connection with the client
 * @param server_sock server socket asked by the client to be connected with
 * @return an error if something went wrong
 */
int Loop::acceptNewConnection(int server_sock, const int _port, const std::string _ip)
{
    unsigned int addr_size = sizeof(struct sockaddr_in);
    int client_sock;
    struct sockaddr_in client_addr;
    int flags;

    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, (socklen_t *)&addr_size);
    if (client_sock == -1)
        return (client_sock);
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (connections[i].getFd() == NO_SOCKET)
        {
            if (-1 == (flags = fcntl(client_sock, F_GETFL, 0)))
                flags = 0;
            fcntl(client_sock, F_SETFL, flags | O_NONBLOCK);
            connections[i].setFd(client_sock);
            connections[i].setPort(_port);
            connections[i].setIp(_ip);
            connections[i].setClientIp(client_addr.sin_addr.s_addr);
            connections[i].setTime();
            return (client_sock);
        }
    }
    debug("webserv: maximum clients connected", WARNING);
    close(client_sock);
    return (client_sock);
}

void Loop::connectionChecker()
{
    for (long unsigned int i = 0; i < listen_sock.size(); i++)
    {
        if (FD_ISSET(listen_sock[i].getFd(), &read_socks))
        {
            if (acceptNewConnection(listen_sock[i].getFd(), listen_sock[i].getPort(), listen_sock[i].getIp()) == -1)
                debug("webserv: trouble accepting incoming connection", ERROR);
        }
        if (FD_ISSET(listen_sock[i].getFd(), &except_socks))
            debug("exception on a master socket ", WARNING);
    }
}

int Loop::handleRequest(FdHandler *client)
{
    std::vector<char> chargetter(BUFFER_SIZE);
    std::string buffer;
    if (!checkNumReqRedir(client))
        return (0);
    ret_recv = recv(client->getFd(), &chargetter[0], BUFFER_SIZE, 0);
    if(!checkRecv())
        return (-1);
    if (!(chargetter.size() < buffer.max_size())) 
    {
        client->setStatus(500, "Internal Server Error");
        return (0);
    }
    buffer.append(chargetter.begin(), chargetter.begin() + ret_recv);
    client->fill(buffer, this->servers);
    return (0);
}

void Loop::responseHandler(FdHandler *client)
{
    if (!(client->request.isComplete()) && client->response_ptr == NULL)
        return;
    if (client->request.isComplete() && client->response_ptr == NULL)
    {
        client->request.checkHost(client->getPort() ,client->getIp(), servers);

        if (!isIpBanned(client))
            reqPerIp[client->getClientIp()] += 1;
        client->to_use.push_back(client->request.getServerToUse());
        client->response_ptr = new HttpResponse(client->request, client->to_use.front());
        if (client->request.getStatusCode() != 0 && !(client->request.getStatusMessage().empty()))
            client->response_ptr->setStatus(client->request.getStatusCode(), client->request.getStatusMessage());
        client->response = client->response_ptr->getResponse();
        if (client->response_ptr->_is_long_response == false)
        {
	        client->send_bit = client->response.size();
            checkRedirect(client);
            return ;
        }
    }
    if (client->request.isComplete() && client->response_ptr->_is_long_response == false)
        return;
    if (client->request.isComplete() && client->response_ptr->_is_long_response == true)
    {
		client->response_ptr->setLongResponse();
    }
    if (client->request.isComplete() && client->response_ptr->_is_long_response == false)
    {
		client->response = client->response_ptr->getLongResponse();
	    client->send_bit = client->response.size();
        checkRedirect(client);
    }
}

void Loop::sendResponse(int pos)
{
    if (connections[pos].response_ptr->_is_long_response == false)
    {
        ret_send = 0;
        ret_send = send(connections[pos].getFd(), &(connections[pos].response)[0], connections[pos].send_bit, 0);
        if (ret_send == -1 || ret_send == 0)
            closeConnection(pos);
        connections[pos].send_bit -= ret_send;
        connections[pos].response.erase(connections[pos].response.begin(), connections[pos].response.begin()+ ret_send);
        if (connections[pos].send_bit == 0)
        {
            if (connections[pos].toClose())
                closeConnection(pos);
            else
                resetConnection(pos);
        }
    }
}

void Loop::connectionHandler()
{
    for (long unsigned int i = 0; i < connections.size(); i++)
    {
        if (connections[i].getFd() != NO_SOCKET && FD_ISSET(connections[i].getFd(), &except_socks))
        {
            debug("error on socket " + to_string(i) + "\nclosing socket" + to_string(i), ERROR);
            closeConnection(i);
        }
        else if (connections[i].getFd() != NO_SOCKET && FD_ISSET(connections[i].getFd(), &read_socks) && (!(connections[i].request.isComplete())))
        {
            if ((handleRequest(&connections[i])) < 0)
                closeConnection(i);
        }
        else if (connections[i].getFd() != NO_SOCKET && FD_ISSET(connections[i].getFd(), &write_socks))
            sendResponse(i);
    }
}