#ifndef __BLOCK_BUILDER_H
#define __BLOCK_BUILDER_H

#include <map>
#include <vector>
#include <string>
#include "object_block.h"

class macro_table;
class object_block;

class block_builder {
public:
    block_builder(const object_block *object, const macro_table *macro);
    std::string str();
    void dumpfile(const std::string &path);
    const std::vector<std::string>& strtab() const;

private:
    struct block {
        std::string type;
        std::string name;
        std::map<std::string, std::string> attr;
        std::map<std::string, std::string> data;
    };

    std::string block_tostring(const block &block);
    std::string class_tostring(const block &block);
    std::string vartab_tostring(const block &block);
    std::string module_tostring(const block &block);
    std::string map_tostring(const block &block, const std::string &name, bool local = false);
    void writefile(const std::string &filename, const std::string &text);
    std::string scope(const block &block);
    std::string super(const block &block);
    std::string name(const block &block);
    std::string init(const block &block);

private:
    block m_block;
    std::vector<std::string> m_strtab;
};

#endif // !__BLOCK_BUILDER_H
