CC = gcc
BUILD= ../build/
CCFLAGS = -I./include -g -Wall -g3
INCLUDE_DIR = -I./include
SHARED = -fPIC -shared
SRCS= ./src/array.c \
			./src/queue.c \
			./src/cyclebuffer.c
all:$(BUILD)libwodcontainer.so \
	$(BUILD)/test_echo
$(BUILD)libwodcontainer.so: $(SRCS)
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) $(SHARED) -o $@ $^
clean:
	rm $(BUILD)libwodcontainer.so
