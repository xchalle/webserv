#include "../../../include/loop.hpp"
#include "../../../include/HttpResponse.hpp"




/**
 * @brief fill locahost vector with the /etc/hosts file
 *
 */
int Loop::fillLocalhosts(std::string line)
{
    std::vector<std::string> potential = split_quotes_tab(line);
    if (potential.size() < 2)
        return (0);
    if ((getAddr(potential[0])) != INADDR_NONE)
    {
        localhosts.insert(make_pair(potential[0], potential[1]));
    }
    return (0);
}

void Loop::openHostsFile()
{
    std::ifstream hosts_file(LINUX_HOST);

    if (!hosts_file.is_open())
    {
        debug("Can't open Linux Hosts file", WARNING);
        return;
    }
    while (hosts_file.good())
    {
        std::string line;
        std::getline(hosts_file, line);
        fillLocalhosts(line);
    }
    hosts_file.close();
}

/**
 * @brief check if the config file Host is accepted
 * @param host to check
 *
 */
bool Loop::findHostInVector(std::string &host)
{
    for (std::map<std::string, std::string>::iterator it = localhosts.begin(); it != localhosts.end(); it++)
    {
        if (host.compare(it->second) == 0)
        {
            host = it->first;
            return (true);
        }
    }
    return (false);
}

/**
 * @brief find the higher fd(int) in the vector of socket
 * usefull for select() param nfds(number of fd)
 * @param listen_socket reference of the vector of socket
 * @return the higher fd of the vector
 */
int Loop::findMaxSock()
{
    int max = 0;
    for (size_t i = 0; i < listen_sock.size(); i++)
    {
        if (listen_sock[i].getFd() > max)
            max = listen_sock[i].getFd();
    }
    return (max);
}

bool Loop::sameHost(const int i, const int pos, const uint32_t ip)
{
    if ((*servers)[i].getHost() == (*servers)[pos].getHost())
        return true;
    if ((*servers)[i].getHost() == "0.0.0.0" && ip >= MIN_IP && ip <= MAX_IP)
        return true;
    uint32_t tmp = ntohl(getAddr((*servers)[i].getHost()));
    if ((*servers)[pos].getHost() == "0.0.0.0" && tmp >= MIN_IP && tmp <= MAX_IP)
        return (true);
    return false;
}

bool Loop::samePort(const int i, const int pos)
{
    if (((*servers)[i].getPort() == (*servers)[pos].getPort()))
        return true;
    return false;
}

bool Loop::sameBind(const int pos, const uint32_t ip)
{
    for (int i = 0; i != pos; i++)
    {
        if (samePort(i, pos) && sameHost(i, pos, ip))
            return (true);
    }
    return (false);
}

bool Loop::isIPAddress(const std::string &host)
{
    if (host.find_first_not_of(IP_NUMBER) != std::string::npos)
        return false;
    if (getAddr(host) == INADDR_NONE)
        return false;
    return true;
}

int Loop::getPos(const int fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (connections[i].getFd() == fd)
            return (i);
    return (-1);
}

/**
 * @brief insert the highest fd in max_sock for select() param1
 */
void Loop::setupMaxSock()
{
    max_sock = save_sock;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (connections[i].getFd() > max_sock)
            max_sock = connections[i].getFd();
}

void Loop::checkRedirect(FdHandler *client)
{
    if (client->response_ptr->getStatusCode() >= 300 && client->response_ptr->getStatusCode() <= 308)
        client->straightredirect++;
    else
        client->straightredirect = 0;
}

bool Loop::isIpBanned(FdHandler *client)
{
    if (banPerIp[client->getClientIp()] + BAN_TIME > get_time())
        return true;
    return false;
}
bool Loop::isTooManyRedirect(FdHandler *client)
{
    if (client->straightredirect >= MAX_STRAIGHT_REDIRECT)
        return true;
    return false;
}

bool Loop::checkNumReqRedir(FdHandler *client)
{
    if (isIpBanned(client))
    {
        client->setStatus(429, "Too Many Request");
        return (false);
    }
    if (isTooManyRedirect(client))
    {
        client->setStatus(310, "Too Many Redirects");
        return (false);
    }
    return (true);
}

bool Loop::checkRecv()
{
    if (ret_recv == -1)
    {
        debug("recv: failed to read on working socket", ERROR);
        return (false);
    }
    if (ret_recv == 0)
    {
        debug("recv: ended on working socket", WARNING);
        return (false);
    }
    return (true);
}

/**
 * @brief close connection with the client and reset the usefull variable
 * @param pos position on the vector of the reseted element
 * @return return 0 if everything goes well
 */
int Loop::closeConnection(const int pos)
{
    FD_CLR(connections[pos].getFd(), &write_socks);
    close(connections[pos].getFd());
    connections[pos].setFd(NO_SOCKET);
    connections[pos].setPort(NO_SOCKET);
    connections[pos].setIp("");
    connections[pos].response.clear();
    connections[pos].send_bit = -1;
    connections[pos].recv_bit = -1;
    connections[pos].nb_request = 0;
    connections[pos].straightredirect = 0;
    delete connections[pos].response_ptr;
    connections[pos].response_ptr = NULL;
    connections[pos].resetRequest();
    connections[pos].to_use.clear();
    return (0);
}

int Loop::closeConnection(FdHandler *client)
{
    FD_CLR(client->getFd(), &write_socks);
    close(client->getFd());
    client->setFd(NO_SOCKET);
    client->setPort(NO_SOCKET);
    client->setIp("");
    client->response.clear();
    client->send_bit = -1;
    client->recv_bit = -1;
    client->nb_request = 0;
    client->straightredirect = 0;
    delete client->response_ptr;
    client->response_ptr = NULL;
    client->to_use.clear();
    client->resetRequest();
    return (0);
}

/**
 * @brief resest connection with the client and reset the usefull variable
 * @param pos position on the vector of the reseted element
 * @return return 0 if everything goes well
 */
int Loop::resetConnection(const int pos)
{
    FD_CLR(connections[pos].getFd(), &write_socks);
    connections[pos].resetRequest();
    connections[pos].response.clear();
    connections[pos].to_use.clear();
    connections[pos].send_bit = -1;
    connections[pos].recv_bit = -1;
    delete connections[pos].response_ptr;
    connections[pos].response_ptr = NULL;
    connections[pos].resetRequest();
    return (0);
}