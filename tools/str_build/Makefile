TARGET	 = str_build

ifeq ($(OS), Windows_NT) # Windows
	TARGET	:= $(TARGET).exe
endif

$(TARGET): main.cpp build_map.cpp
	@ g++ -O2 main.cpp build_map.cpp -o $(TARGET)
