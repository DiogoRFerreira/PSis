# makefile

all: psiskv_client psiskv_server cli-exe-par-1 cli-exe-1

psiskv_client: psiskv_client.c psiskv_lib.c
	gcc -g -Wall psiskv_client.c psiskv_lib.c -o psiskv_client

psiskv_server: psiskv_server_lib.c psiskv_server.c psiskv_list.c
	gcc -g -pthread -Wall psiskv_server_lib.c psiskv_server.c psiskv_list.c -o psiskv_server

cli-exe-par-1: cli-exe-par-1.c psiskv_lib.c
	gcc -g -Wall cli-exe-par-1.c psiskv_lib.c -o  cli-exe-par-1
	
cli-exe-1: cli-exe-1.c psiskv_lib.c
	gcc -g -Wall cli-exe-1.c psiskv_lib.c -o  cli-exe-1