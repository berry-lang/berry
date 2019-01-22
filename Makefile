CFLAGS	 = -Wall -Wextra -std=c99 -pedantic-errors -O2
LIBS	 =  -lm
TARGET	 = berry
CC	 = gcc

INCPATH	 = src default
SRCPATH	 = src default

ifneq ($(OS), Windows_NT) # not windows
    CFLAGS += -DUSE_READLINE_LIB
    LIBS += -lreadline
endif

SRCS	 = $(foreach dir, $(SRCPATH), $(wildcard $(dir)/*.c))
OBJS	 = $(patsubst %.c, %.o, $(SRCS))
DEPS	 = $(patsubst %.c, %.d, $(SRCS))
CFLAGS	+= $(foreach dir, $(INCPATH), -I"$(dir)")

all: $(TARGET)

debug: CFLAGS += -O0 -g -DBE_DEBUG
debug: $(TARGET)

$(TARGET): $(OBJS)
	@ echo [Linking...]
	@ $(CC) $(OBJS) $(CFLAGS) $(LIBS) -o $@
	@ echo done

$(OBJS): %.o: %.c
	@ echo [Compile] $<
	@ $(CC) -MM $(CFLAGS) -MT"$*.d" -MT"$(<:.c=.o)" $< > $*.d
	@ $(CC) $(CFLAGS) -c $< -o $@

sinclude $(DEPS)

install:
	cp $(TARGET) /usr/local/bin

uninstall:
	rm /usr/local/bin/$(TARGET)

clean:
	@ echo [Clean...]
	@ $(RM) $(OBJS) $(DEPS)
	@ echo done
