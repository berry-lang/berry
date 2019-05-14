CFLAGS    = -Wall -Wextra -std=c99 -pedantic-errors -O2
LIBS      = -lm
TARGET    = berry
CC        = gcc
MAKE      = make
MKDIR     = mkdir

INCPATH   = src default
SRCPATH   = src default
GENERATE  = generate/be_const_strtab.h
MAP_BUILD = tools/map_build/map_build
STR_BUILD = tools/str_build/str_build

ifeq ($(OS), Windows_NT) # Windows
    CFLAGS += -Wno-format # for "%I64d" warning
    TARGET := $(TARGET).exe
    MAP_BUILD := $(MAP_BUILD).exe
    STR_BUILD := $(STR_BUILD).exe
else
    CFLAGS += -DUSE_READLINE_LIB
    LIBS += -lreadline
endif

ifneq ($(V), 1)
    Q=@
    MSG=@echo
else
    MSG=@true
endif

SRCS     = $(foreach dir, $(SRCPATH), $(wildcard $(dir)/*.c))
OBJS     = $(patsubst %.c, %.o, $(SRCS))
DEPS     = $(patsubst %.c, %.d, $(SRCS))
INCFLAGS = $(foreach dir, $(INCPATH), -I"$(dir)")

.PHONY : clean

all: $(TARGET)

debug: CFLAGS += -O0 -g -DBE_DEBUG
debug: all

$(TARGET): $(OBJS)
	$(MSG) [Linking...]
	$(Q) $(CC) $(OBJS) $(CFLAGS) $(LIBS) -o $@
	$(MSG) done

$(OBJS): %.o: %.c
	$(MSG) [Compile] $<
	$(Q) $(CC) -MM $(CFLAGS) $(INCFLAGS) -MT"$*.d" -MT"$(<:.c=.o)" $< > $*.d
	$(Q) $(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

sinclude $(DEPS)

$(OBJS): $(GENERATE)

$(GENERATE): $(STR_BUILD) $(MAP_BUILD) generate $(SRCS)
	$(MSG) [Prebuild] generate resources
	$(Q) $(MAP_BUILD) generate $(SRCPATH)
	$(Q) $(STR_BUILD) generate $(SRCPATH) generate

generate:
	$(Q) $(MKDIR) generate

$(STR_BUILD):
	$(MSG) [Make] str_build
	$(Q) $(MAKE) -C tools/str_build -s

$(MAP_BUILD):
	$(MSG) [Make] map_build
	$(Q) $(MAKE) -C tools/map_build -s

install:
	cp $(TARGET) /usr/local/bin

uninstall:
	$(RM) /usr/local/bin/$(TARGET)

prebuild: $(STR_BUILD) $(MAP_BUILD) generate
	$(MSG) [Prebuild] generate resources
	$(Q) $(MAP_BUILD) $(SRCS) generate
	$(Q) $(STR_BUILD) $(SRCS) generate
	$(MSG) done

clean:
	$(MSG) [Clean...]
	$(Q) $(RM) $(OBJS) $(DEPS)
	$(Q) $(RM) generate/*
	$(MSG) done
