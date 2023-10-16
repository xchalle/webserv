#include "../../include/httpRequest.hpp"


static const std::string getAllWS()
{
    std::string all_ws;
    all_ws = " \t";
    all_ws += (char)11;
    all_ws += (char)12;
    all_ws += (char)13;
    return (all_ws);
}


static const std::vector<std::string> getAllowedMethod()
{
    std::vector<std::string> _string;
    _string.push_back("GET");
    _string.push_back("POST");
    _string.push_back("DELETE");
    return _string;
}

void    HttpRequest::assignSplit()
{
    size_t pos = 0;
    while((pos = _string.find(CR, pos)) != std::string::npos)
    {
        if(pos + 1 < _string.size() &&  _string[pos + 1] != '\n')
            _string.replace(pos, 1, SP);
        ++pos;
    }
    while((pos = _string.find(OBS_FOLD, pos)) != std::string::npos)
    {
        _string.replace(pos, 3, SP);
    }
    while((pos = _string.find(OBS_FOLD2, pos)) != std::string::npos)
    {
        _string.replace(pos, 3, SP);
    }
    _split = split_string(_string, EOL);//change for split"\r\n"
}


/*void    HttpRequest::newRequest(std::string buffer, std::string ip, unsigned int port)
{
    _string.clear();
    _keeper.clear();
    potential.clear();
    _split.clear();
    _mpp.clear(); 
    _headers.clear();
    _body.clear();
    _chunked_body.clear();
    _boundary.clear();
    _ip = ip;
    _port = port;
    _clen = NOT_SET;
    _complete = false;
    _headers_bool = false;
    _mpp_bool = false;
    _body_bool = false;
    _string = buffer;
    status_code = 0;
    status_message.clear();
}*/


bool            HttpRequest::isEmpty()
{
    if (_port == NOT_SET)
        return true;
    return false;
}

void    HttpRequest::reset()
{
    _string.clear();
    _keeper.clear();
    potential.clear();
   _split.clear();
   _mpp.clear(); 
   _headers.clear();
   _body.clear();
   _ip.clear();
   _boundary.clear();
   _port = NOT_SET;
   _clen = NOT_SET;
   _headers_bool = false;
   _body_bool = false;
   _mpp_bool = false;
   _complete = false;
   _bad_request = false;
   _chunked = false;
   _close = false;
   _bdread = NOT_SET;
    status_code = 0;
    status_message.clear();
}


bool    HttpRequest::isComplete()
{
    if (_complete)
        return true;
    return false;
}


void    HttpRequest::printMpp()
{
    for (std::vector<std::string>::iterator it = _mpp.begin(); it != _mpp.end(); it++)
        std::cout << *it <<std::endl;
}


void    HttpRequest::printHeaders()
{
   for(std::map<std::string, std::vector<std::string> >::iterator it = _headers.begin(); it != _headers.end(); it++) 
   {
        std::cout << it->first << " -> ";
        for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
            std::cout<< "[" << *it2<< "]" <<std::endl;
   }
}


void    HttpRequest::printBody()
{
    std::cout << _body <<std::endl;
}


bool    HttpRequest::methodAllowed()
{
    std::vector<std::string> method_allowed = getAllowedMethod();
    std::vector<std::string>::iterator ite = method_allowed.end();
    for (std::vector<std::string>::iterator it = method_allowed.begin(); it != ite; it++)
    {
        if (!(it->compare(_mpp[0])))
            return true;
    }
    return false;
}

bool    HttpRequest::RequestTargetTooLong() //TODO
{
    return false;
}


void    HttpRequest::checkStartLine()
{
    if (!methodAllowed() /*|| !OWSinRequestTarget() */|| _mpp.size() != NB_ELEM_SL)
        return (setStatus(400, "Bad Request"));
            //   debug("400 BAD REQUEST", WARNING);
    if (RequestTargetTooLong())
        return (setStatus(400, "Bad Request"));
            //   debug("414 URI TOO LONG", WARNING);
    if (_mpp[2].compare(HTTP_VERSION))
    {
        return (setStatus(400, "Bad Request"));
            //    debug("400 BAD REQUEST", WARNING);
    }
}

