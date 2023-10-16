#include <string>
#include <stdlib.h>

/**
* Decode a url encoded string
*/
std::string decode_url(const std::string & s)
{
	std::string	decoded = "";

	if (s.empty())
		return (decoded);
	for (size_t i = 0; i < s.length(); i++) {
		if (s[i] == '%') {
			if (s[i + 1]  && s[i + 2]) {
				decoded += std::strtol(s.substr(i + 1, 2).c_str(), NULL, 16);
				i += 2;
			}
		} else if (s[i] == '+') {
			decoded += ' ';
		} else {
			decoded += s[i];
		}
	}
	return (decoded);
}