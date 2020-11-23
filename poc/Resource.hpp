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

    // template<typename T> T getValue();
    const std::string& getStringValue() const;
    std::string getValue();

protected:
    std::string res_str;
    std::weak_ptr<ResourceDef> resdef;
};

class StringResource : public Resource {
public:
    StringResource(const char * name, const char * res_str);
    const std::string& getValue() const;
};

class LongResource : public Resource {
public:
    LongResource(const char * name, const char * res_str);
    LongResource(const char * name, long res_long);
    long getValue() const;
    // const long getLongValue() const;
private:
    long amount;
};

class StringArrayResource : public Resource {
public:
    StringArrayResource(const char * name, const char * res_str);
    StringArrayResource(std::string& name, const char * res_str);

    const std::vector<std::string>& getValue() const;

private:
    std::vector<std::string> strarr;
};

class FloatResource : public Resource {
public:
    FloatResource(std::string& name, const char * res_str);
    FloatResource(std::string& name, double res_double);

    double getValue() const;

private:
    double amount;

};

#endif // RESOURCE_HPP

