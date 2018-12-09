#include "be_jsonlib.h"
#include <string.h>

static const char* parser_value(bvm *vm, const char *json);

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

static const char* object_field(bvm *vm, const char *json)
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
        json = object_field(vm, json);
        if (json == NULL) {
            be_pop(vm, 1); /* pop map */
            return NULL;
        }
        while ((s = match_char(json, ',')) != NULL) {
            json = object_field(vm, s);
            if (json == NULL) {
                be_pop(vm, 1); /* pop map */
                return NULL;
            }
        }
    }
    return json;
}

/* parser json value */
static const char* parser_value(bvm *vm, const char *json)
{
    json = skip_space(json);
    switch (*json) {
    case '{': /* object */
        return parser_object(vm, json);
    case '[': /* array */
        break;
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
            return NULL;
        }
    }
    return NULL;
}

static int m_init(bvm *vm)
{
    if (be_isstring(vm, 2)) {
        const char *json = be_tostring(vm, 2);
        parser_value(vm, json);
        be_setmember(vm, 1, "__data__");
    } else {
        be_pushnil(vm);
    }
    return be_returnnil(vm);
}

static void object_tostring(bvm *vm)
{
    be_pushstring(vm, "{");
    be_pushiter(vm, -2); /* map iterator use 1 register */
    while (be_hasnext(vm, -3)) {
        be_next(vm, -3);
        /* key.tostring() */
        if (be_isstring(vm, -2)) { /* add ''' to strings */
            be_pushfstring(vm, "\"%s\"", be_tostring(vm, -2));
        } else {
            be_tostring(vm, -2);
            be_pushvalue(vm, -2); /* push to top */
        }
        be_strconcat(vm, -5);
        be_pop(vm, 1);
        be_pushstring(vm, ": "); /* add ': ' */
        be_strconcat(vm, -5);
        be_pop(vm, 1);
        /* value.tostring() */
        if (be_isstring(vm, -1)) { /* add ''' to strings */
            be_pushfstring(vm, "\"%s\"", be_tostring(vm, -1));
        } else {
            be_tostring(vm, -1);
            be_pushvalue(vm, -1); /* push to top */
        }
        be_strconcat(vm, -5);
        be_pop(vm, 3);
        if (be_hasnext(vm, -3)) {
            be_pushstring(vm, ", ");
            be_strconcat(vm, -3);
            be_pop(vm, 1);
        }
    }
    be_pop(vm, 1); /* pop iterator */
    be_pushstring(vm, "}");
    be_strconcat(vm, -2);
    be_pop(vm, 1);
}

static int m_tostring(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    object_tostring(vm);
    return be_return(vm);
}

void be_json_init(bvm *vm)
{
    static const bmemberinfo members[] = {
        { "__data__", NULL },
        { "init", m_init },
        { "tostring", m_tostring },
        { NULL, NULL }
    };
    be_regclass(vm, "bjson", members);
}
