#include <string>
#include <vector>
#include <memory>

#include "ResourceDef.hpp"

#include "ResourceType.hpp"


// this would be in sinfo
std::vector<std::shared_ptr<ResourceDef>> resdefs;


ResourceDef::ResourceDef(const char * n, ResourceType t, int f) : name(n), type(t), flags(f) {
}

std::string ResourceDef::getName() const {
    return name;
}

ResourceType ResourceDef::getType() const {
    return type;
}
