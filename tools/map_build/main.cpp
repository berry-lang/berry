#include <iostream>
#include <fstream>  
#include <sstream>
#include <string>
#include <regex>
#include "map_build.h"
#include "macro_table.h"

class argument {
public:
    argument(int argc, char **argv);
    ~argument();
    void build();

private:
    void add_arg(const std::string &arg);
    std::string get_str(const std::string &filename, const std::string &str);
    void gen_map_data(const std::string &srcname, const std::string &path);
    void listdir(const std::string &srcpath, const std::string &dstpath);

private:
    enum arg_state {
        Input,
        Output,
        Config
    };
    std::string m_output;
    std::vector<std::string> m_input;
    std::vector<std::string> m_config;
    arg_state m_state;
    macro_table *m_macro;
};

std::string argument::get_str(const std::string &filename, const std::string &str)
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

void argument::gen_map_data(const std::string &srcname, const std::string &path)
{
    std::ifstream in(srcname);
    std::ostringstream tmp;
    tmp << in.rdbuf();
    std::string str = tmp.str();
    map_build mb(m_macro, path);
    mb.parse_block(get_str(srcname, str));
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
void argument::listdir(const std::string &srcpath, const std::string &dstpath)
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
void argument::listdir(const std::string &srcpath, const std::string &dstpath)
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

void argument::build()
{
    for (auto it : m_input) {
        listdir(it, m_output);
    }
}

argument::argument(int argc, char **argv)
{
    m_state = Input;
    for (int i = 1; i < argc; ++i) {
        add_arg(argv[i]);
    }
    m_macro = new macro_table();
    for (auto it : m_config) {
        m_macro->scan_file(it);
    }
}

argument::~argument()
{
    delete m_macro;
}

void argument::add_arg(const std::string &arg)
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
    argument arg(argc, argv);
    arg.build();
    return 0;
}
