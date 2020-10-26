#include <vector>
#include <string>
#include "Resource.hpp"
#include "ResourceDef.hpp"
#include "ResourceType.hpp"


int main() {

    resdefs.push_back(std::make_shared<ResourceDef>("ncpus", ResourceType::rt_long, 1));
    resdefs.push_back(std::make_shared<ResourceDef>("vnode", ResourceType::rt_string, 1));
    resdefs.push_back(std::make_shared<ResourceDef>("colors", ResourceType::rt_stringArray, 1));

    for (const auto &resdef : resdefs) {
        printf("Name [%s] | Type [%d]\n", resdef->getName().c_str(), static_cast<int>(resdef->getType()));
    }

    // a job's requested resources
    std::vector<std::shared_ptr<Resource>> res;
    res.push_back(std::make_shared<LongResource>("ncpus", "30"));
    res.push_back(std::make_shared<StringResource>("vnode", "shecil"));
    res.push_back(std::make_shared<StringArrayResource>("colors", "red,green,blue"));

    for (const auto &resreq : res) {
        switch(resreq->getType()) {
            case ResourceType::rt_long:
            {
                auto long_res = static_cast<LongResource *>(resreq.get());
                printf("Name [%s] | Type [%d] | Value [%d]\n", long_res->getName().c_str(), long_res->getType(), long_res->getLongValue());
                break;
            }
            case ResourceType::rt_string:
            {
                auto string_res = static_cast<StringResource *>(resreq.get());
                printf("Name [%s] | Type [%d] | Value [%s]\n", string_res->getName().c_str(), string_res->getType(), string_res->getStringValue().c_str());
                break;
            }
            case ResourceType::rt_stringArray:
            {
                auto strarr_res = static_cast<StringArrayResource *>(resreq.get());
                printf("Name [%s] | Type [%d] | Values ", strarr_res->getName().c_str(), strarr_res->getType());
                auto strarr = strarr_res->getStringArrayValue();
                for (auto it = strarr.begin(); it != strarr.end(); ++it) {
                    printf("[%s] ", it->c_str());
                }
                printf("\n");
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