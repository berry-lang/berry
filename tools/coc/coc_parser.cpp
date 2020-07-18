#include "coc_parser.h"
#include <string.h>

static inline int _isalnum(int c)
{
    return isalnum(c) || c == '_';
}

coc_parser::coc_parser(const std::string &text)
{
    m_ptr = text.data();
    m_end = m_ptr + text.size();
    for (; m_ptr < m_end; ++m_ptr) {
        if (*m_ptr == '@') {
            parse_object();
        }
    }
}

void coc_parser::skip_space()
{
    while (isspace(*m_ptr))
        ++m_ptr;
}

bool coc_parser::parse_char_base(int c, bool necessary)
{
    bool res = *m_ptr == c;
    if (!res && necessary)
        throw "error";
    if (res)
        ++m_ptr;
    return res;
}

bool coc_parser::parse_char(int c, bool necessary)
{
    skip_space();
    return parse_char_base(c, necessary);
}

bool coc_parser::parse_char_continue(int c, bool necessary)
{
    int ch;
    while (((ch = *m_ptr) == ' ') || ch == '\t')
        ++m_ptr;
    return parse_char_base(c, necessary);
}

std::string coc_parser::parse_word()
{
    skip_space();
    const char *p = m_ptr;
    if (_isalnum(*m_ptr)) {
        while (_isalnum(*(++m_ptr)));
        return std::string(p, m_ptr - p);
    }
    throw "error";
}

std::string coc_parser::parse_tocomma()
{
    int c;
    skip_space();
    const char *p = m_ptr;
    while (((c = *m_ptr) != ',') && !isspace(c))
        ++m_ptr;
    if (p == m_ptr)
        throw "error";
    return std::string(p, m_ptr - p);
}

std::string coc_parser::parse_tonewline()
{
    int c;
    skip_space();
    const char *p = m_ptr;
    while (((c = *m_ptr) != '\r') && c != '\n')
        ++m_ptr;
    if (p == m_ptr)
        throw "error";
    return std::string(p, m_ptr - p);
}

void coc_parser::parse_object()
{
    const char *text_begin = "@const_object_info_begin";
    size_t len = strlen(text_begin);
    if (m_ptr + len >= m_end) {
        throw "error";
    }
    if (!memcmp(m_ptr, text_begin, len)) {
        m_ptr += len;
        parse_block();
    }
}

void coc_parser::parse_block()
{
    m_block.type = parse_word();
    m_block.name = parse_word();
    parse_attr();
    parse_body();
}

void coc_parser::parse_attr()
{
    skip_char('(');
    parse_attr_pair();
    while (parse_char(',')) {
        parse_attr_pair();
    }
    skip_char(')');
}

void coc_parser::parse_attr_pair()
{
    std::string key = parse_word();
    skip_char(':');
    std::string value = parse_word();
    m_block.attr[key] = value;
}

void coc_parser::parse_body()
{
    skip_char('{');
    if (!parse_char('}')) {
        do {
            parse_body_item();
        } while (!parse_char('}'));
    }
}

void coc_parser::parse_body_item()
{
    object_block::data_value value;
    std::string key = parse_tocomma();
    parse_char_continue(',', true);
    value.value = parse_tocomma();
    if (parse_char_continue(','))
        value.depend = parse_tonewline();
    m_block.data[key] = value;
}
