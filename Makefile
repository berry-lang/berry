CFLAGS    = -Wall -Wextra -std=c99 -pedantic-errors -O2
LIBS      = -lm
TARGET    = berry
CC        = gcc
MKDIR     = mkdir
LFLAGS    = 

INCPATH   = src default
SRCPATH   = src default
GENERATE  = generate
CONFIG	  = default/berry_conf.h
MAP_BUILD = tools/map_build/map_build
CONST_TAB = $(GENERATE)/be_const_strtab.h
MAKE_MAP_BUILD = $(MAKE) -C tools/map_build

ifeq ($(OS), Windows_NT) # Windows
    CFLAGS += -Wno-format # for "%I64d" warning
    LFLAGS += -Wl,--out-implib,berry.lib # export symbols lib for dll linked
    TARGET := $(TARGET).exe
    MAP_BUILD := $(MAP_BUILD).exe
else
    CFLAGS += -DUSE_READLINE_LIB
	LFLAGS += -Wl,--export-dynamic
    LIBS += -lreadline -ldl
endif

ifneq ($(V), 1)
    Q=@
    MSG=@echo
    MAKE_MAP_BUILD += -s Q=$(Q)
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
	$(Q) $(CC) $(OBJS) $(LFLAGS) $(LIBS) -o $@
	$(MSG) done

$(OBJS): %.o: %.c
	$(MSG) [Compile] $<
	$(Q) $(CC) -MM $(CFLAGS) $(INCFLAGS) -MT"$*.d" -MT"$(<:.c=.o)" $< > $*.d
	$(Q) $(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

sinclude $(DEPS)

$(OBJS): $(CONST_TAB)

$(CONST_TAB): $(MAP_BUILD) $(GENERATE) $(SRCS) $(CONFIG)
	$(MSG) [Prebuild] generate resources
	$(Q) $(MAP_BUILD) -i $(SRCPATH) -c $(CONFIG) -o $(GENERATE)

$(GENERATE):
	$(Q) $(MKDIR) $(GENERATE)

$(MAP_BUILD):
	$(MSG) [Make] map_build
	$(Q) $(MAKE_MAP_BUILD)

install:
	cp $(TARGET) /usr/local/bin

uninstall:
	$(RM) /usr/local/bin/$(TARGET)

prebuild: $(MAP_BUILD) $(GENERATE)
	$(MSG) [Prebuild] generate resources
	$(Q) $(MAP_BUILD) -o $(GENERATE) $(SRCPATH) -c $(CONFIG)
	$(MSG) done

clean:
	$(MSG) [Clean...]
	$(Q) $(RM) $(OBJS) $(DEPS) $(GENERATE)/* berry.lib
	$(Q) $(MAKE_MAP_BUILD) clean
	$(MSG) done
