#pragma once
#include "var.h"
#include "str.h"
#include "vector.h"
#define map_new(ret, ...) _map_new(ret, ##__VA_ARGS__,NULL)
#define mget(x, ...) _mget(x, ##__VA_ARGS__,NULL)
#define map_each(x,y,z) z=x.vals.ptr; for(int y=0; y<x.keys.len; y++)
#define map_add(x,y) *(var*)map_add_ex(&x,c_(y))
#define map_addv(x,y) *(var*)map_add_ex(&x,y)
#define map_add_type(a,b,c) *(a*)map_add_ex(&b,c)
#define map_get_type(x,y,z) (*(x*)map_getp(y,z))
#define map_(x) p_(&x,Map)


typedef struct {
	vector keys;
	vector vals;
	vector index;
} map;
typedef struct {
	long long head;
	long long tail;
} mapindex;

typedef map rows;

#define NullMap (map){.index.type=MapIndex,.keys.type=Var,.vals.type=Var}
map vec_map(vector keys,vector vals);
map map_new_ex(id type);
int index_has(vector index, string key, vector keys);
void* map_getp(map in,var key);
var map_own(map in,char* key);
var cget(map in, char* key);
var map_get(map in,var key);
var _mget(map in, ...);
map* map_delc(map* in, char* key);
map* map_del_idx(map* in, int slot);
map* map_del(map* in, var key);
void* map_add_ex(map* in, var key);
vector keys_index(vector keys);
vector keys_reindex(vector keys, vector index, int from);
map map_ro(map in);
void map_free(void* val);
void* map_p_i(map m,int i);
map _map_new(char* in,...);
int rows_count(rows in);
map rows_row(rows in, int idx);
string rows_tsv(rows in);//api
string map_s(map in,char* sepkey,char* sepval);
string _json(var in);
int hash(string in);
int pow2(int i);
rows tsv_rows(string in);//api
