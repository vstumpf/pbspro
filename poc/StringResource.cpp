#include <string>
#include "Resource.hpp"

StringResource::StringResource(const char *name, const char *res_str) : Resource(name, res_str) {

}

const std::string& StringResource::getValue() const {
    return res_str;
}
