#include <iostream>
#include <fstream>  
#include <sstream>
#include <string>

#include "map_build.h"

using namespace std;

int main(int argc, char *argv[])
{
	if (argc > 1) {
		ifstream in(argv[1]);
		ostringstream tmp;
		tmp << in.rdbuf();
		string str = tmp.str();
		map_build mb(str);
		mb.str();
	}
	return 0;
}
