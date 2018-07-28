.PHONY: clean test
warnings = -Weverything -Werror
default: vm irc astc parser

oldvm: vm.c
	clang -O3 $(warnings) -o oldvm vm.c

vm: vm2.c
	clang -O3 $(warnings) -o vm vm2.c

irc: irc.c
	clang -O3 $(warnings) -o irc irc.c

parser: parser.c
	clang -O3 $(warnings) -o parser parser.c


test0.bc: irc test0.ir
	./irc test0.ir

test: vm test0.bc
	./vm test0.bc


clean:
	rm -f parser irc astc vm test0.bc
