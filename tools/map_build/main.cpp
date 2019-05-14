#include <iostream>
#include <fstream>  
#include <sstream>
#include <string>
#include <regex>
#include "map_build.h"

std::string get_str(const std::string &filename, const std::string &str)
{
    size_t size = filename.size();
    if (size > 2 && filename.substr(size - 2, 2) == ".c") {
        std::regex reg("@const_object_info_begin([^@]+)@const_object_info_end");
        std::sregex_iterator it(str.begin(), str.end(), reg);
        std::sregex_iterator end;
        std::string result;
        while (it != end) {
            result += it->str(1);
            ++it;
        }
        return result;
    }
    return str;
}

static void gen_map_data(const std::string &srcname, const std::string &path)
{
    std::ifstream in(srcname);
    std::ostringstream tmp;
    tmp << in.rdbuf();
    std::string str = tmp.str();
    map_build mb(get_str(srcname, str), path);
    mb.str();
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
static void listdir(const std::string &srcpath, const std::string &dstpath)
{
    DIR *dp;
    struct dirent *ep;
    dp = opendir(srcpath.data());
    if (dp != NULL) {
        while ((ep = readdir(dp)) != NULL) {
            std::string fname(ep->d_name);
            size_t find = fname.find_last_of(".");
            std::string subname(find < fname.size() ? fname.substr(find) : "");
            if (subname == ".c") {
                gen_map_data(srcpath + "/" + fname, dstpath);
            }
        }
        closedir(dp);
    }
}
#else
static void listdir(const std::string &srcpath, const std::string &dstpath)
{
    HANDLE find;
    WIN32_FIND_DATA data;
    find = FindFirstFile((srcpath + "/*").data(), &data);
    if (find != INVALID_HANDLE_VALUE) {
        do {
            std::string fname(data.cFileName);
            size_t find = fname.find_last_of(".");
            std::string subname(find < fname.size() ? fname.substr(find) : "");
            if (subname == ".c") {
                gen_map_data(srcpath + "/" + fname, dstpath);
            }
        } while (FindNextFile(find, &data) != 0);
        FindClose(find);
    }
}
#endif

int main(int argc, char *argv[])
{
    if (argc > 1) {
        for (int i = 2; i < argc; ++i) {
            listdir(argv[i], argv[1]);
        }
    }
	return 0;
}
