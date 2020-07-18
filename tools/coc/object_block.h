#ifndef __OBJECT_BLOCK_H
#define __OBJECT_BLOCK_H

#include <map>
#include <string>

struct object_block {
    struct data_value {
        std::string value;
        std::string depend;
    };
    std::string type;
    std::string name;
    std::map<std::string, std::string> attr;
    std::map<std::string, data_value> data;
};

#endif
