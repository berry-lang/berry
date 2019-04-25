TARGET	 = map_build

ifeq ($(OS), Windows_NT) # Windows
	TARGET	:= $(TARGET).exe
endif

$(TARGET): hash_map.cpp map_build.cpp main.cpp
	@ g++ -O2 hash_map.cpp map_build.cpp main.cpp -o $(TARGET)
