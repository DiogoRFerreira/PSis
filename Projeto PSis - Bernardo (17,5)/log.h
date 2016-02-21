#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void write_log(int type, char* username, char* message, int *number);

void read_log();

void get_lost_log_identifier(int*identifier);

#endif // LOG_H_INCLUDED
