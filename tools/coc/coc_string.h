#ifndef __COC_STRING_H
#define __COC_STRING_H

#include <string>

namespace coc {
    uint32_t hashcode(const std::string &string);
    std::string escape_operator(const std::string &string);
}

#endif
