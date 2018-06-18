

default: vm bcc

vm: vm.c
	clang -O3 -Wall -o vm vm.c

bcc: bcc.c
	clang -O3 -Wall -o bcc bcc.c
