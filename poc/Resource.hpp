#include <memory>
#include <string>
#include <vector>
#include "ResourceType.hpp"
#include "ResourceDef.hpp"

#ifndef RESOURCE_HPP
#define RESOURCE_HPP



class Resource {
public:
    Resource(const char *name, const char *res_str);
    // Resource(std::string name, const char * res_str);
    // Resource(ResourceDef& resdef, const char * res_str);
    // virtual ~Resource();
    Resource(const char *name);
    std::string getName();
    ResourceType getType();

protected:
    std::string res_str;
    std::weak_ptr<ResourceDef> resdef;
};

class StringResource : public Resource {
public:
    StringResource(const char * name, const char * res_str);
    const std::string& getStringValue() const;
};

class LongResource : public Resource {
public:
    LongResource(const char * name, const char * res_str);
    LongResource(const char * name, long res_long);
    long getLongValue() const;
private:
    long amount;
};


#endif // RESOURCE_HPP

