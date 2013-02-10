BUILD=./build/
all:deps $(BUILD)test_netev
deps:
	cd ./wodcontainer && $(MAKE) all
	cd ./wodevent && $(MAKE) all
	cd ./wodnet && $(MAKE) all
	
install:
	mkdir -p /usr/include/wod
	cp ./wodnet/include/* ./wodevent/include/* ./wodcontainer/include/* /usr/include/wod
	ln -s -f ./build/libwnet.so /lib/libwnet.so
	ln -s -f ./build/libwct.so /lib/libwct.so
	ln -s -f ./build/libwev.so /lib/libwev.so
clean:
	rm $(BUILD)*
$(BUILD)test_netev:./test/test_netev.c
	gcc -o $@ $^ $(BUILD)libwev.so  $(BUILD)libwnet.so 
