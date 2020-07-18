#ifndef __COC_PARSER_H
#define __COC_PARSER_H

#include <string>
#include "object_block.h"

class coc_parser {
public:
    coc_parser(const std::string &text);

private:
    void skip_space();
    void skip_char(int c) {
        parse_char(c, true);
    }
    bool parse_char_base(int c, bool necessary);
    bool parse_char(int c, bool necessary = false);
    bool parse_char_continue(int c, bool necessary = false);
    std::string parse_word();
    std::string parse_tocomma();
    std::string parse_tonewline();
    void parse_object();
    void parse_block();
    void parse_attr();
    void parse_attr_pair();
    void parse_body();
    void parse_body_item();

private:
    const char *m_ptr, *m_end;
    object_block m_block;
};

#endif // !__COC_PARSER_H
