TARGET	 = str_build

ifeq ($(OS), Windows_NT) # Windows
	TARGET	:= $(TARGET).exe
endif

$(TARGET): main.cpp build_map.cpp
	@ g++ -std=c++11 -O2 main.cpp build_map.cpp -o $(TARGET)
