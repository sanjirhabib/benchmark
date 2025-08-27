#include "var.h"
#include "str.h"
#include "vector.h"

#define vec_all(ret, ...) _vec_all(ret, ##__VA_ARGS__,VarEnd)
#define vec_append(ret, ...) _vec_append(ret, ##__VA_ARGS__,VarEnd)
#define c_vec(ret, ...) _c_vec(ret, ##__VA_ARGS__,NULL)
#define NullVec (var){.type=String}
#define set(vector,index) (*(var*)getp(vector,index))
#define get_type(type,vector,index) (*(type*)getp(vector,index))
#define array_vec(x,y) _array_vec(x,y,sizeof(y)/meta[x].datasize)


//get() ONLY IF item is a var
//get_type() gets map var int
//get() map var int boxed into var
//get() boxed and readonly
//get_p direct pointer to item
var get(vector in,int index){
	void* ptr=getp(in,index);
	if(in.type==Var||in.type==String||in.type==Vector||in.type==CharPtr){
		var ret=*(var*)ptr;
		ret.readonly=1;
	}
	return p_ro(ptr,in.type);
}
void* getp(vector in,int index){
	int size=datasize(in);
	if(!size) return NULL;
	return index>=0 && index<in.len ? in.str+size*index : NULL;
}
//void* vec_last(vector in){
//	return getp(in,in.len-1);
//}
//void* vec_first(vector in){
//	return getp(in,0);
//}
vector vec_del(vector in,int idx){
	return splice(in,sub(in,idx,1),NullVec);
}
vector vec_disown(vector in){
	in=rw(in);
	for(int i=0; i<in.len; i++){
		in.var[i].readonly=1;
	}
	return in;
}
vector vec_own(vector in){
	for(int i=0; i<in.len; i++){
		in.var[i]=rw(in.var[i]);
	}
	return in;
}
vector splice(vector in,vector old,vector by){
	if(!in.len) return by;

	assert(!by.len || datasize(in)==datasize(by));

	int from=old.str ? (old.str-in.str)/datasize(in) : 0;
	assert(from>=0 && from<=in.len);

	int add=by.len;
	int less=old.len;
	int rest=in.len-from-less;
	int extra=add-less;

	if(!in.readonly && meta[in.type].free && less){
		for(int i=0; i<less; i++){
			meta[in.type].free(getp(in,from+i));
		}
	}
	if(rest){
		if(extra>0){
			in=resize(in,in.len+extra);
			memmove(getp(in,from+old.len+extra),getp(in,from+old.len),datasize(in)*(in.len-from-old.len-extra));
		}
		else if(extra<0){
			in=rw(in);
			memmove(getp(in,from+old.len+extra),getp(in,from+old.len),datasize(in)*(in.len-from-old.len));
			in=resize(in,in.len+extra);
		}
	}
	else{
		in=resize(in,in.len+extra);
	}
	if(add){
		in=rw(in);
		memcpy(getp(in,from),by.str,by.len*datasize(in));
		ptr_free(&by);
	}
	return in;
}
vector vec_new(id type,int len){
	vector ret=(vector){.type=type};
	if(len) ret=resize(ret,len);
	return ret;
}
//vector vec_set(vector in,int idx,var val){
//	if(idx<0||idx>=in.len) return in;
//	ptr_free(in.var[idx]);
//	in.var[idx]=val;
//	return in;
//}
vector _c_vec(char* in, ...){
	if(!in) return NullVec;
	va_list args;
	va_list args2;
	va_start(args,in);
	va_copy(args2,args);
	int len=1;
	while(1){
		char* sub=va_arg(args,char*);
		if(!sub) break;
		len++;
	}
	va_end(args);
	vector ret=vec_new(String,len);

	len=0;
	ret.var[len++]=c_(in);
	while(1){
		char* sub=va_arg(args2,char*);
		if(!sub) break;
		ret.var[len++]=c_(sub);
	}
	va_end(args2);
	return ret;
}
vector _array_vec(id type,void* in,int len){
	return (vector){
		.str=in,
		.type=type,
		.len=len,
		.readonly=1
	};
}
vector vec_dup(vector in){
	vector ret=copy(in);
	for(int i=0; i<ret.len; i++){
		ret.var[i]=copy(ret.var[i]);
	}
	return ret;
}
vector split(string in,char* by){
	return s_vec_ex(in,by,0);
}
vector s_vec_ex(string in,char* by,int limit){
	vector ret=NullVec;
	string r=NullStr;
	while((r=next(in,by,r)).type!=Terminator){
		add(ret)=r;
		if(limit && ret.len==limit-1){
			add(ret)=sub(in,r.len+strlen(by)+r.str-in.str,End);
			break;
		}
	}
	return ret;
}
string join(vector in,char* sep){
	if(!in.len) return NullStr;
	string sep1=c_(sep);
	int len=((in.len-1)*sep1.len)+vec_strlen(in);
	string ret=s_new(NULL,len);
	int off=0;
	for(int i=0; i<in.len ; i++){
		if(i) off=buff_add(ret,sep1,off);
		off=buff_add(ret,ro(in.var[i]),off);
	}
	vfree(&in);
	return ret;
}
int vec_strlen(vector in){
	int ret=0;
	for(int i=0; i<in.len; i++) ret+=_c(in.var[i],NULL);
	return ret;
}
vector _vec_append(string ret, ...){
	va_list args;
	va_start(args,ret);
	while(1){
		var val=va_arg(args,var);
		if(val.type==Terminator) break;
		add(ret)=val;
	}
	va_end(args);
	return ret;
}
vector _vec_all(string in,...){
	vector ret=NullVec;
	va_list args;
	va_start(args,in);
	var val=in;
	while(1){
		if(val.type==Terminator) break;
		add(ret)=val;
		val=va_arg(args,var);
	}
	va_end(args);
	return ret;
}
vector vec_unescape(vector in){
	for(int i=0; i<in.len; i++){
		in.var[i]=unescape(in.var[i]);
	}
	return in;
}
vector vec_escape(vector in){
	for(int i=0; i<in.len; i++){
		in.var[i]=escape(in.var[i]);
	}
	return in;
}
void vec_freeable(vector* v,string in){
	if(in.readonly || !v->len) return;
	if(v->var[0].str==in.str)
		v->var[0].readonly=0;
	else
		assert(false);
}
string replace(string in, char* from, char* into){//api
	return join(split(in,from),into);
}
string replace_vec(string in,vector from,vector into){//api
	int hit=0;
	int extra=0;
	if(from.len>into.len) return in;
	for(int i=0; i<in.len; i++){
		for(int j=0; j<from.len; j++){
			if(start_s(sub(in,i,End),from.var[j])){
				extra+=into.var[j].len-from.var[j].len;
				hit=1;
				break;
			}
		}
	}
	if(!hit){
		vfree(&from);
		vfree(&into);
		return in;
	}
//dump(from);
//dump(into);
	string ret=s_new(NULL,in.len+extra);
	extra=0;
	for(int i=0; i<in.len; i++){
		int hit=0;
		for(int j=0; j<from.len; j++){
			if(start_s(sub(in,i,End),from.var[j])){
				memcpy(ret.str+i+extra,into.var[j].str,into.var[j].len);
				extra+=into.var[j].len-from.var[j].len;
				i+=from.var[j].len-1;
				hit=1;
				break;
			}
		}
		if(hit) continue;
		ret.str[i+extra]=in.str[i];
	}
	vfree(&from);
	vfree(&into);
	ptr_free(&in);
	return ret;
}
string code_name(string in){//api
	string ret=in;
	in=rw(in);
	for(int i=0; i<ret.len; i++){
		if(!i){
			if(ret.str[i]>='a' && ret.str[i]<='z') ret.str[i]-='a'-'A';
		}
		else if(ret.str[i]=='_') ret.str[i]=' ';
		else if(ret.str[i]>='A' && ret.str[i]<='Z'){
			if(ret.str[i-1]>='a' && ret.str[i-1]<='z'){
				ret=splice(ret,sub(ret,i,1),c_(" "));
			}
		}
	}
	return ret;
}