bool HttpRequest::ImplementedEncoding(std::string to_cmp)
{
    std::vector<std::string> to_determine = split(to_cmp, ",");
    for (std::vector<std::string>::iterator it = to_determine.begin(); it != to_determine.end(); it++)
    {
        if (!(trim(*it, SP).compare("chunked")))
            _chunked = true;
        else
            return (false);
    }
    return (true);
}

bool    HttpRequest::checkHeaderHost()
{
    if ((_headers["Host"].empty()))
        return false;
    return true;
}

void    HttpRequest::checkHeaders()
{
    if (!checkHeaderHost())
        return (setStatus(400, "Bad Request"));

    const std::string all_ws = getAllWS();
    std::map<std::string, std::vector<std::string> >::iterator ite = _headers.end();
    for (std::map<std::string, std::vector<std::string> >::iterator it = _headers.begin(); it != ite; it++)
    {
        if (it->first.find_first_of(all_ws) != std::string::npos)
            return (setStatus(400, "Bad Request"));
            //debug("1 400 BAD REQUEST", WARNING);
        if (!(it->first.compare("Connection")))
        {
            std::vector<std::string> tmp = it->second;
            for(std::vector<std::string>::iterator it2 = tmp.begin(); it2 != tmp.end(); it2++)
            {
                if (it2->find("close") != std::string::npos)
                    _close = true;
            }
        }
        if (!(it->first.compare("Content-Length")))
        {
            if (it->second.front().find_first_not_of(DIGITS) != std::string::npos || it->second.front().empty())
                return (setStatus(400, "Bad Request"));
            //debug("2 400 BAD REQUEST", WARNING);
            _clen = atoll(it->second.front().c_str());
            if (_clen < 0 || _clen > LLONG_MAX)
                return (setStatus(400, "Bad Request"));
            // debug("3 400 BAD REQUEST", WARNING);
            if (_clen == LLONG_MAX && (it->second.front().compare(STR(LLONG_MAX))))
                return (setStatus(400, "Bad Request"));
            //  debug("4 400 BAD REQUEST", WARNING);
        }
        if (!(it->first.compare("Transfer-Encoding")))
        {
            if(!ImplementedEncoding(it->second.front()))
                return(setStatus(400, "Bad Request"));
            //   debug("5 501 NOT IMPLEMENTED", WARNING);
        }
    }
}

void    HttpRequest::DefineHTReadBody()
{
    if (_chunked)
    {
        if (_clen >= 0)
            _close = true;
        _bdread = 1;
    }
    else if (_clen >= 0 && !_chunked)
        _bdread = 2;
    else
    {
        _bdread = 0;
        _complete = true;
        if (!_keeper.empty())
            return (setStatus(411, "Length Required"));
    }
}

bool    HttpRequest::checkTransferEncoding()
{
    int to_read = 0;
    int i = 0;
    std::vector<std::string> tmp = split(_body, EOL);
    for(std::vector<std::string>::iterator it = tmp.begin(); it != tmp.end(); it++)
    {
        if (i % 2 == 0)
        {
            if((to_read = atoi_hex(*it)) == -1)
                return false;
        }
        else
        {
            if(it->size() != (unsigned long)to_read)
                return false;
        }
        i++;
    }
    return true;
}

bool    HttpRequest::checkTransferEncoding(size_t end)
{
    int to_read = 0;
    int i = 0;
    _body.erase(end);
    std::vector<std::string> tmp = split_first_occurence(_body, EOL);
    if (tmp.size()!= 2)
        return false;
    _body.clear();
    while(tmp.size() == 2)
    {
            if((to_read = atoi_hex(tmp[0])) == -1)
                return false;
            if(tmp[1].substr(to_read, strlen(EOL)) == EOL)
            {
                _body += tmp[1].substr(0, to_read);
                tmp[1] = tmp[1].substr(to_read + strlen(EOL));
            }
            else
                return false;
        i++;
        tmp = split_first_occurence(tmp[1], EOL);
    }
    return true;
}

