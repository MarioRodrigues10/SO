all: folders server client

server: bin/monitor

client: bin/tracer

folders:
	@mkdir -p src obj bin tmp

bin/monitor: obj/monitor.o obj/utils.o
	gcc -g obj/monitor.o obj/utils.o -o bin/monitor

obj/monitor.o: src/monitor.c
	gcc -Wall -g -c src/monitor.c -o obj/monitor.o

bin/tracer: obj/tracer.o obj/utils.o
	gcc -g obj/tracer.o obj/utils.o -o bin/tracer

obj/tracer.o: src/tracer.c
	gcc -Wall -g -c src/tracer.c -o obj/tracer.o

obj/utils.o: src/utils.c
	gcc -Wall -g -c src/utils.c -o obj/utils.o

clean:
	rm -f obj/* tmp/* bin/{tracer,monitor} PIDS/*

