#include "be_string.h"
#include <stdio.h>
#include <stdlib.h>

#define count_of(a) (sizeof(a) / sizeof(a[0]))
#define sstr(_s)    cast(char*, cast(bsstring*, _s) + 1)

typedef struct bconststrtab bconststrtab;

#define default_size 24

static const char *const_strlist[] = {
    "iter",
    "list",
    "map",
    "range",
    "print",
    "clock",
    "exit",
    "super",
    "memcount",
    "type",
    "classname",
    "number",
    "string",
    "length",
    "__iterator__",
    "__hasnext__",
    "__next__",
    ".data",
    "init",
    "tostring",
    "append",
    "insert",
    "remove",
    "item",
    "setitem",
    "size",
    "resize",
    "__lower__",
    "__upper__",
    "lower",
    "upper",
    "setrange",
    "open",
    "write",
    "read",
    "readline",
    "seek",
    "tell",
    "flush",
    "close",
    "deinit",
};

static bstring* create_sstr(const char *str)
{
    size_t len = strlen(str);
    size_t size = sizeof(bsstring) + len + 1;
    bsstring *s = malloc(size);
    if (s) {
        s->type = BE_STRING;
        s->next = NULL;
        s->marked = 0;
        s->extra = 0;
        s->slen = cast(bbyte, len);
        strcpy(sstr(s), str);
    }
    return cast(bstring*, s);
}

/* FNV-1a Hash */
static uint32_t strhash(const char *str)
{
    uint32_t ch;
    uint32_t hash = 2166136261u;
    while ((ch = *str++) != '\0') {
        hash = (hash ^ ch) * 16777619u;
    }
    return hash;
}

static bstring* findstr(bstring *list, const char *str)
{
    while (list) {
        if (!strcmp(sstr(list), str)) {
            printf("duplicate string: %s\n", str);
            return list;
        }
        list = (bstring *)list->next;
    }
    return NULL;
}

static bconststrtab* table_build(const char **list, size_t count)
{
    size_t i, size = default_size;
    bconststrtab *strtab = malloc(
        sizeof(bconststrtab) + sizeof(bstring*) * size);
    bstring **table = (bstring **)(strtab + 1);
    strtab->table = (const bstring **)table;

    strtab->size = size;
    strtab->count = count;
    for (i = 0; i < size; ++i) {
        table[i] = NULL;
    }
    for (i = 0; i < count; ++i) {
        const char *str = *list++;
        uint32_t hash = strhash(str);
        bstring **slot = table + hash % size;
        if (!findstr(*slot, str)) {
            bstring *sstr = create_sstr(str);
            if (sstr) {
                sstr->next = cast(void*, *slot);
                *slot = sstr;
            }
        }
    }
    return strtab;
}

static void print_src_header(FILE *fp)
{
    fprintf(fp, "#include \"be_cstrtab.h\"\n"
                "#include \"be_gc.h\"\n"
                "\n");
}

static void print_inc_header(FILE *fp)
{
    fprintf(fp, "#ifndef BE_CSTRTAB_H\n"
                "#define BE_CSTRTAB_H\n\n"
                "#include \"be_string.h\"\n"
                "\n");
}

static void print_inc_footer(FILE *fp)
{
    fprintf(fp, "#endif\n");
}

static const char* name(const char *str)
{
    char *p;
    int ch;
    static char buf[1024];
    strcpy(buf, "be_const_str_");
    p = buf + sizeof("be_const_str_") - 1;
    while ((ch = *str++) != '\0') {
        if (ch >= 0 && ch <= 9 || ch >= 'a' && ch <= 'z'
            || ch >= 'A' && ch <= 'Z' || ch == '_') {
            *p++ = (char)ch;
        } else if (ch == '.') {
            p = strcpy(p, "dot_");
        }
    }
    *p = '\0';
    return buf;
}

static void print_string(FILE *fp, const bstring *str)
{
    const char *s = sstr(str);

    fprintf(fp, "const bcstring %s = {\n", name(s));
    if (str->next) {
        const char *next = sstr(str->next);
        fprintf(fp, "    .next = (bgcobject *)&%s,\n", name(next));
    } else {
        fprintf(fp, "    .next = NULL,\n");
    }
    fprintf(fp, "    .type = BE_STRING,\n");
    fprintf(fp, "    .marked = GC_CONST,\n");
    fprintf(fp, "    .extra = 0,\n");
    fprintf(fp, "    .slen = %d,\n", (int)str->slen);
    fprintf(fp, "    .s = \"%s\"\n", s);
    fprintf(fp, "};\n\n");
}

static void print_table(FILE *fp, bconststrtab *strtab)
{
    fprintf(fp, "static const bstring* m_string_table[] = {\n");
    for (size_t i = 0; i < strtab->size; ++i) {
        const bsstring *str = (const bsstring *)strtab->table[i];
        if (str) {
            fprintf(fp, "    (const bstring *)&%s%s\n",
                        name(sstr(str)), i < strtab->size - 1 ? "," : "");
        } else {
            fprintf(fp, "    NULL%s\n", i < strtab->size - 1 ? "," : "");
        }
    }
    fprintf(fp, "};\n\n");
    fprintf(fp, "const struct bconststrtab be_const_string_table = {\n");
    fprintf(fp, "    .size = %d,\n", strtab->size);
    fprintf(fp, "    .count = %d,\n", strtab->count);
    fprintf(fp, "    .table = m_string_table\n");
    fprintf(fp, "};\n");
}

static void print_strtab(FILE *fp, bconststrtab *strtab)
{
    for (size_t i = 0; i < strtab->size; ++i) {
        const bstring *str = strtab->table[i];
        while (str) {
            print_string(fp, str);
            str = (void*)str->next;
        }
    }
    print_table(fp, strtab);
}

static void print_strtab_extern(FILE *fp, bconststrtab *strtab)
{
    size_t i;
    for (i = 0; i < strtab->size; ++i) {
        const bstring *str = strtab->table[i];
        while (str) {
            const char *s = sstr(str);
            fprintf(fp, "extern const bcstring %s;\n", name(s));
            str = (void*)str->next;
        }
    }
    fprintf(fp, "\n");
}

static void gen_src(bconststrtab *strtab)
{
    FILE *fp = fopen("be_cstrtab.c", "w");
    print_src_header(fp);
    print_strtab(fp, strtab);
    fclose(fp);
}

static void gen_inc(bconststrtab *strtab)
{
    FILE *fp = fopen("be_cstrtab.h", "w");
    print_inc_header(fp);
    print_strtab_extern(fp, strtab);
    print_inc_footer(fp);
    fclose(fp);
}

int main(void)
{
    bconststrtab *strtab;
    strtab = table_build(const_strlist, count_of(const_strlist));
    gen_src(strtab);
    gen_inc(strtab);
    return 0;
}
