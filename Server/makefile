# makefile

all: psiskv_server

psiskv_server: psiskv_server_lib.c psiskv_server.c psiskv_list.c
	gcc -g -pthread -Wall psiskv_server_lib.c psiskv_server.c psiskv_list.c -o psiskv_server
