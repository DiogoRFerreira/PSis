#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED
#include <stdio.h>


typedef struct sockaddr_in sock;

typedef struct _buffer{
	char m_forward[100];
	char username_sender[100];
}buffer_aux;

typedef struct _users{
	int descriptor;
	char*name;
	struct _users*next;
}users;


typedef struct _client_database{
	users u;
	int n_clients;
}client_database;

typedef struct _message{
	char m[100];
}message_;

typedef struct _server_database{
	struct _server_database* next;
	message_ m_vector_20[20];
}server_database;

typedef struct _variables{
	int flag_quit;
	client_database database;
	int n_stored_messages;
	int n_aux_message_storage;
	server_database*head;
	int log_messages;
}variables;

#endif // STRUCTURES_H_INCLUDED
