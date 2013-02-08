CC = gcc

BUILD= ../build/

CCFLAGS = -I./include -g -Wall -g3

INCLUDE_DIR = -I./include

SHARED = -fPIC -shared

SRCS= ./src/array.c \
			./src/queue.c \
			./src/cyclebuffer.c\
			./src/hashmap.c			
all:$(BUILD)libwct.so \
	$(BUILD)test_arr \
	$(BUILD)hashmap_test

$(BUILD)libwct.so: $(SRCS)
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) $(SHARED) -o $@ $^

$(BUILD)test_arr: ./test/array_test.c
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) -o $@ $^ $(BUILD)libwct.so
$(BUILD)hashmap_test: ./test/hashmap_test.c
	$(CC) $(CCFLAGS) $(INCLUDE_DIR) -o $@ $^ $(BUILD)libwct.so

clean:
	rm $(BUILD)libwct.so $(BUILD)test_arr
