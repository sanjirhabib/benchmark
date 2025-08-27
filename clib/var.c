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

#include "var.h"


//enum id
static char* ids_str[]={
	"Null",//0
	"String",
	"Char",
	"Int",
	"Var",
	"Bool",
	"Pointer",
	"Double",
	"Float",
	"Map",
	"Range",
	"MapIndex",
	"Vector",
	"StrPair",
	"Terminator",
	"Field",
	"Error",
	"CharPtr",
	"ConstCharPtr",
	"Void",
	"VoidPtr",
	"YMD",
	"YMD2",
	"SQLType",
	"LiteRS",
	"Lite",
	"DLib",
	"Rows",
	"Member",
	"MetaType",
	"Long",
	"TimeT",
	"ID"
};

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



struct meta_type meta[256];

//end
/*header

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

end*/


void pair_free(void* val){
	pair* v=val;
	ptr_free(&v->head);
	ptr_free(&v->tail);
}

int c_len(char* in){
	return in ? strlen(in) : 0;
}
int s_ends(string in,char* by){
	return ends_s(in,c_(by));
}
int ends_s(string in,string by){
	return eq_s(sub(in,End-by.len,End),by);
}
int start_s(string in,string subs){
	return in.len>=subs.len && eq_s(sub(in,0,subs.len),subs);
}
int s_start(string in,char* sub){
	return start_s(in,c_(sub));
}
var type_(id in){
	return (var){.type=in};
}
//var oldp_(var* in){
//	return in ? ro(*in) : NullStr;
//}
int s_out(string in){
	for(int i=0; i<in.len; i++){
		char c=in.str[i];
		switch(c){
			case '\t': printf("%*s",4,""); break;
			default: putchar(c); break;
		}
	}
	ptr_free(&in);
	return 0;
}
void* _p(var* in,id type){
	if(!in) return NULL;
	if(meta[type].embed || meta[type].isvar) return in;
	return in->ptr;
}
var p_(void* in,id type){
	if(!in) return VarEnd;
	if(meta[type].isvar) return *(var*)in;
	if(meta[type].embed){
		var ret={.type=type};
		memcpy(&ret,in,meta[type].datasize);
		return ret;
	}
	return (var){.ptr=in, .type=type};
}
var p_ro(void* in,id type){
	return ro(p_(in,type));
}
int s_i(string in){
	if(in.len>32||!in.len) return 0;
	char temp[33]={0};
	memcpy(temp,in.str,in.len);
	ptr_free(&in);
	return atoi(temp);
}
int _i(var in){
	switch(in.type){
		case Null : return 0;
		case Int : return in.i;
		case Double : return in.f;
		case Float : return in.f;
		case Long : return in.i;
		case Char : return in.len ? s_i(in) :  in.i;
	}
	return 0;
}
int p_i(var* in){
	return in ? _i(*in) : 0;
}
string i_s(long long in){
	char ret[21]={0};
	int at=sizeof(ret)-2;
	long long temp=in<0 ? in*-1 : in;
	do{ ret[at--]='0'+temp%10; } while ((temp/=10));
	if(in<0) ret[at--]='-';
	return copy(c_(ret+at+1));
}
void ptr_free(void* val){
	var* in=val;
	if(in->readonly) return;
	if(in->ptr) allocator(in->ptr,0);
}

void struct_free(void*,id);

