#include <string.h>
#include "Resource.hpp"

LongResource::LongResource(const char * name, const char * res_str) : Resource(name, res_str) {
    amount = strtol(res_str, nullptr, 10);
}

LongResource::LongResource(const char * name, long res_long) : Resource(name) {
    res_str = res_long;
    amount = res_long;
}

long LongResource::getLongValue() const {
    return amount;
}
