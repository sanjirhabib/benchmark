#pragma once
#include "var.h"
#include "str.h"
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
