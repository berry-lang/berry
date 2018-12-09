#include "be_jsonlib.h"
#include <string.h>
#include <stdlib.h>

#define MAX_INDENT      30
#define INDENT_WIDTH    4

static const char* parser_value(bvm *vm, const char *json);
static void json2str(bvm *vm, int *indent, int idx, int fmt);

static const char* skip_space(const char *s)
{
    int c;
    while (((c = *s) != '\0') && ((c == ' ')
        || (c == '\t') || (c == '\r') || (c == '\n'))) {
        ++s;
    }
    return s;
}

static int is_digit(int c)
{
    return c >= '0' && c <= '9';
}

static const char* match_char(const char *json, int ch)
{
    json = skip_space(json);
    if (*json == ch) {
        return skip_space(json + 1);
    }
    return NULL;
}

static int is_object(bvm *vm, const char *class, int idx)
{
    if (be_isinstance(vm, idx)) {
        const char *name = be_classname(vm, idx);
        return !strcmp(name, class);
    }
    return  0;
}

static void json2berry(bvm *vm, const char *class)
{
    be_getglobal(vm, class);
    be_pushvalue(vm, -2);
    be_call(vm, 1);
    be_moveto(vm, -2, -3);
    be_pop(vm, 2);
}

static const char* parser_true(bvm *vm, const char *json)
{
    if (!strncmp(json, "true", 4)) {
        be_pushbool(vm, btrue);
        return json + 4;
    }
    return NULL;
}

static const char* parser_false(bvm *vm, const char *json)
{
    if (!strncmp(json, "false", 5)) {
        be_pushbool(vm, bfalse);
        return json + 5;
    }
    return NULL;
}

static const char* parser_null(bvm *vm, const char *json)
{
    if (!strncmp(json, "null", 4)) {
        be_pushnil(vm);
        return json + 4;
    }
    return NULL;
}

static const char* parser_string(bvm *vm, const char *json)
{
    if (*json == '"') {
        const char *end = ++json;
        while (*end && *end != '"') {
            ++end;
        }
        if (*end) {
            be_pushnstring(vm, json, end - json);
            return end + 1;
        }
    }
    return NULL;
}

static const char* parser_field(bvm *vm, const char *json)
{
    if (json && *json == '"') {
        json = parser_string(vm, json);
        if (json) {
            json = match_char(json, ':');
            if (json) {
                json = parser_value(vm, json);
                if (json) {
                    be_insert(vm, -3);
                    be_pop(vm, 2); /* pop key and value */
                    return json;
                }
            }
        }
    }
    return json;
}

static const char* parser_object(bvm *vm, const char *json)
{
    json = match_char(json, '{');
    be_newmap(vm);
    if (*json != '}') {
        const char *s;
        json = parser_field(vm, json);
        if (json == NULL) {
            be_pop(vm, 1); /* pop map */
            return NULL;
        }
        while ((s = match_char(json, ',')) != NULL) {
            json = parser_field(vm, s);
            if (json == NULL) {
                be_pop(vm, 1); /* pop map */
                return NULL;
            }
        }
    }
    if (match_char(json, '}') == NULL) {
        be_pop(vm, 1); /* pop map */
        return NULL;
    }
    json2berry(vm, "map");
    return json + 1;
}

static const char* parser_array(bvm *vm, const char *json)
{
    json = match_char(json, '[');
    be_newlist(vm);
    if (*json != ']') {
        const char *s;
        json = parser_value(vm, json);
        if (json == NULL) {
            be_pop(vm, 1); /* pop map */
            return NULL;
        }
        be_append(vm, -2);
        be_pop(vm, 1); /* pop value */
        while ((s = match_char(json, ',')) != NULL) {
            json = parser_value(vm, s);
            if (json == NULL) {
                be_pop(vm, 1); /* pop map */
                return NULL;
            }
            be_append(vm, -2);
            be_pop(vm, 1); /* pop value */
        }
    }
    if (match_char(json, ']') == NULL) {
        be_pop(vm, 1); /* pop map */
        return NULL;
    }
    json2berry(vm, "list");
    return json + 1;
}

static const char* parser_number(bvm *vm, const char *json)
{
    int len;
    char buffer[24];
    const char *end = json;
    while (is_digit(*end)) {
        ++end;
    }
    len = end - json;
    strncpy(buffer, json, len);
    buffer[len] = '\0';
    be_pushint(vm, atoi(buffer));
    return end;
}

/* parser json value */
static const char* parser_value(bvm *vm, const char *json)
{
    json = skip_space(json);
    switch (*json) {
    case '{': /* object */
        return parser_object(vm, json);
    case '[': /* array */
        return parser_array(vm, json);
    case '"': /* string */
        return parser_string(vm, json);
    case 't': /* true */
        return parser_true(vm, json);
    case 'f': /* false */
        return parser_false(vm, json);
    case 'n': /* null */
        return parser_null(vm, json);
    default:
        if (is_digit(*json)) { /* number */
            return parser_number(vm, json);
        }
    }
    return NULL;
}

