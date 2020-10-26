#include <string>

#include "ResourceType.hpp"

#ifndef RESOURCE_DEF_HPP
#define RESOURCE_DEF_HPP

class ResourceDef;

// this would be in sinfo
extern std::vector<std::shared_ptr<ResourceDef>> resdefs;


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

#endif // RESOURCE_DEF_HPP