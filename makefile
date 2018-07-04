.PHONY: clean test
warnings = -Weverything -Werror
default: vm irc astc parser

vm: vm.c
	clang -O3 $(warnings) -o vm vm.c

irc: irc.c
	clang -O3 $(warnings) -o irc irc.c

astc: astc.c
	clang -O3 $(warnings) -o astc astc.c

parser: parser.c
	clang -O3 $(warnings) -o parser parser.c


test0.bc: test0.ir irc
	irc test0.ir

test: vm test0.bc
	vm test0.bc


clean:
	rm -f parser irc astc vm test0.bc
