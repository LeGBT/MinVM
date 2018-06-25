.PHONY: clean test

default: vm irc astc

vm: vm.c
	clang -O3 -Wall -o vm vm.c

irc: irc.c
	clang -O3 -Wall -o irc irc.c

astc: astc.c
	clang -O3 -Wall -o astc astc.c


test0.bc: test0.ir irc
	irc test0.ir

test: vm test0.bc
	vm test0.bc


clean:
	rm -f irc astc vm test0.bc
