#pragma once
#include "mem.h"
#include "var.h"
#include "str.h"
#include "vector.h"
#include "map.h"
#include "file.h"
#define ids_init(x,y) ids_add(x,sizeof(x)/sizeof(char*),y)
extern vector cache;
string id_s(id in);//api
id s_id(string name);//api
var id_(id in);
void ids_add(char** in, int len, int first_id);
void meta_free(void* ptr);
void null_free(void*);
void reg_cvar();
void reg_free();
void globals_get(struct globals* g);
void globals_set(struct globals* g);
