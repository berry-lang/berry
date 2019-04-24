#include <iostream>
#include <string>
#include <map>
#include <regex>
#include <fstream>
#include <sstream>
#include "build_map.h"

class str_map {
public:
    str_map() {}
    void scan_file(const std::string &filename);
    std::map<std::string, int> data() { return m_map; }

private:
    std::map<std::string, int> m_map;
};

static std::string readfile(const std::string &filename)
{
    std::ifstream in(filename);
    std::ostringstream tmp;
    tmp << in.rdbuf();
    return tmp.str();
}

void str_map::scan_file(const std::string &filename)
{
    std::string str(readfile(filename));
    std::regex reg("\\bbe_const_str_(\\w*)\\b");
    std::sregex_iterator it(str.begin(), str.end(), reg);
    std::sregex_iterator end;
    while (it != end) {
        m_map[it->str(1)] = 0;
        ++it;
    }
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
static void listdir(str_map *map, const std::string &path)
{
    DIR *dp;
    struct dirent *ep;
    dp = opendir(path.data());
    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL) {
            std::string fname(ep->d_name);
            size_t find = fname.find_last_of(".");
            std::string subname(find < fname.size() ? fname.substr(find) : "");
            if ((subname == ".c" || subname == ".h") &&
                fname != "be_const_strtab_def.h" && fname != "be_const_strtab.h") {
                map->scan_file(path + "/" + fname);
            }
        }
        closedir(dp);
    }
}
#else
static void listdir(str_map *map, const std::string &path)
{
    HANDLE find;
    WIN32_FIND_DATA data;
    find = FindFirstFile((path + "/*").data(), &data);
    if (find != INVALID_HANDLE_VALUE) {
        do {
            std::string fname(data.cFileName);
            size_t find = fname.find_last_of(".");
            std::string subname(find < fname.size() ? fname.substr(find) : "");
            if ((subname == ".c" || subname == ".h") &&
                fname != "be_const_strtab_def.h" && fname != "be_const_strtab.h") {
                map->scan_file(path + "/" + fname);
            }
        } while (FindNextFile(find, &data) != 0);
        FindClose(find);
    }
}
#endif

int main(int argc, char *argv[])
{
    str_map smap;
    listdir(&smap, "src");
    listdir(&smap, "generate");
    build_map(smap.data());
    return 0;
}
