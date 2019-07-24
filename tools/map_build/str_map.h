#ifndef __STR_MAP_H
#define __STR_MAP_H

#include <string>
#include <map>

class str_map {
public:
    str_map() {}
    void parse_text(const std::string &filename, const std::string &text);
    std::map<std::string, int> data() { return m_map; }

    static bool vaild_filename(const std::string &filename);

private:
    std::map<std::string, int> m_map;
};

#endif
