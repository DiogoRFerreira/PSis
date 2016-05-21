# makefile

all: psiskv_client cli-exe-par-1 cli-exe-1 test1 test2 test3

psiskv_client: psiskv_client.c psiskv_lib.c
	gcc -g -Wall psiskv_client.c psiskv_lib.c -o psiskv_client

cli-exe-par-1: cli-exe-par-1.c psiskv_lib.c
	gcc -g -Wall cli-exe-par-1.c psiskv_lib.c -o  cli-exe-par-1
	
cli-exe-1: cli-exe-1.c psiskv_lib.c
	gcc -g -Wall cli-exe-1.c psiskv_lib.c -o  cli-exe-1

test1: test1.c psiskv_lib.c
	gcc -g -Wall test1.c psiskv_lib.c -o test1

test2: test2.c psiskv_lib.c
	gcc -g -Wall test2.c psiskv_lib.c -o test2

test3: test3.c psiskv_lib.c
	gcc -g -Wall test3.c psiskv_lib.c -o test3