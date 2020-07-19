#include "block_builder.h"
#include "hash_map.h"
#include "macro_table.h"
#include "object_block.h"
#include <regex>
#include <sstream>
#include <fstream>

static bool depend(const object_block *object, const macro_table *macro)
{
    auto it = object->attr.find("depend");
    if (it != object->attr.end()) {
        return macro->query(it->second);
    }
    return true;
}

block_builder::block_builder(const object_block *object, const macro_table *macro)
{
    m_block.name = object->name;
    if (depend(object, macro)) {
        m_block.type = object->type;
        m_block.attr = object->attr;
        for (auto i : object->data) {
            if (i.second.depend.empty() || macro->query(i.second.depend))
                m_block.data[i.first] = i.second.value;
        }
    }
}

std::string block_builder::block_tostring(const block &block)
{
    std::ostringstream ostr;
    if (block.type == "map") {
        ostr << map_tostring(block, block.name);
    } else if (block.type == "class") {
        ostr << class_tostring(block);
    } else if (block.type == "vartab") {
        ostr << vartab_tostring(block);
    } else if (block.type == "module") {
        ostr << module_tostring(block);
    }
    return ostr.str();
}

std::string block_builder::class_tostring(const block &block)
{
    bool empty_map = block.data.empty();
    std::ostringstream ostr;
    hash_map map(block.data);
    std::string map_name(block.name + "_map");

    if (!empty_map) {
        ostr << map_tostring(block, map_name, true) << std::endl;
    }
    ostr << scope(block) << "const bclass " << block.name << " = {\n"
         << "    be_const_header_class(),\n"
         << "    .nvar = " << map.var_count() << ",\n"
         << "    .super = " << super(block) << ",\n"
         << "    .members = " << (empty_map ? "NULL" : "(bmap *)&" + map_name) << ",\n"
         << "    .name = (bstring *)&be_const_str_" << name(block) << "\n"
         << "};\n";
    return ostr.str();
}

std::string block_builder::map_tostring(const block &block, const std::string &name, bool local)
{
    std::ostringstream ostr;
    hash_map map(block.data);
    std::string map_name(name + "_slots");

    hash_map::entry_table list = map.entry_list();
    ostr << "static const bmapnode " << map_name << "[] = {\n";
    for (auto it : list) {
        m_strtab.push_back(it.key);
        ostr << "    { be_const_key(" << it.key << ", "
             << it.next << "), " << it.value << " }," << std::endl;
    }
    ostr << "};\n\n";

    ostr << (local ? "static " : scope(block))
         << "const bmap " + name + " = {\n"
         << "    be_const_header_map(),\n"
         << "    .slots = (bmapnode *)" << map_name << ",\n"
         << "    .lastfree = (bmapnode *)&" << map_name <<
                "[" << list.size() - 1 << "],\n"
         << "    .size = " << list.size() << ",\n"
         << "    .count = " << list.size() << "\n"
         << "};\n";
    return ostr.str();
}

std::string block_builder::vartab_tostring(const block &block)
{
    std::ostringstream ostr;
    struct block idxblk;
    std::vector<std::string> varvec;
    int index = 0;

    idxblk = block;
    idxblk.data.clear();
    for (auto it : block.data) {
        varvec.push_back(it.second);
        it.second = "int(" + std::to_string(index++) + ")";
        idxblk.data.insert(it);
    }

    ostr << map_tostring(idxblk, block.name + "_map", true) << std::endl;
    ostr << "static const bvalue __vlist_array[] = {\n";
    for (auto it : varvec) {
        ostr << "    be_const_" << it << "," << std::endl;
    }
    ostr << "};\n\n";

    ostr << "static const bvector " << block.name << "_vector = {\n"
            "    .capacity = " << varvec.size() << ",\n" <<
            "    .size = sizeof(bvalue),\n" <<
            "    .count = " << varvec.size() << ",\n" <<
            "    .data = (void*)__vlist_array,\n" <<
            "    .end = (void*)(__vlist_array + " << varvec.size() - 1 << ")\n" <<
            "};" << std::endl;
    return ostr.str();
}

std::string block_builder::module_tostring(const block &block)
{
    std::ostringstream ostr;
    std::string name("m_lib" + block.name);
    std::string map_name(name + "_map");

    ostr << map_tostring(block, map_name, true) << std::endl;
    ostr << "static const bmodule " << name << " = {\n"
         << "    be_const_header_module(),\n"
         << "    .table = (bmap*)&" << map_name << ",\n"
         << "    .info.name = \"" << block.name << "\"\n"
         << "};" << std::endl;
    if (scope(block).empty()) { /* extern */
        ostr << "\nbe_define_const_module(" << block.name << ", "
             << init(block) << ");" << std::endl;
    }
    return ostr.str();
}

std::string block_builder::scope(const block &block)
{
    if (block.attr.find("scope") != block.attr.end()
        && block.attr.at("scope") == "local") {
        return "static ";
    }
    return "";
}

std::string block_builder::super(const block &block)
{
    if (block.attr.find("super") == block.attr.end()) {
        return "NULL";
    }
    return "(bclass *)&" + block.attr.at("super");
}

std::string block_builder::name(const block &block)
{
    if (block.attr.find("name") == block.attr.end()) {
        return block.name;
    }
    return block.attr.at("name");
}

std::string block_builder::init(const block &block)
{
    if (block.attr.find("init") == block.attr.end()) {
        return "NULL";
    }
    return block.attr.at("init");
}

void block_builder::writefile(const std::string &filename, const std::string &text)
{
    std::string pathname(filename);
    std::string otext("#include \"be_constobj.h\"\n\n" + text);

    std::ostringstream buf;
    std::ifstream fin(pathname);
    buf << fin.rdbuf();
    if (buf.str() != otext) {
        std::ofstream fout;
        fout.open(pathname, std::ios::out);
        fout << otext;
        fout.close();
    }
}

void block_builder::dumpfile(const std::string &path)
{
    std::string s = block_tostring(m_block);
    writefile(path + "/be_fixed_" + m_block.name + ".h", s);
}

const std::vector<std::string>& block_builder::strtab() const
{
    return m_strtab;
}
