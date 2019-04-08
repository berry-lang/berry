#ifndef __MAP_BUILD_H
#define __MAP_BUILD_H

#include <map>
#include <vector>

class map_build {
public:
	map_build(const std::string &str);
	std::string str();

private:
	struct block {
		std::string type;
		std::string name;
		std::map<std::string, std::string> attr;
	};

	void parse_block(const std::string &str);
	std::map<std::string, std::string> parse_body(const std::string &str);
	std::string block_tostring(const block &block);
	std::string class_tostring(const block &block);
	std::string map_tostring(const block &block, const std::string &name);
	void writefile(const std::string &filename, const std::string &text);

private:
	std::vector<block> m_block;
};

#endif // !__MAP_BUILD_H
