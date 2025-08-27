#include "var.h"
#include "str.h"
#include "vector.h"
#include "map.h"

#define map_new(ret, ...) _map_new(ret, ##__VA_ARGS__,NULL)
#define mget(x, ...) _mget(x, ##__VA_ARGS__,NULL)
#define map_each(x,y,z) z=x.vals.ptr; for(int y=0; y<x.keys.len; y++)
#define map_add(x,y) *(var*)map_add_ex(&x,c_(y))
#define map_addv(x,y) *(var*)map_add_ex(&x,y)
#define map_add_type(a,b,c) *(a*)map_add_ex(&b,c)
#define map_get_type(x,y,z) (*(x*)map_getp(y,z))
#define map_(x) p_(&x,Map)


/*header


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

end*/

#define NullMap (map){.index.type=MapIndex,.keys.type=Var,.vals.type=Var}

map vec_map(vector keys,vector vals){
	return (map){
		.keys=keys,
		.vals=vals,
		.index=keys_index(keys),
	};
}
map map_new_ex(id type){
	map ret={0};
	ret.keys=NullVec;
	ret.vals=vec_new(type,0);
	ret.index=vec_new(MapIndex,0);
	return ret;
}
int index_has(vector index, string key, vector keys){
	//2025:habib was keys.len
	if(!index.len) return End;
	int slot=hash(key) & index.len-1;
	slot=((mapindex*)index.str)[slot].head-1;
	while(1){
		if(slot<0) return End;
		if(eq_s(key,((var*)keys.str)[slot])) return slot;
		slot=((mapindex*)index.str)[slot].tail-1;
	}
	//never.
}
void* map_getp(map in,var key){
	return getp(in.vals,index_has(in.index,key,in.keys));
}
var map_own(map in,char* key){
	return own(getp(in.vals,index_has(in.index,c_(key),in.keys)));
}
var cget(map in, char* key){
	return p_ro(map_getp(in,c_(key)),in.vals.type);
}
var map_get(map in,var key){
	return p_ro(map_getp(in,key),in.vals.type);
}
var _mget(map in, ...){
	va_list args;
	va_start(args,in);
	var ret=p_ro(&in,Map);
	char* temp=NULL;
	while((temp=va_arg(args,char*)) && ret.type!=Terminator){
		string key=c_(temp);
		if(ret.type==Map){
			ret=map_get(in,key);
			continue;
		}
		if(!ret.len || ret.type==Char){
			ret=VarEnd;
			break;
		}
		if(!s_is(key,"0")){
			ret=VarEnd;
			break;
		}
		int i=atoi(key.ptr);
		ret=get(ret,i);
	}
	return ret;
}
map* map_delc(map* in, char* key){
	return map_del(in,c_(key));
}
map* map_del_idx(map* in, int slot){
	if(slot<0||slot>=in->keys.len) return in;
	in->vals=vec_del(in->vals,slot);
	in->keys=vec_del(in->keys,slot);
	int newlen=pow2(in->keys.len);
	if(newlen!=in->index.len){
		in->index=keys_reindex(in->keys,resize(in->index,newlen),0);
		return in;
	}
	mapindex* idx=(void*)in->index.str;
	int slotnext=idx[slot].tail;
	for(int i=0; i<in->index.len; i++){
		if(idx[i].head==slot+1) idx[i].head=slotnext;
		if(i>=slot && i<in->keys.len) idx[i].tail=idx[i+1].tail;
		if(idx[i].tail==slot+1) idx[i].tail=slotnext;
		if(idx[i].head>slot+1) idx[i].head--;
		if(idx[i].tail>slot+1) idx[i].tail--;
	}
	idx[in->keys.len].tail=0;
	return in;
}
map* map_del(map* in, var key){
	return map_del_idx(in,index_has(in->index,key,in->keys));
}
void* map_add_ex(map* in, var key){
	int slot=index_has(in->index,key,in->keys);
	if(slot!=End){ //key exists
		//printf("key=%.*s exits at slot=%d\n",ls(key),slot);
		if(meta[in->vals.type].free) meta[in->vals.type].free(getp(in->vals,slot));
		ptr_free(&key);
		return getp(in->vals,slot);
	}
	if(in->index.len<in->keys.len+1){
		in->index=keys_reindex(in->keys,resize(in->index,pow2(in->keys.len+1)),0);
	}
	add(in->keys)=key;
	in->index=keys_reindex(in->keys,in->index,in->keys.len-1);
	return grow(&in->vals,1);
}
vector keys_index(vector keys){
	vector ret=vec_new(MapIndex,pow2(keys.len));
	mapindex* idx=(mapindex*)ret.str;
	for(int i=0; i<keys.len; i++){
		int slot=hash(keys.var[i]) & ret.len-1;
		if(idx[slot].head) idx[i].tail=idx[slot].head;
		idx[slot].head=i+1;
	}
	return ret;
}
vector keys_reindex(vector keys, vector index, int from){
	mapindex* idx=(mapindex*)index.ptr;
	if(!from) memset(idx,0,datasize(index)*index.len);
	for(int i=from; i<keys.len; i++){
		int slot=hash(keys.var[i]) & index.len-1;
		if(idx[slot].head) idx[i].tail=idx[slot].head;
		idx[slot].head=i+1;
	}
	return index;
}
map map_ro(map in){
	in.vals.readonly=1;
	in.keys.readonly=1;
	in.index.readonly=1;
	return in;
}
void map_free(void* val){
	map* in=val;
	vfree(&in->vals);
	vfree(&in->keys);
	ptr_free(&in->index);
}
void* map_p_i(map m,int i){
	return m.vals.str+datasize(m.vals)*i;
}
map _map_new(char* in,...){
	map ret=NullMap;
	va_list args;
	va_start(args,in);
	char* name=in;
	while(1){
		if(!name) break;
		var val=va_arg(args,var);
		map_add(ret,name)=val;
		name=va_arg(args,char*);
	}
	va_end(args);
	return ret;
}
int rows_count(rows in){
	return in.vals.len/in.keys.len;
}
map rows_row(rows in, int idx){
	if(idx<0||idx>=rows_count(in)) return (map){0};
	in.vals.str=in.vals.str+datasize(in.vals)*in.keys.len*idx;
	in.vals.len=in.keys.len;
	in.vals.readonly=in.keys.readonly=in.index.readonly=1;
	return in;
}
string rows_tsv(rows in){//api
	vector lines=NullVec;
	lines=resize(lines,in.vals.len/in.keys.len+1);
	set(lines,0)=join(vec_escape(in.keys),"\t");
	for(int i=0; i<in.vals.len/in.keys.len; i++){
		set(lines,i+1)=join(vec_escape(rows_row(in,i).vals),"\t");
	}
	ptr_free(&in.index);
	return join(lines,"\n");
}
string map_s(map in,char* sepkey,char* sepval){
	if(!in.keys.len) return NullStr;
	string sep1=c_(sepkey);
	string sep2=c_(sepval);
	int len=in.keys.len*(sep1.len+sep2.len)+vec_strlen(in.keys)+vec_strlen(in.vals);
	string ret=s_new(NULL,len);
	int off=0;
	for(int i=0; i<in.keys.len; i++){
		off=buff_add(ret,get(in.keys,i),off);
		off=buff_add(ret,sep1,off);
		off=buff_add(ret,get(in.vals,i),off);
		off=buff_add(ret,sep2,off);
	}
	map_free(&in);
	return ret;
}
string _json(var in){
	string ret={0};
	if(!in.ptr && !in.len) return c_("NULL");
	if(in.len>0 && !datasize(in)) //string
		return escape(in); // todo: only for '"'

	if(in.type==Map){
		map m=*(map*)in.ptr;
		ret=cat(ret,c_("{"));
		for(int i=0; i<m.keys.len; i++){
			if(i) ret=cat(ret,c_(", "));
			ret=cat(ret, _json(get(m.keys,i)));
			ret=cat(ret,c_(": "));
			ret=cat(ret, _json(get(m.vals,i)));
		}
		ret=cat(ret,c_("}"));
		return ret;
	}
	if(in.type==String){
		ret=cat(ret,c_("["));
		for(int i=0; i<in.len; i++){
			if(i) ret=cat(ret,c_(", "));
			ret=cat(ret, _json(get(in,i)));
		}
		ret=cat(ret,c_("]"));
		return ret;
	}
	return _s(in);
}
int hash(string in){
	int ret = 0x1d4592f8;
	if(!in.len){
		ret+=(int)in.ll+(in.ll>>32);
	}
	else{
		int i=0;
		while(i<in.len) ret=((ret<<5)+ret)+tolower(in.str[i++]);
	}
	return ret ? ret : 0xc533c700; //never return 0
}
int pow2(int i){
	if(!i) return 0;
	i--;
	int ret=2;
	while(i>>=1) ret<<=1;
	return ret;
};
rows tsv_rows(string in){//api
	vector lines=split(trim(in),"\n");
	if(!lines.len) return NullMap;
	vector keys=vec_unescape(split(get(lines,0),"\t"));
	if(!keys.len) return NullMap;
	vector vals=vec_new(Var, keys.len*(lines.len-1));
	int off=0;
	for(int i=1; i<lines.len; i++){
		vector temp=vec_unescape(split(get(lines,i),"\t"));
		assert(temp.len<=keys.len);
		temp=resize(temp,keys.len);
		off=buff_add(vals,temp,off);
	}
	ptr_free(&lines);
//	vfree(&vals);
//	vfree(&keys);
//	return NullMap;
	return (rows){
		.keys=keys,
		.vals=vals,
		.index=keys_index(keys),
	};
}
