CC = gcc
BUILD= ../build/
CCFLAGS = -I./include -g -Wall -g3
INCLUDE_DIR = -I./include
SHARED = -fPIC -shared
WDNET_SRCS= ./src/ev.c

all:$(BUILD)libwev.so 
$(BUILD)libwev.so: $(WDNET_SRCS)
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) $(SHARED) -o $@ $^ 
clean:
	rm $(BUILD)*
