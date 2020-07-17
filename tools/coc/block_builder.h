#ifndef __BLOCK_BUILDER_H
#define __BLOCK_BUILDER_H

#include <map>
#include <vector>
#include <string>

class macro_table;

class block_builder {
public:
    block_builder(const macro_table *macro, const std::string &path = ".");
    void parse_block(const std::string &str);
    std::string str();

private:
    struct block {
        std::string type;
        std::string name;
        std::map<std::string, std::string> attr;
        std::map<std::string, std::string> data;
    };

    std::map<std::string, std::string> parse_body(const std::string &str);
    std::map<std::string, std::string> parse_attr(const std::string &str);
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
    bool block_depend(const block &block);
    std::string query_item(const std::string &str);

private:
    std::vector<block> m_block;
    std::string m_outpath;
    const macro_table *m_macro;
};

#endif // !__BLOCK_BUILDER_H
