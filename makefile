BUILD=./build/
all:deps $(BUILD)test_netev
deps:
	cd ./wodcontainer && $(MAKE) all
	cd ./wodevent && $(MAKE) all
	cd ./wodnet && $(MAKE) all
	
install:
	mkdir -p /usr/include/wod
	cp ./wodnet/include/* ./wodevent/include/* ./wodcontainer/include/* /usr/include/wod
	ln -s  ./build/libwnet.so /lib/libwnet.so.0.0.1
	ln -s  ./build/libwct.so /lib/libwct.so.0.0.1
	ln -s  ./build/libwev.so /lib/libwev.so.0.0.1
clean:
	rm $(BUILD)*
$(BUILD)test_netev:./test/test_netev.c
	gcc -o $@ $^ $(BUILD)libwev.so  $(BUILD)libwnet.so 
