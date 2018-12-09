CFLAGS	 = -Wall -Wextra -std=c89 -O2
TARGET	 = berry
CC	 = gcc

INCPATH	 = src
SRCPATH	 = src

ifeq ($(@shell uname), Linux)
LIBS	 = -lreadline
endif

SRCS	 = $(foreach dir, $(SRCPATH), $(wildcard $(dir)/*.c))
OBJS	 = $(patsubst %.c, %.o, $(SRCS))
DEPS	 = $(patsubst %.c, %.d, $(SRCS))
CFLAGS	+= $(foreach dir, $(INCPATH), -I"$(dir)")
CFLAGS	+= $(LIBS)

all: $(TARGET)

$(TARGET): $(OBJS)
	@ echo [Linking...]
	@ $(CC) $(OBJS) $(CFLAGS) -o $@
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
