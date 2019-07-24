#include "main.h"
#include "map_build.h"
#include "str_build.h"
#include "str_map.h"
#include "macro_table.h"
#include <iostream>
#include <fstream>  
#include <sstream>
#include <regex>

std::string builder::info_block(const std::string &text)
{
    std::regex reg("@const_object_info_begin([^@]+)@const_object_info_end");
    std::sregex_iterator it(text.begin(), text.end(), reg);
    std::sregex_iterator end;
    std::string result;
    while (it != end) {
        result += it->str(1);
        ++it;
    }
    return result;
}

void builder::parse_all(const std::string &filename, const std::string &subname)
{
    std::string text = readfile(filename);
    if (subname == ".c" || subname == ".cc") {
        map_build mb(m_macro, m_output);
        mb.parse_block(info_block(text));
        m_strmap->parse_text("temp.h", mb.str());
    }
    m_strmap->parse_text(filename, text);
}

std::string builder::readfile(const std::string &filename)
{
    std::ifstream in(filename);
    std::ostringstream tmp;
    tmp << in.rdbuf();
    return tmp.str();
}

#ifndef _MSC_VER
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#else
#include <windows.h>
#include <direct.h>
#endif

#ifndef _MSC_VER
void builder::scandir(const std::string &srcpath)
{
    DIR *dp;
    struct dirent *ep;
    dp = opendir(srcpath.data());
    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL) {
            std::string fname(ep->d_name);
            size_t find = fname.find_last_of(".");
            std::string subname(find < fname.size() ? fname.substr(find) : "");
            if (subname == ".c" || subname == ".h") {
                parse_all(srcpath + "/" + fname, subname);
            }
        }
        closedir(dp);
    }
}
#else
void builder::scandir(const std::string &srcpath)
{
    HANDLE find;
    WIN32_FIND_DATA data;
    find = FindFirstFile((srcpath + "/*").data(), &data);
    if (find != INVALID_HANDLE_VALUE) {
        do {
            std::string fname(data.cFileName);
            size_t find = fname.find_last_of(".");
            std::string subname(find < fname.size() ? fname.substr(find) : "");
            if (subname == ".c" || subname == ".cpp" ||
                subname == ".cc" || subname == ".h" || subname == ".hpp") {
                parse_all(srcpath + "/" + fname, subname);
            }
        } while (FindNextFile(find, &data) != 0);
        FindClose(find);
    }
}
#endif

void builder::build()
{
    for (auto it : m_input) {
        scandir(it);
    }
    str_build sb(m_strmap->data());
    sb.build(m_output);
}

builder::builder(int argc, char **argv)
{
    m_state = Input;
    for (int i = 1; i < argc; ++i) {
        add_arg(argv[i]);
    }
    m_strmap = new str_map();
    m_macro = new macro_table();
    for (auto it : m_config) {
        m_macro->scan_file(it);
    }
}

builder::~builder()
{
    delete m_macro;
    delete m_strmap;
}

void builder::add_arg(const std::string &arg)
{
    if (arg == "-i") {
        m_state = Input;
    } else if (arg == "-o") {
        m_state = Output;
    } else if (arg == "-c") {
        m_state = Config;
    } else {
        switch (m_state) {
        case Output:
            m_output = arg;
            break;
        case Config:
            m_config.push_back(arg);
            break;
        case Input:
        default:
            m_input.push_back(arg);
            break;
        }
        m_state = Input;
    }
}

int main(int argc, char *argv[])
{
    builder arg(argc, argv);
    arg.build();
    return 0;
}
