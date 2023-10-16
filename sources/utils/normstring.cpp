#include <vector>
#include <string>


std::string normstring(std::string _to_norm)
{
    if (_to_norm[0] == ':')
        _to_norm.erase(0, 1);
    while(_to_norm[0] == ' ' || _to_norm[0] == '\t' )
        _to_norm.erase(0, 1);
    while(_to_norm[_to_norm.size() - 1] == ' ' || _to_norm[_to_norm.size() - 1] == '\t' )
        _to_norm.erase(_to_norm.size() - 1, 1);
    return(_to_norm);
}