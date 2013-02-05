CC = gcc
BUILD= ../build/
CCFLAGS = -I./include -g -Wall -g3
INCLUDE_DIR = -I./include
SHARED = -fPIC -shared
WDNET_SRCS= ./src/ev.c

all:$(BUILD)libwodevent.so 
$(BUILD)libwodevent.so: $(WDNET_SRCS)
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) $(SHARED) -o $@ $^ $(BUILD)libwodcontainer.so
clean:
	rm $(BUILD)*
