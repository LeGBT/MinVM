.PHONY: clean test

default: vm ir2bc ast2bc

vm: vm.c
	clang -O3 -Wall -o vm vm.c

ir2bc: ir2bc.c
	clang -O3 -Wall -o ir2bc ir2bc.c

ast2bc: ast2bc.c
	clang -O3 -Wall -o ast2bc ast2bc.c


test0.bc: test0.ir ir2bc
	ir2bc test0.ir

test: vm test0.bc
	vm test0.bc


clean:
	rm -f ir2bc ast2bc vm test0.bc
