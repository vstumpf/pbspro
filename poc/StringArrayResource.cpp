#include <string>
#include <vector>
#include "Resource.hpp"


StringArrayResource::StringArrayResource(const char * name, const char * res_str) : Resource(name, res_str) {
    std::string tok = res_str;
    std::string delim = ",";
    size_t start = 0;
    size_t end = tok.find(delim);

    while (end != std::string::npos) {
        strarr.push_back(tok.substr(start, end - start));
        start = end + delim.length();
        end = tok.find(delim, start);
    }
    strarr.push_back(tok.substr(start, end));
}

const std::vector<std::string>&
StringArrayResource::getValue() const {
    return strarr;
}