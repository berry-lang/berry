CFLAGS	 = -Wall -Wextra -std=c99 -pedantic-errors -O2
LIBS	 =  -lm
TARGET	 = berry
CC	 = gcc

INCPATH	 = src default
SRCPATH	 = src default

ifeq ($(OS), Windows_NT) # Windows
    CFLAGS += -Wno-format # for "%I64d" warning
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

SRCS	 = $(foreach dir, $(SRCPATH), $(wildcard $(dir)/*.c))
OBJS	 = $(patsubst %.c, %.o, $(SRCS))
DEPS	 = $(patsubst %.c, %.d, $(SRCS))
INCFLAGS = $(foreach dir, $(INCPATH), -I"$(dir)")

all: $(TARGET)

debug: CFLAGS += -O0 -g -DBE_DEBUG
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(MSG) [Linking...]
	$(Q) $(CC) $(OBJS) $(CFLAGS) $(LIBS) -o $@
	$(MSG) done

$(OBJS): %.o: %.c
	$(MSG) [Compile] $<
	$(Q) $(CC) -MM $(CFLAGS) $(INCFLAGS) -MT"$*.d" -MT"$(<:.c=.o)" $< > $*.d
	$(Q) $(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

sinclude $(DEPS)

install:
	cp $(TARGET) /usr/local/bin

uninstall:
	rm /usr/local/bin/$(TARGET)

clean:
	$(MSG) [Clean...]
	$(Q) $(RM) $(OBJS) $(DEPS)
	$(MSG) done
