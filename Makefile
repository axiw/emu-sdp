all:
	gcc -g -O2 *.c -o fuzzer

clean:
	rm -rf fuzzer
