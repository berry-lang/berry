TARGET   = coc
CXXFLAGS = -std=c++11 -O2
CXX      = g++

OBJS    = hash_map.o            \
          macro_table.o         \
          main.o                \
          map_build.o           \
          str_build.o           \
          str_map.o             \

ifeq ($(OS), Windows_NT) # Windows
    TARGET  := $(TARGET).exe
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(Q) $(CXX) $(OBJS) -o $@

$(OBJS): %.o: %.cpp
	$(Q) $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(Q) $(RM) $(OBJS)
