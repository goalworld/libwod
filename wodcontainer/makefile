CC = gcc

BUILD= ../build/

CCFLAGS = -I./include -g -Wall -g3

INCLUDE_DIR = -I./include

SHARED = -fPIC -shared

SRCS= ./src/array.c \
			./src/queue.c \
			./src/cyclebuffer.c
all:$(BUILD)libwct.so \
	$(BUILD)test_arr

$(BUILD)libwct.so: $(SRCS)
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) $(SHARED) -o $@ $^

$(BUILD)test_arr: ./test/array_test.c
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) -o $@ $^ $(BUILD)libwct.so

clean:
	rm $(BUILD)libwct.so $(BUILD)test_arr
