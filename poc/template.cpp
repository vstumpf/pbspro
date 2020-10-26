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


std::vector<std::shared_ptr<ResourceDef>> resdefs;

class ResourceReq {
public:
    ResourceReq(const char *name, const char *res_str);
    std::string getName();
    ResourceType getType();

protected:
    char *res_str;
    std::weak_ptr<ResourceDef> resdef;
};

ResourceReq::ResourceReq(const char * name, const char * res_str) {
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
    this->res_str = strdup(res_str);
}

ResourceType ResourceReq::getType() {
    if (auto shared = resdef.lock())
        return shared->getType();
    else
        return ResourceType::rescTypeError;

}

std::string ResourceReq::getName() {
    if (auto shared = resdef.lock())
        return shared->getName();
    else
        return nullptr;
}

class StringResourceReq : public ResourceReq {
public:
    char * getStringValue() const;
};

char * StringResourceReq::getStringValue() const {
    return strdup(res_str);
}

class LongResourceReq : public ResourceReq {
public:
    LongResourceReq(const char * name, const char * res_str);
    long getLongValue() const;
private:
    long amount;
};

LongResourceReq::LongResourceReq(const char * name, const char * res_str) : ResourceReq(name, res_str) {
    amount = strtol(res_str, nullptr, 10);
}

long LongResourceReq::getLongValue() const {
    return amount;
}




int main() {
    resdefs.push_back(std::make_shared<ResourceDef>("ncpus", ResourceType::rescTypeLong, 1));
    resdefs.push_back(std::make_shared<ResourceDef>("vnode", ResourceType::rescTypeString, 1));

    for (const auto &resdef : resdefs) {
        printf("Name [%s] | Type [%d]\n", resdef->getName().c_str(), static_cast<int>(resdef->getType()));
    }

    // a job's requested resources
    std::vector<std::shared_ptr<ResourceReq>> resreqs;
    resreqs.push_back(std::make_shared<LongResourceReq>("ncpus", "30"));
    resreqs.push_back(std::make_shared<LongResourceReq>("vnode", "shecil"));

    for (const auto &resreq : resreqs) {
        switch(resreq->getType()) {
            case ResourceType::rescTypeLong:
            {
                auto long_resreq = static_cast<LongResourceReq *>(resreq.get());
                printf("Name [%s] | Type [%d] | Value [%d]\n", long_resreq->getName().c_str(), long_resreq->getType(), long_resreq->getLongValue());
                break;
            }
            case ResourceType::rescTypeString:
            {
                auto string_resreq = static_cast<StringResourceReq *>(resreq.get());
                printf("Name [%s] | Type [%d] | Value [%s]\n", string_resreq->getName().c_str(), string_resreq->getType(), string_resreq->getStringValue());
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