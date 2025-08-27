#pragma once
//file:mem.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <execinfo.h>
#define POOL_SIZE (128 * 1024 * 1024)  // 128MB
#define MIN_SIZE 16                     // 16 bytes minimum
#define MAX_ORDER 23                    // log2(128MB/16) = 23

typedef struct block {
    size_t next:28;
    size_t prev:28;
    size_t order:5;
    size_t is_free:1;
} block_t;

struct arena {
	block_t* free_list[MAX_ORDER + 1];
	size_t total_allocator;
	size_t allocator_count;
	char* memory_pool;
	char cache[2*1024*1024];
};

extern void*(*allocator)(void* ptr, size_t len);
extern struct arena* arena;

struct block* block_next(struct block* in);
struct block* block_prev(struct block* in);
int block_no(struct block* in);
void buddy_init();
void buddy_check_leaks() ;
void buddy_close();
void* buddy_alloc(void* ptr, size_t len);
void* os_alloc(void* ptr, size_t len);
//file:var.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <memory.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <stddef.h>
#include <sys/stat.h>
#include <unistd.h>
typedef enum {
	Null=0,
	String,
	Char,
	Int,
	Var,
	Bool,
	Pointer,
	Double,
	Float,
	Map,
	Range,
	MapIndex,
	Vector,
	StrPair,
	Terminator,
	Field,
	Error,
	CharPtr,
	ConstCharPtr,
	Void,
	VoidPtr,
	YMD,
	YMD2,
	SQLType,
	LiteRS,
	Lite,
	DLib,
	Rows,
	Member,
	MetaType,
	Long,
	TimeT,
	ID,
}id;
#define ENUM0
#define End (-1)
#define VarEnd (var){.type=Terminator}
#define ls(x) (int)(x).len, (x).str
#define cat_all(ret, ...) _cat_all(ret, ##__VA_ARGS__,VarEnd)
#define min(x,y) (x<y?x:y)
#define max(x,y) (x>y?x:y)
#define between(x,y,z) ((y)<(x) ? (x) : ((y)>(z) ? (z) : (y))) 
#define null (var){0}
#define NullStr (var){.type=Char}
#define VarError (var){.type=Error}
#define NullPair (pair){.head=NullStr, .tail=NullStr}
#define each(x,y,z) z=x.ptr; for(int y=0; y<x.len; y++)
#define add(x) *(var*)grow(&x,1)
#define add_type(x,y) *(x*)grow(&y,1)
#define cs_(x) cl_(x,x ? sizeof(x)-1 : 0)
#define datasize(x) (meta[(x).type].datasize)
#define to_heap(x) memcpy(allocator(NULL,sizeof(x)),&x,sizeof(x))
#define array_copy(des,src) memcpy(des,src,sizeof(des));
#define valp(type,v) _Generic(type, var: &v, default: sizeof(type)<=sizeof(void*) ? (type*)&v : (type*)(v.ptr))
#define val(type,v) _Generic(type, var: v, default: sizeof(type)<=sizeof(void*) ? ((union { void* ptr; type vv; }){.ptr = v.ptr}.vv) : *(type*)(v.ptr))

extern void*(*allocator)(void* ptr, size_t len);
typedef struct s_int2 {
	int x;
	int y;
} int2;
typedef struct s_var {
	union {
		int i;
		double f;
		void* ptr;
		char* str;
		int* no;
		long long ll;
		struct s_var* var;
	};
	unsigned long long len : 47;
	unsigned long long readonly : 1;
	unsigned long long locked : 1;
	id type : 15;
} var;

typedef var string;
typedef var vector;

typedef struct meta_type {
	vector member;
	void(*free)(void* in);
	void(*dump)(void* in);
	int datasize;
	unsigned int array:1;
	unsigned int embed:1;
	unsigned int resource:1;
	unsigned int isvar:1;
} meta_type;

struct member {
	char* name;
	int type;
	int offset;
};


extern struct meta_type meta[256];

typedef struct s_pair {
	string head;
	string tail;
} pair;

typedef struct s_range {
	int from;
	int len;
} range;

