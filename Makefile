switch:
	gcc -o $@ $(wildcard *.c) -I. -g -Wall -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function

clean:
	rm switch -rf

.PHONY:switch clean
