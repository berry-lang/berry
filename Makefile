CFLAGS      = -Wall -Wextra -std=c99 -pedantic-errors -O2
DEBUG_FLAGS = -O0 -g -DBE_DEBUG
TEST_FLAGS  = $(DEBUG_FLAGS) --coverage -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined
LIBS        = -lm
TARGET      = berry
CC          = gcc
MKDIR       = mkdir
LFLAGS      =
PREFIX      = /usr/local
BINDIR      = $(PREFIX)/bin

INCPATH     = src default
SRCPATH     = src default
GENERATE    = generate
CONFIG      = default/berry_conf.h
COC         = tools/coc/coc
CONST_TAB   = $(GENERATE)/be_const_strtab.h

ifeq ($(OS), Windows_NT) # Windows
    CFLAGS    += -Wno-format # for "%I64d" warning
    LFLAGS    += -Wl,--out-implib,berry.lib # export symbols lib for dll linked
    TARGET    := $(TARGET).exe
    PYTHON    ?= python # only for windows and need python3
    COC       := $(PYTHON) $(COC)
else
    CFLAGS    += -DUSE_READLINE_LIB
    LIBS      += -lreadline -ldl
    OS        := $(shell uname)
    ifeq ($(OS), Linux)
        LFLAGS += -Wl,--export-dynamic
    endif
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

.PHONY : clean docker-berry docs-dev docs-dev-shell

all: $(TARGET)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

test: CFLAGS += $(TEST_FLAGS)
test: LFLAGS += $(TEST_FLAGS)
test: all
	$(MSG) [Run Testcases...]
	$(Q) ./testall.be
	$(Q) $(RM) */*.gcno */*.gcda

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

$(CONST_TAB): $(GENERATE) $(SRCS) $(CONFIG)
	$(MSG) [Prebuild] generate resources
	$(Q) $(COC)  -o $(GENERATE) $(SRCPATH) -c $(CONFIG)

$(GENERATE):
	$(Q) $(MKDIR) $(GENERATE)

install:
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/$(TARGET)

prebuild: $(GENERATE)
	$(MSG) [Prebuild] generate resources
	$(Q) $(COC) -o $(GENERATE) $(SRCPATH) -c $(CONFIG)
	$(MSG) done

clean:
	$(MSG) [Clean...]
	$(Q) $(RM) $(OBJS) $(DEPS) $(GENERATE)/* berry.lib
	$(MSG) done

docker-berry:
	docker build -t berry-lang/berry -f Dockerfile.berry .

docs-dev:
	docker compose up --build --remove-orphans

docs-dev-shell:
	docker compose run docs-dev bash
