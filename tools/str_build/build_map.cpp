#include "build_map.h"
#include <iostream>
#include <sstream>
#include <fstream>

build_map::build_map(std::map<std::string, int> map)
{
    size_t size = map.size() / 2;
    m_count = map.size();
    m_hashtable.resize(size < 4 ? 4 : size);
    for (auto it : map) {
        make_ceil(it.first);
    }
    build();
}

build_map::~build_map()
{
}

void build_map::build()
{
    writefile("generate/be_const_strtab_def.h", build_table_def());
    writefile("generate/be_const_strtab.h", build_table_ext());
}

uint32_t build_map::hashcode(const std::string &string)
{
    const char *str = string.data();
    uint32_t c, hash = 2166136261u;
    while ((c = *str++) != '\0') {
        hash = (hash ^ c) * 16777619u;
    }
    return hash;
}

void build_map::make_ceil(const std::string &string)
{
    str_info info;
    info.hash = hashcode(string);
    info.str = string;
    m_hashtable[info.hash % m_hashtable.size()].push_back(info);
}

std::string build_map::escdot(const std::string &string)
{
    if (string.size() > 4 && string.substr(0, 4) == "dot_") {
        return "." + string.substr(4);
    }
    return string;
}

void build_map::print_table()
{
    for (auto i : m_hashtable) {
        std::cout << "+ ";
        for (auto j : i) {
            std::cout << escdot(j.str) << ", ";
        }
        std::cout << std::endl;
    }
}

void build_map::writefile(const std::string &filename, const std::string &text)
{
    std::ofstream fout;
    std::string pathname(filename);
    fout.open(pathname, std::ios::out);
    fout << text;
}

std::string build_map::build_table_def()
{
    std::ostringstream ostr;
    for (auto bucket : m_hashtable) {
        size_t size = bucket.size();
        for (size_t i = 0; i < size; ++i) {
            str_info info = bucket[i];
            std::string next(i < size - 1 ?
                "&be_const_str_" + bucket[i + 1].str : "NULL");
            std::string str(escdot(info.str));
            ostr << "be_define_const_str("
                << info.str << ", \"" << str << "\", "
                << info.hash << "u, " << "0, " << str.size()
                << ", " << next << ");" << std::endl;
        }
    }
    ostr << std::endl;
    ostr << "static const bstring* const m_string_table[] = {" << std::endl;
    size_t size = m_hashtable.size();
    for (size_t i = 0; i < size; ++i) {
        auto bucket = m_hashtable[i];
        if (bucket.size()) {
            ostr << "    (const bstring *)&be_const_str_"
                << bucket[0].str;
        } else {
            ostr << "    NULL";
        }
        ostr << (i < size - 1 ? "," : "") << std::endl;
    }
    ostr << "};" << std::endl << std::endl;
    ostr <<
        "static const struct bconststrtab m_const_string_table = {\n"
        "    .size = " << size << ",\n" <<
        "    .count = " << m_count << ",\n" <<
        "    .table = m_string_table\n" <<
        "};" << std::endl;
    return ostr.str();
}

std::string build_map::build_table_ext()
{
    std::ostringstream ostr;
    for (auto bucket : m_hashtable) {
        for (auto info : bucket) {
            ostr << "extern const bcstring be_const_str_"
                << info.str << ";" << std::endl;
        }
    }
    return ostr.str();
}
