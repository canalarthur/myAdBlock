all:
	gcc src/serverTCP.c -pthread -I include/ -o proxy

clean:
	rm -rf proxy
