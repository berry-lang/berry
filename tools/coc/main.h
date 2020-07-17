#ifndef __MAIN_H
#define __MAIN_H

#include <string>
#include <vector>

class macro_table;
class str_map;

class builder {
public:
    builder(int argc, char **argv);
    ~builder();
    void build();

private:
    void add_arg(const std::string &arg);
    std::string info_block(const std::string &text);
    void parse_all(const std::string &filename, const std::string &subname);
    void scandir(const std::string &srcpath);
    std::string readfile(const std::string &filename);

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
    str_map *m_strmap;
};

#endif
