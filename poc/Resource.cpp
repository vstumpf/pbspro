#include <string>

#include "Resource.hpp"

Resource::Resource(const char * name, const char * res_str) {
    bool found = false;
    for (const auto &rd : resdefs) {
        if (rd->getName() == name) { //yes, you CAN do this!
            resdef = std::weak_ptr<ResourceDef>(rd);
            found = true;
            break;
        }
    }
    if (!found) {
        throw;
    }
    this->res_str = res_str;
}


Resource::Resource(const char * name) : Resource(name, "") {
}

// Resource::Resource(std::string name, const char * res_str) {
// }

// Resource::Resource(ResourceDef& resdef, const char * res_str) {
// }

ResourceType Resource::getType() {
    if (auto shared = resdef.lock())
        return shared->getType();
    else
        return ResourceType::rescTypeError;
}

std::string Resource::getName() {
    if (auto shared = resdef.lock())
        return shared->getName();
    else
        return nullptr;
        // raise instead of returning nullptr
}
