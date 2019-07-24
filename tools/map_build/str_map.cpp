#include "str_map.h"
#include <fstream>
#include <sstream>
#include <regex>

void str_map::parse_text(const std::string &filename, const std::string &text)
{
    if (vaild_filename(filename)) {
        std::regex reg("\\bbe_const_str_(\\w*)\\b");
        std::sregex_iterator it(text.begin(), text.end(), reg);
        std::sregex_iterator end;
        while (it != end) {
            m_map[it->str(1)] = 0;
            ++it;
        }
    }
}

bool str_map::vaild_filename(const std::string &filename)
{
    return filename != "be_const_strtab_def.h" && filename != "be_const_strtab.h";
}
