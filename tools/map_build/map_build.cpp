#include "map_build.h"
#include "hash_map.h"
#include <regex>
#include <sstream>
#include <fstream>

map_build::map_build(const std::string &str, const std::string &path)
{
    m_outpath = path.substr(path.find_last_of("\\") + 1);
	parse_block(str);
}

void map_build::parse_block(const std::string &str)
{
	const std::string body;
	std::regex reg("(\\w+)\\s+(\\w+)(\\s*\\(([^\\)]*)\\)\\s*|\\s+)\\{([^\\}]*)\\}");
	std::sregex_iterator it(str.begin(), str.end(), reg);
	std::sregex_iterator end;
	while (it != end) {
		block bl;
		bl.type = it->str(1);
		bl.name = it->str(2);
        bl.attr = parse_attr(it->str(4));
		bl.data = parse_body(it->str(5));
		m_block.push_back(bl);
		++it;
	}
}

std::map<std::string, std::string> map_build::parse_body(const std::string &str)
{
    std::regex reg("(\\w+|\\.\\w+|\\(\\)|-\\*|<<|>>|"
                   "<=|>=|==|!=|[<>\\+\\-\\*/%&\\|\\^|~])\\s*,\\s*([^\\n^\\r]+)");
    std::sregex_iterator it(str.begin(), str.end(), reg);
    std::sregex_iterator end;
    std::map<std::string, std::string> data;
    while (it != end) {
        data[it->str(1)] = it->str(2);
        ++it;
    }
    return data;
}

std::map<std::string, std::string> map_build::parse_attr(const std::string &str)
{
    std::regex reg("(\\w+)\\s*:\\s*(\\w+)([^\\w]+|$)");
    std::sregex_iterator it(str.begin(), str.end(), reg);
    std::sregex_iterator end;
    std::map<std::string, std::string> attr;
    while (it != end) {
        attr[it->str(1)] = it->str(2);
        ++it;
    }
    return attr;
}

void map_build::writefile(const std::string &filename, const std::string &text)
{
    std::string pathname(m_outpath + "/" + filename);
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

std::string map_build::block_tostring(const block &block)
{
	std::ostringstream ostr;

	if (block.type == "map") {
		ostr << map_tostring(block, block.name);
	} else if (block.type == "class") {
		ostr << class_tostring(block);
	} else if (block.type == "vartab") {
		ostr << vartab_tostring(block);
	}
	writefile("be_fixed_" + block.name + ".h", ostr.str());
	return ostr.str();
}

std::string map_build::class_tostring(const block &block)
{
	std::ostringstream ostr;
	hash_map map(block.data);

	ostr << map_tostring(block, "__class_map", true) << std::endl;
	ostr << scope(block) << "const bclass " + block.name + " = {\n"
		 << "    be_const_header_class(),\n"
		 << "    .nvar = " << map.var_count() << ",\n"
		 << "    .super = " << super(block) << ",\n"
		 << "    .members = (bmap *)&__class_map,\n"
		 << "    .name = (bstring *)&be_const_str_" << name(block) << "\n"
		 << "};\n";
	return ostr.str();
}

std::string map_build::map_tostring(const block &block, const std::string &name, bool local)
{
	std::ostringstream ostr;
	hash_map map(block.data);

	hash_map::entry_table list = map.entry_list();
	ostr << "static const bmapnode __map_slots[] = {\n";
	for (auto it : list) {
		int next = it.next;
		std::string key = it.key;
		std::string value = it.value;
		ostr << "    { be_const_key(" << key << ", "
			<< next << "), " << value << " }," << std::endl;
	}
	ostr << "};\n\n";

	ostr << (local ? "static " : scope(block))
		 << "const bmap " + name + " = {\n"
		 << "    be_const_header_map(),\n"
		 << "    .slots = (bmapnode *)__map_slots,\n"
		 << "    .lastfree = (bmapnode *)&__map_slots[" << list.size() - 1 << "],\n"
		 << "    .size = " << list.size() << ",\n"
		 << "    .count = " << list.size() << "\n"
		 << "};\n";
	return ostr.str();
}

std::string map_build::vartab_tostring(const block &block)
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
			"    .count = " << varvec.size() <<
			", .capacity = " << varvec.size() <<
			", .size = sizeof(bvalue),\n" <<
			"    .data = (void*)__vlist_array,\n" <<
			"    .end = (void*)(__vlist_array + " << varvec.size() - 1 << ")\n" <<
			"};" << std::endl;
	return ostr.str();
}

std::string map_build::scope(const block &block)
{
    if (block.attr.find("scope") != block.attr.end()
        && block.attr.at("scope") == "local") {
        return "static ";
    }
    return "";
}

std::string map_build::super(const block &block)
{
    if (block.attr.find("super") == block.attr.end()) {
        return "NULL";
    }
    return block.attr.at("super");
}

std::string map_build::name(const block &block)
{
    if (block.attr.find("name") == block.attr.end()) {
        return block.name;
    }
    return block.attr.at("name");
}

std::string map_build::str()
{
	hash_map map;
	std::ostringstream ostr;

	for (auto it : m_block) {
		ostr << block_tostring(it) << std::endl;
	}
	return ostr.str();
}