void    HttpRequest::TransferEncodingParsing()
{
    size_t pos = 0;

    if((_keeper.size() + _string.size()) > _keeper.max_size())
        return (setStatus(413, "Request Entity Too Large"));
    else
        _body += _string;
    if (_body.size() >= strlen(END_OF_CHUNKED) && ((pos = _string.find(END_OF_CHUNKED)) != std::string::npos)/*!(_body.substr(_body.size() - strlen(END_OF_CHUNKED)).compare(END_OF_CHUNKED))*/)
    {
        if (checkTransferEncoding(pos + _body.size() - _string.size()))
            _complete = true;
        else
            return(setStatus(400, "Bad Request"));
    }
}

void    HttpRequest::LengthParsing()
{
    if((_keeper.size() + _string.size()) > _keeper.max_size())
        return (setStatus(413, "Request Entity Too Large"));
    else
        _body += _string;
    if ((long long)_body.size() >= _clen)
    {
        if ((long long)_body.size() > _clen)
            _close = true;
        _complete= true;
    }
}

void    HttpRequest::parseBody()
{
    if (_body.size() > _max_size)
        return (setStatus(413, "Request Entity Too Large"));
    if (_bdread == 1)
    {
        TransferEncodingParsing();
    }
    if (_bdread == 2)
        LengthParsing();
}

void    HttpRequest::parseHeaders()
{
    size_t pos;
    if((_keeper.size() + _string.size()) > _keeper.max_size())
        return (setStatus(413, "Request Entity Too Large"));
    else
        _keeper += _string;
    if ((pos =_keeper.find(END_OF_HEADERS)) == std::string::npos)
        return ;
    _string = _keeper.substr(0, pos);
    _keeper = _keeper.substr(pos + strlen(END_OF_HEADERS));
    assignSplit(); //obs-fold ' ' et '\t' DONE
    fillMpp(); 
    if(isComplete())
        return;
    fillHeaders(); // rm all OWS after : and after value
    if(isComplete())
        return;
    checkStartLine();
    if(isComplete())
        return;
    checkHeaders();
    if(isComplete())
        return;
    DefineHTReadBody();
    _string = _keeper;
    _keeper.clear();
}

void    HttpRequest::fillMpp()
{
    const std::string to_split = getAllWS();
    _mpp = split(_split[0], to_split); 
    if (_mpp.size() != NB_ELEM_SL)
        return (setStatus(400, "Bad Request"));
    _mpp_bool = true;
}

void    HttpRequest::fillBody(int pos)
{
    _body.append(_string.substr(pos));
    if ((_clen < 0 && _boundary.empty()) ||(_clen >= 0 && _clen <= (long int)_body.size()) || (!_boundary.empty() && (_body.size() > _boundary.size()) &&  _body.substr(_body.size() - std::strlen(_boundary.c_str())).compare(_boundary) == 0 /*_body.find(_boundary) != std::string::npos)*/))
    {
        if (_clen >= 0 && _clen <= (long int)_body.size())
            _body = _body.substr(0, _clen);
        if (_clen < 0 && _boundary.empty())
            _body.clear();
        _body_bool = true;
    }
    else if (_clen >= 0 && _clen < (long int)_body.size())
    {
        _bad_request = true;
    }
    else 
        _body_bool = true;
}


