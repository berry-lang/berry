#include "macro_table.h"
#include <regex>
#include <fstream>
#include <sstream>

std::string macro_table::readfile(const std::string &filename)
{
    std::ifstream in(filename);
    std::ostringstream tmp;
    tmp << in.rdbuf();
    return tmp.str();
}

int macro_table::parse_value(std::string str)
{
    if (!str.length()) {
        return 1; /* defined a macro name but no content, considered true */
    }
    if (!(str[0] >= '0' && str[0] <= '9')) {
        return 1;
    }
    return atoi(str.c_str());
}

void macro_table::scan_file(const std::string &filename)
{
    std::string str(readfile(filename));
    std::regex reg("^\\s*#define\\s+(\\w+)([ \t]+(.+)?(//|/*)?|\\n)");
    std::sregex_iterator it(str.begin(), str.end(), reg);
    std::sregex_iterator end;
    while (it != end) {
        m_map[it->str(1)] = parse_value(it->str(3));
        ++it;
    }
}

bool macro_table::query(const std::string &str)
{
    std::regex reg("(!?)(\\w+)");
    std::match_results<std::string::const_iterator> res;
    if (regex_match(str, res, reg)) {
        int value = m_map[str];
        return res[1] == "!" ? value == 0 : value != 0;
    }
    return 0;
}

std::map<std::string, int> macro_table::table()
{
    return m_map;
}
