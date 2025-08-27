#pragma once
#include <assert.h>
#include <errno.h>
#include "map.h"
#include "struct.h"
#include "ids.h"
struct s_log {
	vector lines;
	vector types;
	int is_error;
};
extern struct s_log logs;

vector ring_add(vector in,vector add,int size);
void dx(var in);
int app_debug(char* format,...);
int app_error(char* msg,...);
int app_print(char* format,...);
void dump(var in);
void map_dump(map in);
void _dump(var in,int indent);
void _vec_dump(vector in, int indent);
void _map_dump(map m, int indent);
string bin_dump(string in);
void pair_dump(pair v,int indent);
void dlib_dump(dlib in);
