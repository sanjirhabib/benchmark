#pragma once
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
