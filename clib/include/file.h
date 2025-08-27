#pragma once
#include <wordexp.h>
#include <dlfcn.h>
#include "mem.h"
#include "map.h"
typedef struct {
	void* lib;
	map funcs;
	map(*reflect)();
	var(*invoke)(string,vector);
	var(*release)();
	var(*init)(void*);
} dlib;
extern int(*error)(char* format,...);
extern int(*print)(char* format,...);
extern int(*debug)(char* format,...);

struct globals {
	struct meta_type meta[256];
	map id_name;
	map name_id;
	vector cache;
};
int is_file(char* filename);
int is_dir(char* filename);
int file_size(char* filename);
string path_cat(string path1, string path2);
int fp_write(FILE* fp,string out);
int s_file(string in,char* filename);//api
FILE* file_fp(char* filename,char* rw);
string file_s(char* filename);//api
char* filename_os(string filename);
void lib_close(dlib in);//api
void lib_close_p(void* lib);
dlib lib_conn(char* file);//api
