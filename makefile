BUILD=./build/
INC_DIR=-I./include
LIB_DIR=-L./build
CFLAGS=-Wall -O
SHARED=-fPIC -shared
CC = gcc
all : $(BUILD)libwod.so $(BUILD)test_netev $(BUILD)test_echo $(BUILD)test_array $(BUILD)test_hashmap $(BUILD)test_cli

install:
	cp ./include/*  /usr/include
	cp $(BUILD)libwod.so /lib64
clean:
	rm $(BUILD)*


SRCS=./container/wod_array.c \
				./container/wod_cyclebuffer.c \
				./container/wod_hashmap.c \
				./container/wod_queue.c \
				./event/wod_event.c \
				./net/wod_net_unix.c\
				./time/wod_time.c\
				./sys/wod_sys.c\
				./thread/unix/wod_thread.c\
				./thread/unix/wod_mutex_cond.c\
				./thread/unix/wod_rwlock.c
$(BUILD)libwod.so : $(SRCS)
			$(CC) $(CFLAGS) $(INC_DIR) $(SHARED) -o $@ $^  -lpthread
			
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