static int m_json_load(bvm *vm)
{
    if (be_isstring(vm, 1)) {
        const char *json = be_tostring(vm, 1);
        if (parser_value(vm, json)) {
            return be_return(vm);
        }
    }
    return be_returnnil(vm);
}

static void make_indent(bvm *vm, int stridx, int indent)
{
    int i, idx = be_absindex(vm, stridx);
    char buf[MAX_INDENT * INDENT_WIDTH + 1];
    indent = (indent < MAX_INDENT ? indent : MAX_INDENT) * INDENT_WIDTH;
    for (i = 0; i < indent; ++i) {
        buf[i] = ' ';
    }
    buf[indent] = '\0';
    be_pushstring(vm, buf);
    be_strconcat(vm, idx);
    be_pop(vm, 1);
}

static void object_tostr(bvm *vm, int *indent, int idx, int fmt)
{
    be_getmember(vm, idx, "__data__");
    be_pushstring(vm, fmt ? "{\n" : "{");
    be_pushiter(vm, -2); /* map iterator use 1 register */
    *indent += fmt;
    while (be_hasnext(vm, -3)) {
        make_indent(vm, -2, *indent);
        be_next(vm, -3);
        /* key.tostring() */
        json2str(vm, indent, -2, fmt);
        be_strconcat(vm, -5);
        be_pop(vm, 1);
        be_pushstring(vm, fmt ? ": " : ":"); /* add ': ' */
        be_strconcat(vm, -5);
        be_pop(vm, 1);
        /* value.tostring() */
        json2str(vm, indent, -1, fmt);
        be_strconcat(vm, -5);
        be_pop(vm, 3);
        if (be_hasnext(vm, -3)) {
            be_pushstring(vm, fmt ? ",\n" : ",");
            be_strconcat(vm, -3);
            be_pop(vm, 1);
        } else if (fmt) {
            be_pushstring(vm, "\n");
            be_strconcat(vm, -3);
            be_pop(vm, 1);
        }
    }
    *indent -= fmt;
    be_pop(vm, 1); /* pop iterator */
    make_indent(vm, -1, *indent);
    be_pushstring(vm, "}");
    be_strconcat(vm, -2);
    be_moveto(vm, -2, -3);
    be_pop(vm, 2);
}

static void array_tostr(bvm *vm, int *indent, int idx, int fmt)
{
    be_getmember(vm, idx, "__data__");
    be_pushstring(vm, fmt ? "[\n" : "[");
    be_pushiter(vm, -2);
    *indent += fmt;
    while (be_hasnext(vm, -3)) {
        make_indent(vm, -2, *indent);
        be_next(vm, -3);
        json2str(vm, indent, -1, fmt);
        be_strconcat(vm, -4);
        be_pop(vm, 2);
        if (be_hasnext(vm, -3)) {
            be_pushstring(vm, fmt ? ",\n" : ",");
            be_strconcat(vm, -3);
            be_pop(vm, 1);
        } else if (fmt) {
            be_pushstring(vm, "\n");
            be_strconcat(vm, -3);
            be_pop(vm, 1);
        }
    }
    *indent -= fmt;
    be_pop(vm, 1); /* pop iterator */
    make_indent(vm, -1, *indent);
    be_pushstring(vm, "]");
    be_strconcat(vm, -2);
    be_moveto(vm, -2, -3);
    be_pop(vm, 2);
}

static void json2str(bvm *vm, int *indent, int idx, int fmt)
{
    if (is_object(vm, "map", idx)) {
        object_tostr(vm, indent, idx, fmt);
    } else if (is_object(vm, "list", idx)) {
        array_tostr(vm, indent, idx, fmt);
    } else if (be_isnil(vm, idx)) {
        be_pushstring(vm, "null");
    } else if (be_isstring(vm, idx)) { /* add '"" to strings */
        be_pushfstring(vm, "\"%s\"", be_tostring(vm, idx));
    } else {
        be_tostring(vm, idx);
        be_pushvalue(vm, idx); /* push to top */
    }
}

static int m_json_dumps(bvm *vm)
{
    int indent = 0, argc = be_top(vm);
    int fmt = 0;
    if (argc > 1) {
        fmt = !strcmp(be_tostring(vm, 2), "format");
    }
    json2str(vm, &indent, 1, fmt);
    return be_return(vm);
}

void be_json_init(bvm *vm)
{
    be_regcfunc(vm, "json_load", m_json_load);
    be_regcfunc(vm, "json_dumps", m_json_dumps);
}
