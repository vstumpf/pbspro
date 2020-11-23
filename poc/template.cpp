#include <vector>
#include <stdio.h>
#include <memory>
#include <string>
#include <string.h>

// Forwarded from C++14
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}


enum class ResourceType {
    rescTypeError = -1,
    rescTypeString,
    rescTypeLong
};

class ResourceDef
{
public:
    ResourceDef(const char * n, ResourceType t, int f);
    std::string getName() const;
    ResourceType getType() const;

private:
	std::string name;			/* name of resource */
	ResourceType type;	/* resource type */
	unsigned int flags;		/* resource flags (see pbs_ifl.h) */
};

ResourceDef::ResourceDef(const char * n, ResourceType t, int f) : name(n), type(t), flags(f) {
}

std::string ResourceDef::getName() const {
    return name;
}

ResourceType ResourceDef::getType() const {
    return type;
}


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

class StringResource : public Resource {
public:
    StringResource(const char * name, const char * res_str);
    const std::string& getValue() const;
};

StringResource::StringResource(const char *name, const char *res_str) : Resource(name, res_str) {

}

const std::string& StringResource::getValue() const {
    return res_str;
}

class LongResource : public Resource {
public:
    LongResource(const char * name, const char * res_str);
    LongResource(const char * name, long res_long);
    long getValue() const;
private:
    long amount;
};

LongResource::LongResource(const char * name, const char * res_str) : Resource(name, res_str) {
    amount = strtol(res_str, nullptr, 10);
}

LongResource::LongResource(const char * name, long res_long) : Resource(name) {
    res_str = res_long;
    amount = res_long;
}

long LongResource::getValue() const {
    return amount;
}

class StringArrayResource : public Resource {
public:
    StringArrayResource(const char * name, const char * res_str);
    StringArrayResource(std::string& name, const char * res_str);

private:
    std::vector<std::string> strarr;
};

StringArrayResource::StringArrayResource(const char * name, const char * res_str) : StringArrayResource(std::string(name), res_str) {
}

StringArrayResource::StringArrayResource(std::string& name, const char * res_str) : Resource(name, res_str) {
    std::string tok;
    std::string delim = ",";
    size_t pos = 0;
    size_t end = name.find(delim);

    while (end != std::string::npos) {
        strarr.push_back(name.substr(pos, end-pos));
        start = end + delim.length();
        end = name.find(delim, start);
    }

    strarr.push_back(start, end);
}



int main() {
    resdefs.push_back(std::make_shared<ResourceDef>("ncpus", ResourceType::rescTypeLong, 1));
    resdefs.push_back(std::make_shared<ResourceDef>("vnode", ResourceType::rescTypeString, 1));

    for (const auto &resdef : resdefs) {
        printf("Name [%s] | Type [%d]\n", resdef->getName().c_str(), static_cast<int>(resdef->getType()));
    }

    // a job's requested resources
    std::vector<std::shared_ptr<Resource>> resreqs;
    resreqs.push_back(std::make_shared<LongResource>("ncpus", "30"));
    resreqs.push_back(std::make_shared<StringResource>("vnode", "shecil"));

    for (const auto &resreq : resreqs) {
        switch(resreq->getType()) {
            case ResourceType::rescTypeLong:
            {
                auto long_resreq = static_cast<LongResource *>(resreq.get());
                printf("Name [%s] | Type [%d] | Value [%d]\n", long_resreq->getName().c_str(), long_resreq->getType(), long_resreq->getValue());
                break;
            }
            case ResourceType::rescTypeString:
            {
                auto string_resreq = static_cast<StringResource *>(resreq.get());
                printf("Name [%s] | Type [%d] | Value [%s]\n", string_resreq->getName().c_str(), string_resreq->getType(), string_resreq->getValue().c_str());
                break;
            }
        }
    }


    // rescs.push_back(new Resource<int>());
    // rescs.push_back(new Resource<char *>());
    // rescs.push_back(new Resource<double>());

    // printf("%s\n", rescs[0]->getName());
    // printf("%s\n", rescs[1]->getName());
    // printf("%s\n", rescs[2]->getName());

    // resc.getSize();
    // resc.getValue<Size>();
    // resc.getValue<int>();
}






#if 0


class IResource {
public:
    virtual char * getName() = 0;
};

template <typename T>
class Resource : public IResource{
public:
    char * name;
    T value;
    char * getName() override;
};

template <typename T>
char * Resource<T>::getName() {
    return "any";
}

template <>
char * Resource<char *>::getName() {
    return "char *";
}

template <>
char * Resource<double>::getName() {
    return "double";
}


int main() {
    std::vector<IResource *> rescs;

    rescs.push_back(new Resource<int>());
    rescs.push_back(new Resource<char *>());
    rescs.push_back(new Resource<double>());

    printf("%s\n", rescs[0]->getName());
    printf("%s\n", rescs[1]->getName());
    printf("%s\n", rescs[2]->getName());

    // resc.getSize();
    // resc.getValue<Size>();
    // resc.getValue<int>();
}


#endif