void vfree(void* val){
	var* in=val;
	if(in->locked) return;
	if(!in->ptr) return;
	void(*free)(void*)=meta[in->type].free;
	int len=in->len;
	if(!len) len=1;
	if(free){
		int size=datasize(*in);
		for(int i=0; i<len; i++){
			free(in->str+size*i);
		}
	}
	else if(meta[in->type].member.len){
		for(int i=0; i<len; i++){
			struct_free(in->str+datasize(*in)*i, in->type);
		}
	}
	if(in->readonly) return;
	if(meta[in->type].resource) return;
	if(meta[in->type].embed) return;
	if(meta[in->type].isvar && !in->len) return;
//	if(!in->readonly && (!meta[in->type].embed || meta[in->type].isvar && !meta[in->type].resource))
	allocator(in->ptr,0);
	in->len=0;
	in->ptr=NULL;
}
var i_(int val){
	return (var){ .i=val, .type=Int };
}
var f_(double val){
	return (var){ .f=val, .type=Double };
}
var rw(var in){
	return in.readonly ? copy(in) : in;
}
var ro(var in){
	in.readonly=1;
	return in;
}
var unlock(var in){
	in.locked=1;
	return in;
}
var lock(var in){
	in.locked=1;
	return in;
}
var own(var* in){
	if(!in) return null;
	var ret=*in;
	in->readonly=1;
	return ret;
}
//var move(var* in){
//	var ret=*in;
//	*in=null;
//	return ret;
//}
//string vown(string in){
//	return rw(in);
//}
int c_eq(char* in1,char* in2,int len){
	for(int i=0; i<len; i++)
		if(tolower(in1[i])!=tolower(in2[i])) return 0;
	return 1;
}
int eq_s(var in1, var in2){
	if(in1.len<=0) return in1.ptr==in2.ptr;
	if(in1.len!=in2.len) return 0;
	return c_eq(in1.str,in2.str,in1.len);
}
string s_new(void* str,int len){
	string ret=NullStr;
	ret=resize(ret,len);
	if(str && len) memcpy(ret.str,str,len);
	return ret;
}
void* mem_resize(void* in,int len,int oldlen){
	void* ret=NULL;
	if(!len){
		if(in) allocator(in,0);
		return NULL;
	}
	if(!in){
		ret=memset(allocator(NULL,len),0,len);
		assert(ret); //usleep(10*1000);
		return ret;
	}
	if(len<=oldlen) return in;

	ret=allocator(in,len);
	if(ret){
		memset((char*)ret+oldlen,0,len-oldlen);
		return ret;
	}
	assert(ret);
	// realloc can fail in php
	ret=memset(allocator(NULL,len),0,len);
	assert(ret); //usleep(10*1000);
	memcpy(ret,in,oldlen);
	memset((char*)ret+oldlen,0,len-oldlen);
	allocator(in,0);

	return ret;
}
void* grow(string* in,int len){
	int old=in->len;
	*in=resize(*in,old+len);
	return in->str+old*datasize(*in);
}
string resize(string in,int len){
	if(!len){
		ptr_free(&in);
		return (string){.type=in.type};
	}
	if(in.len==len) return in;
	if(len<in.len){
		in.len=len;
		return in;
	}
	int datasize=datasize(in);
	assert(datasize);
	void* ret=NULL;
	if(in.readonly){
		ret=mem_resize(NULL,len*datasize,in.len*datasize);
		memcpy(ret,in.str,in.len);
	}
	else
		ret=mem_resize(in.str,len*datasize,in.len*datasize);
	return (string){
		.ptr=ret,
		.len=len,
		.type=in.type,
	};
}
string cat(string str1,string str2){
	if(!str2.len) return str1;
	if(!str1.len) return str2;

	assert(datasize(str2)==datasize(str2));

	if(str1.str+str1.len*datasize(str1)==str2.str){
 		//when string was previously split, ptrs are side by side
		str1.len+=str2.len;
		return str1;
	}
	void* ptr=grow(&str1,str2.len);
	memcpy(ptr,str2.str,str2.len*datasize(str1));
	ptr_free(&str2);
	return str1;
}
void pair_sub(pair* in,int start,int len){
	if(!len) len=in->tail.len-start;
	in->tail.str+=start;
	in->tail.len-=len;
}
string s_c(string in){
	if(!in.len) return in;
	if(!in.str[in.len-1]) return in;
	add_type(char,in)='\0';
	return in;
}
string head(string in, int len){
	return sub(in,0,len);
}
string tail(string in, int len){
	return sub(in,len,in.len-len);
}
string sub(string in, int from, int len){
	if(!in.len) return in;
	if(from<0) from=in.len+from+1;
	if(from<0) from=0;
	if(len<0) len=in.len-from+len+1;
	if(len<0) return (var){.type=in.type};
	if(from+len>in.len) len=in.len-from;
	if(len<0) return (var){.type=in.type};
	return (string){
		.str=in.str+from*datasize(in),
		.len=len,
		.type=in.type,
		.readonly=1,
	};
}
int s_caseeq(string str1,string str2){
	if(str1.len!=str2.len) return 0;	
	for(int i=0; i<str1.len; i++)
		if(str1.str[i]!=str2.str[i]) return 0;
	return 1;
}
int eq(string str1,char* str2){
	if(!str2) return 0;
	int len=strlen(str2);
	if(len!=str1.len) return 0;
	return c_eq(str2,str1.str,len);
}
pair split_any(string in,char* any){
	pair ret={.head=in};
	int i=0;
	while(i<in.len && !strchr(any,in.str[i])) i++;
	ret.head.len=i;
	while(i<in.len && strchr(any,in.str[i])) i++;
	ret.tail=sub(in,i,in.len);
	return ret;
}
string next(string in,char* sep,string r){
	if(!in.len) return VarEnd;
	int len=sep ? strlen(sep) : 0;
	if(!len){
		return r.len ? VarEnd : in;
	}
	if(!r.str){
		r.str=in.str;
		r.readonly=1;
	}
	else
		r.str+=r.len+len;
	int rest=in.str+in.len-r.str;

	if(!rest){
		r.len=0;
		return r;
	}
	if(rest<0){
		return VarEnd;
	}

	char* end=in.ptr+in.len;
	char* ptr=r.str;
	while(ptr<=end-len && memcmp(ptr,sep,len)!=0) ptr++;

	if(ptr>end-len){ //whole string matched.
		r.len=in.len-(r.str-in.str);
		return r;
	}

	r.len=ptr-r.str;
	return r;
}
pair cut(string in,char* by){
	string s=next(in,by,NullStr);
	if(s.type==Terminator) return (pair){0};
	pair ret={.head=s};
	s=next(in,by,s);
	if(s.type==Terminator) s=NullStr;
	else{
		s.len=in.len-(s.str-in.str);
	}
	ret.tail=s;
	return ret;
}
int buff_add(var in,var add,int from){
	if(!add.len) return from;
	assert(datasize(in)==datasize(add));
	assert(add.len+from<=in.len);
	memcpy(in.ptr+from*datasize(in),add.ptr,add.len*datasize(add));
	ptr_free(&add);
	return from+add.len;
}
string _s(var in){
	if(in.len>=0) return in;
	string ret=s_new(NULL,_c(in,NULL));
	_c(in,ret.str);
	return ret;
}
string cl_(const void* in,int len){
	return in && len ? (string){
		.str=(void*)in,
		.len=len,
		.type=Char,
		.readonly=1,
	} : NullStr;
}
var bool_(int in){
	return (var){
		.i=in,
		.type=Bool,
	};
}
var cp_(char* in){
	return (var){
		.ptr=in,
		.len=strlen(in)+1,
		.type=CharPtr
	};
}
string c_(char* in){
	return cl_(in,in ? strlen(in) : 0);
}
int _c(var in,char* out){
	switch(in.type){
		case Char :
			if(out) memcpy(out,in.str,in.len);
			return in.len;
		case Pointer : if(out) memcpy(out,"<ptr>",6); return 5;
		case Terminator : if(out) memcpy(out,"<end>",6); return 5;
		case Int :
			int len=snprintf(0,0,"%d",in.i);
			if(!out) return len;
			char buff[32];
			snprintf(buff,sizeof(buff),"%d",in.i);
			memcpy(out,buff,len);
			return len;
		case Float :
		case Double :
			len=snprintf(0,0,"%g",in.f);
			if(!out) return len;
			snprintf(buff,sizeof(buff),"%g",in.f);
			memcpy(out,buff,len);
			return len;
		default : return 0;
	}
}
string cat_c(string in,char* add){
	return cat(in,c_(add));
}
int _len(var in){
	return _c(in,NULL);
}
string _cat_all(string in,...){
	va_list args;
	va_list args2;
	va_start(args,in);
	va_copy(args2,args);
	int len=_len(in);
	while(1){
		var sub=va_arg(args,var);
		if(sub.type==Terminator) break;
		len+=_len(sub);
	}
	va_end(args);

	string ret=s_new(NULL,len);
	int off=buff_add(ret,in,0);
	while(1){
		var sub=va_arg(args2,var);
		if(sub.type==Terminator) break;
		off=buff_add(ret,sub,off);
	}
	va_end(args2);
	return ret;
}
var copy(var in){
	var ret=in;
	ret.readonly=0;
	if(!in.len) return ret;
	ret.ptr=memcpy(mem_resize(NULL,ret.len*datasize(ret),0),in.str,ret.len*datasize(ret));
	return ret;
}
void die(char* msg){
	fprintf(stderr,"%s",msg);
	exit(-1);
}
string chop(string in,int len){
	if(len>=in.len){
		ptr_free(&in);
		return (string){0};
	}
	memmove(in.str,in.str+len*datasize(in),(in.len-len)*datasize(in));
	in.len-=len;
	return in;
}
void ids_add(char** in, int len, int first_id);
void reg_var(){
	ids_add(ids_str,sizeof(ids_str)/sizeof(char*),Null);
}
