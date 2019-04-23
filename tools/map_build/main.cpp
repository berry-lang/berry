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

int main(int argc, char *argv[])
{
	if (argc > 1) {
        if (argc > 2) {
            std::string path(argv[argc - 1]);
            for (int i = 1; i < argc - 1; ++i) {
                gen_map_data(argv[i], path);
            }
        } else {
            gen_map_data(argv[1], ".");
        }
	}
	return 0;
}
