CC = gcc
BUILD= ../build/
CCFLAGS = -I./include -g -Wall -g3
INCLUDE_DIR = -I./include
SHARED = -fPIC -shared
WDNET_SRCS= ./src/net_unix.c
TEST_SRCS = ./test/net_test.c
all:$(BUILD)libwnet.so \
	$(BUILD)test_echo
$(BUILD)libwnet.so: $(WDNET_SRCS)
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) $(SHARED) -o $@ $^
	
$(BUILD)test_echo: $(TEST_SRCS)
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) -o $@ $^ $(BUILD)libwnet.so
clean:
	rm $(BUILD)*