void pair_free(void* val);
int c_len(char* in);
int s_ends(string in,char* by);
int ends_s(string in,string by);
int start_s(string in,string subs);
int s_start(string in,char* sub);
var type_(id in);
int s_out(string in);
void* _p(var* in,id type);
var p_(void* in,id type);
var p_ro(void* in,id type);
int s_i(string in);
int _i(var in);
int p_i(var* in);
string i_s(long long in);
void ptr_free(void* val);
void vfree(void* val);
var i_(int val);
var f_(double val);
var rw(var in);
var ro(var in);
var unlock(var in);
var lock(var in);
var own(var* in);
int c_eq(char* in1,char* in2,int len);
int eq_s(var in1, var in2);
string s_new(void* str,int len);
void* mem_resize(void* in,int len,int oldlen);
void* grow(string* in,int len);
string resize(string in,int len);
string cat(string str1,string str2);
void pair_sub(pair* in,int start,int len);
string s_c(string in);
string head(string in, int len);
string tail(string in, int len);
string sub(string in, int from, int len);
int s_caseeq(string str1,string str2);
int eq(string str1,char* str2);
pair split_any(string in,char* any);
string next(string in,char* sep,string r);
pair cut(string in,char* by);
int buff_add(var in,var add,int from);
string _s(var in);
string cl_(const void* in,int len);
var bool_(int in);
var cp_(char* in);
string c_(char* in);
int _c(var in,char* out);
string cat_c(string in,char* add);
int _len(var in);
string _cat_all(string in,...);
var copy(var in);
void die(char* msg);
string chop(string in,int len);
void reg_var();
//file:str.c
#include <string.h>
#include <stdarg.h>
#include <assert.h>
string dequote(string in,char* qchars);
string unescape_ex(string in,string find, string replace);
string unescape(string in);//api
string escape(string in);//api
string escape_ex(string in,string find,string replace);
int s_chr(string in,char has);
char* s_has(string in,char* has);
int is_word(string word,char* list);
string s_join(string in,char* terminator,string add);
string format(char* fmt,...);
string valist_s(int readonly,char* fmt,va_list args);
string c_repeat(char* in, int times);
string sub_a(string in, string sub);
string sub_bc(string in, string sub);
string s_upper(string in);
string s_lower(string in);
void s_catchar(string* in,char c);
string c_nullterm(char* in);
char* s_cptr(string in);
int char_count(string in,char c);
string s_insert(string in,int offset,string by);
string trim_ex(string in, char* chars);
string trim(string in);
int c_is(char c, char* chars);
int s_is(string in, char* chars);
string head_add(string in, int add);
//file:vector.c
#define vec_all(ret, ...) _vec_all(ret, ##__VA_ARGS__,VarEnd)
#define vec_append(ret, ...) _vec_append(ret, ##__VA_ARGS__,VarEnd)
#define c_vec(ret, ...) _c_vec(ret, ##__VA_ARGS__,NULL)
#define NullVec (var){.type=String}
#define set(vector,index) (*(var*)getp(vector,index))
#define get_type(type,vector,index) (*(type*)getp(vector,index))
#define array_vec(x,y) _array_vec(x,y,sizeof(y)/meta[x].datasize)
var get(vector in,int index);
void* getp(vector in,int index);
vector vec_del(vector in,int idx);
vector vec_disown(vector in);
vector vec_own(vector in);
vector splice(vector in,vector old,vector by);
vector vec_new(id type,int len);
vector _c_vec(char* in, ...);
vector _array_vec(id type,void* in,int len);
vector vec_dup(vector in);
vector split(string in,char* by);
vector s_vec_ex(string in,char* by,int limit);
string join(vector in,char* sep);
int vec_strlen(vector in);
vector _vec_append(string ret, ...);
vector _vec_all(string in,...);
vector vec_unescape(vector in);
vector vec_escape(vector in);
void vec_freeable(vector* v,string in);
string replace(string in, char* from, char* into);//api
string replace_vec(string in,vector from,vector into);//api
string code_name(string in);//api
//file:map.c
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
//file:struct.c
void struct_free(void* ptr,id type);
var s_type(var in, id type);
string struct_s(void* in, id type);
void* s_struct(string in,id type,void* ret);
void* map_struct(map in, void* ret,id type);
void* member_ptr(id type, void* ptr, int memberno);
void* vec_struct(vector in, void* ret, id type);
vector struct_vec(void* in, id type);
map struct_map(void* in, id type);
//file:file.c
#include <wordexp.h>
#include <dlfcn.h>
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
//file:ids.c
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
//file:log.c
#include <assert.h>
#include <errno.h>
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