int    HttpRequest::fillHeaders()
{
    std::vector<std::string>    tmp;
    std::vector<std::string>    tmp2;
    int pos = (*_split.begin()).size() + 2;

    for (std::vector<std::string>::iterator it1 =_split.begin() + 1; it1 != _split.end(); it1++)
    {
        tmp = split_first_of(*it1, ":"); // split ";" and use struct Pair from Pair.hpp
        if (tmp.size() != 2 && !tmp.empty())
            setStatus(400, "Bad Request");
        if (!tmp.empty())
        {
            if((*it1).compare(EOL) == 0 /*|| *it.compare(boundary) */)
            {
                _headers_bool = true;
                pos +=2;
                break;
            }
            pos += (*it1).size() + 2;
            tmp2 = split(tmp[1], ";");
            for (std::vector<std::string>::iterator it = tmp2.begin(); it != tmp2.end(); it++)
                *it = trim(*it, OWS);
            if (tmp.size() > 1)
            {
                if ((!(tmp[0].compare("Host")) && _headers.find("Host") != _headers.end()) || (!(tmp[0].compare("Content-Length")) && _headers.find("Content-Length") != _headers.end()) || (!(tmp[0].compare("Transfer-Encoding")) && _headers.find("Transfer-Encoding") != _headers.end()))
                    setStatus(400, "Bad Request");
                _headers.insert(make_pair(tmp[0], tmp2));
            }
        }
    }
    setBoundary();
    return (pos);
}

Server     HttpRequest::checkHost(int port, std::string ip, std::vector<Server> *servers)
{
    std::vector<Server> potentials;
    for (std::vector<Server>::iterator it = servers->begin(); it != servers->end(); it++)
        if ((*it).getHost() == ip && (int)(*it).getPort() == port)
            potentials.push_back(*it);
    if (potentials.size() == 0 && ip != "0.0.0.0")
        return (checkHost(port, "0.0.0.0", servers));
    if (potentials.size() == 1)
        to_use = potentials[0];
    else if(!(_headers["Host"].empty()))
        to_use = checkServerName(potentials);
    else
        to_use = potentials[0];
    return (to_use);
}

Server    HttpRequest::checkServerName(std::vector<Server> &servers)
{
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        std::vector<std::string> server_name = (*it).getServerName();
        for (std::vector<std::string>::iterator it2 = server_name.begin(); it2 != server_name.end(); it2++)
        {
            if(it2->compare(_headers["Host"].front()) == 0)
            {
                to_use = *it;
                return (to_use);
            }
        }
    }
    int tmp = 0;
    int max = 0;
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        std::vector<std::string> server_name = (*it).getServerName();
        for (std::vector<std::string>::iterator it2 = server_name.begin(); it2 != server_name.end(); it2++)
        {
            if ((tmp = beginWildcard(*it2)) > max)
            {
                max = tmp;
                to_use = *it;
            }
        }
    }
    if (max != 0)
        return to_use;
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        std::vector<std::string> server_name = (*it).getServerName();
        for (std::vector<std::string>::iterator it2 = server_name.begin(); it2 != server_name.end(); it2++)
        {
            if ((tmp = endWildcard(*it2)) > max)
            {
                max = tmp;
                to_use = *it;
            }
        }
    }
    if (max == 0)
        to_use = servers[0];
    return to_use;
}

int    HttpRequest::beginWildcard(std::string name)
{
    std::string host = _headers["Host"].front();
    std::string tmp = host;
    std::size_t pos = 0;
    if ((name.size() >= 2 && name[0] == '*' && name[1] == '.'))
        while((pos = host.find(".", pos)) != std::string::npos)
        {
            tmp = host.substr(pos);
            if (tmp.compare(name.c_str() + 1) == 0)
                return (name.size());
            if ((++pos) >= host.size())
                return 0;
        }
    return 0;
}

int    HttpRequest::endWildcard(std::string name)
{
    std::string host = _headers["Host"].front();
    std::string tmp;
    std::size_t pos = host.size();
    if ((name.size() >= 2 && name[name.size() - 1] == '*' && name[name.size() - 2] == '.'))
        while((pos = host.rfind(".", pos)) != std::string::npos)
        {
            tmp = host.substr(0, pos);
            if (tmp.compare(name.substr(0, name.size() - 2)) == 0)
                return (name.size()); 
            if (pos == 0)
                return 0;
            pos--;
        }
    return 0;
}
