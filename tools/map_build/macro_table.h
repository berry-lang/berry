#ifndef __MACRO_TABLE_H
#define __MACRO_TABLE_H

#include <map>
#include <string>
#include <vector>

class macro_table {
public:
    macro_table() {}
    void scan_file(const std::string &filename);
    bool query(const std::string &str);
    std::map<std::string, int> table();

private:
    std::string readfile(const std::string &filename);
    int parse_value(std::string str);

private:
    std::map<std::string, int> m_map;
};

#endif
