TARGET	 = map_build

ifeq ($(OS), Windows_NT) # Windows
	TARGET	:= $(TARGET).exe
endif

$(TARGET): hash_map.cpp map_build.cpp macro_table.cpp main.cpp
	@ g++ -std=c++11 -O2 hash_map.cpp map_build.cpp macro_table.cpp main.cpp -o $(TARGET)
