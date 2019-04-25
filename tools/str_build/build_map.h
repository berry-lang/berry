#ifndef __BUILD_MAP_H
#define __BUILD_MAP_H

#include <map>
#include <string>
#include <vector>

class build_map
{
public:
    build_map(std::map<std::string, int> map);
    ~build_map();
    void build();

private:
    uint32_t hashcode(const std::string &string);
    void make_ceil(const std::string &string);
    std::string escdot(const std::string &string);
    std::string build_table_def();
    std::string build_table_ext();
    void writefile(const std::string &filename, const std::string &text);

private:
    struct str_info {
        uint32_t hash;
        std::string str;
    };
    size_t m_count;
    std::vector<std::vector<str_info>> m_hashtable;
};

#endif
