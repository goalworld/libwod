BUILD=./build/
INC_DIR=-I./include
LIB_DIR=-L./build
SHARED=-fPIC -shared
CC = gcc
all : $(BUILD)libwod.so $(BUILD)test_netev $(BUILD)test_echo $(BUILD)test_array $(BUILD)test_hashmap $(BUILD)test_cli

install:
	cp ./include/*  /usr/include
	cp $(BUILD)libwod.so /lib64
clean:
	rm $(BUILD)*


SRCS=./container/array.c \
				./container/cyclebuffer.c \
				./container/hashmap.c \
				./container/queue.c \
				./event/ev.c \
				./net/net_unix.c
$(BUILD)libwod.so : $(SRCS)
			$(CC) $(CFLAGS) $(INC_DIR) $(SHARED) -o $@ $^ 
			
$(BUILD)test_netev:./test/netev_test.c
			$(CC) $(CFLAGS) $(INC_DIR) -o $@ $^ -lwod $(LIB_DIR)
$(BUILD)test_echo:  ./test/net_test.c
			$(CC) $(CFLAGS) $(INC_DIR) -o $@ $^ -lwod $(LIB_DIR)
$(BUILD)test_cli:  ./test/net_cli_test.c
			$(CC) $(CFLAGS) $(INC_DIR) -o $@ $^ -lwod $(LIB_DIR)
$(BUILD)test_array:  ./test/array_test.c
			$(CC) $(CFLAGS) $(INC_DIR) -o $@ $^ -lwod $(LIB_DIR)
$(BUILD)test_hashmap: ./test/hashmap_test.c
			$(CC) $(CFLAGS) $(INC_DIR) -o $@ $^ -lwod $(LIB_DIR)
