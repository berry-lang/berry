#include "hashcode.h"

namespace string {

uint32_t hashcode(const std::string &string)
{
    size_t len = string.size();
    const char *str = string.data();
    uint32_t hash = 2166136261u;
    while (len--) {
        hash = (hash ^ (unsigned char)*str++) * 16777619u;
    }
    return hash;
}

}
