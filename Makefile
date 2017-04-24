serverTCP:
	gcc src/serverTCP.c -I include/ -o serverTCP


clean:
	rm -rf serverTCP
