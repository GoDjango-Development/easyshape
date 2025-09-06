cflags = -I ./include
libs =

#Release profile

obj = release/obj/main.o release/obj/eshp.o release/obj/eshpcfg.o \
	release/obj/eshperr.o release/obj/eshptc.o

hdr = include/eshp.h include/eshpcfg.h include/eshperr.h include/eshptc.h

conf_file = config/example.conf

release_bin = release/eshp
CCX = gcc -o
CC = gcc -c
release_command = $(CCX) $(release_bin) $(obj) $(ldflags) $(libs)

release: $(obj) $(hdr)
	$(release_command)

release/obj/main.o: src/main.c
	$(CC) src/main.c -o release/obj/main.o $(cflags) 

release/obj/eshp.o: src/eshp.c include/eshp.h
	$(CC) src/eshp.c -o release/obj/eshp.o $(cflags)

release/obj/eshpcfg.o: src/eshpcfg.c include/eshpcfg.h
	$(CC) src/eshpcfg.c -o release/obj/eshpcfg.o $(cflags)

release/obj/eshperr.o: src/eshperr.c include/eshperr.h
	$(CC) src/eshperr.c -o release/obj/eshperr.o $(cflags)

release/obj/eshptc.o: src/eshptc.c include/eshptc.h
	$(CC) src/eshptc.c -o release/obj/eshptc.o $(cflags)

run: release
ifneq ("$(wildcard $(release_bin))","")
	$(release_bin) $(conf_file)
else
	$(release_command)
	$(release_bin) $(conf_file)
endif

.PHONY: rebuild

rebuild: 
	$(MAKE) clean
	$(MAKE) release
	
# Debug profile

dbg = debug/obj/main.o debug/obj/eshp.o debug/obj/eshpcfg.o \
	debug/obj/eshperr.o
 
debug_bin = debug/eshp
CCGX = gcc -g -DDEBUG -o
CCG = gcc -g -DDEBUG -c
debug_command = $(CCGX) $(debug_bin) $(dbg) $(ldflags) $(libs)
	
debug: $(dbg) $(hdr)
	$(debug_command)

debug/obj/main.o: src/main.c
	$(CCG) src/main.c -o debug/obj/main.o $(cflags) 

debug/obj/eshp.o: src/eshp.c include/eshp.h
	$(CCG) src/eshp.c -o debug/obj/eshp.o $(cflags)

debug/obj/eshp.o: src/eshpcfg.c include/eshpcfg.h
	$(CCG) src/eshpcfg.c -o debug/obj/eshpcfg.o $(cflags)

debug/obj/eshperr.o: src/eshperr.c include/eshperr.h
	$(CCG) src/eshperr.c -o debug/obj/eshperr.o $(cflags)

debug/obj/eshptc.o: src/eshptc.c include/eshptc.h
	$(CCG) src/eshptc.c -o debug/obj/eshptc.o $(cflags)

run_debug: debug
ifneq ("$(wildcard $(debug_bin))","")
	gdb $(debug_bin)
else
	$(debug_command)
	gdb $(debug_bin)
endif

.PHONY: rebuild_debug
	
rebuild_debug: 
	$(MAKE) clean
	$(MAKE) debug

#Others rules

.PHONY: prepare clean

prepare:
	mkdir -p debug release debug/obj release/obj
	mkdir -p src include
clean:
	rm -R -f $(obj) $(dbg) release/obj/* debug/obj/* 
	rm -R -f $(release_bin) $(debug_bin)

