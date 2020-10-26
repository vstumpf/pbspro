#include <vector>
#include <string>
#include "Resource.hpp"
#include "ResourceDef.hpp"
#include "ResourceType.hpp"


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
                printf("Name [%s] | Type [%d] | Value [%d]\n", long_resreq->getName().c_str(), long_resreq->getType(), long_resreq->getLongValue());
                break;
            }
            case ResourceType::rescTypeString:
            {
                auto string_resreq = static_cast<StringResource *>(resreq.get());
                printf("Name [%s] | Type [%d] | Value [%s]\n", string_resreq->getName().c_str(), string_resreq->getType(), string_resreq->getStringValue().c_str());
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