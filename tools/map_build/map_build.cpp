#include "map_build.h"
#include "hash_map.h"
#include <regex>
#include <sstream>
#include <fstream>

map_build::map_build(const std::string &str)
{
	parse_block(str);
}

void map_build::parse_block(const std::string &str)
{
	const std::string body;
	std::regex reg("(\\w+)\\s+(\\w+)\\s+\\{([^\\}]*)\\}");
	std::smatch res;
	std::sregex_iterator it(str.begin(), str.end(), reg);
	std::sregex_iterator end;
	while (it != end) {
		block bl;
		bl.type = it->str(1);
		bl.name = it->str(2);
		bl.attr = parse_body(it->str(3));
		m_block.push_back(bl);
		++it;
	}
}

std::map<std::string, std::string> map_build::parse_body(const std::string &str)
{
	std::regex reg("([\\.\\w]+),\\s*([^\\n]+)");
	std::smatch res;
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
	std::ofstream fout;
	fout.open(filename, std::ios::out);
	fout << text;
}

std::string map_build::block_tostring(const block &block)
{
	std::ostringstream ostr;

	if (block.type == "map") {
		ostr << map_tostring(block, block.name);
	} else if (block.type == "class") {
		ostr << class_tostring(block);
	}
	writefile("be_fixed_" + block.name + ".h", ostr.str());
	return ostr.str();
}

std::string map_build::class_tostring(const block &block)
{
	std::ostringstream ostr;
	hash_map map(block.attr);

	ostr << map_tostring(block, "__class_map") << std::endl;
	ostr << "const bclass " + block.name + " = {\n"
		 << "    be_const_header_class(),\n"
		 << "    .nvar = " << map.var_count() << ",\n"
		 << "    .super = NULL,\n"
		 << "    .members = (bmap *)__class_map,\n"
		 << "    .name = (bstring *)&be_const_str_" << block.name << "\n"
		 << "};\n";
	return ostr.str();
}

std::string map_build::map_tostring(const block &block, const std::string &name)
{
	std::ostringstream ostr;
	hash_map map(block.attr);

	hash_map::entry_table list = map.entry_list();
	ostr << "static const bmapnode __map_slots = {\n";
	for (auto it : list) {
		int next = it.next;
		std::string key = it.key;
		std::string value = it.value;
		ostr << "    { be_const_key(" << key << ", "
			<< next << "), " << value << " }," << std::endl;
	}
	ostr << "};\n\n";

	ostr << ((name == "__class_map") ? "static " : "")
		 << "const bmap " + name + " = {\n"
		 << "    be_const_header_map(),\n"
		 << "    .slots = (bmapnode *)__map_slots,\n"
		 << "    .lastfree = (bmapnode *)&__map_slots[" << list.size() - 1 << "],\n"
		 << "    .size = " << list.size() << ",\n"
		 << "    .count = " << list.size() << "\n"
		 << "};\n";
	return ostr.str();
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
