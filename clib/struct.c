#include "map.h"
#include "struct.h"


void struct_free(void* ptr,id type){
	if(!ptr) return;
	each(meta[type].member, i, struct member* mem){
		if(!meta[mem[i].type].free) continue;
		meta[mem[i].type].free((char*)ptr+mem[i].offset);
	}
}
var s_type(var in, id type){
	if(in.type==type) return in;
	var ret=null;
	if(!in.ptr || in.type==Null) return ret;
	assert(in.type==Char);
	switch(type){
		case Int:
			ret=(var){.i=atoi(in.str),.type=Int};
			break;
		case Double:
			ret=(var){.f=atof(in.str),.type=Double};
			break;
		case Long:
			ret=(var){.type=Long};
			*(long long*)&ret=atoll(in.str);
			break;
		default:
			return in;
	}
	vfree(&in);
	return ret;
}
string struct_s(void* in, id type){
	return map_s(struct_map(in,type)," ","\n");
}
void* s_struct(string in,id type,void* ret){
	vector lines=split(in,"\n");
	map temp=NullMap;
	each(lines,i,string* ln){
		string line=cut(ln[i],"//").head;
		pair kv=cut(line," ");
		if(!kv.head.len || !kv.tail.len) continue;
		map_addv(temp,kv.head)=kv.tail;
	}
	return map_struct(temp,ret,type);
}
void* map_struct(map in, void* ret,id type){
	each(meta[type].member, i, struct member* mem){
		void* ptr=map_getp(in,c_(mem[i].name));
		if(!ptr) continue;
		var temp=s_type(*(var*)ptr,mem[i].type);
		memcpy(ret+mem[i].offset,_p(&temp,mem[i].type),meta[mem[i].type].datasize);
	}
	ptr_free(&in.index);
	vfree(&in.keys);
	ptr_free(&in.vals);
	return ret;
}
void* member_ptr(id type, void* ptr, int memberno){
	return (char*)ptr+get_type(struct member, meta[type].member, memberno).offset;
}
void* vec_struct(vector in, void* ret, id type){
	each(meta[type].member, i, struct member* mem){
		var v=get(in,i);	
		memcpy(ret+mem[i].offset,_p(&v,mem[i].type),meta[mem[i].type].datasize);
	}
	ptr_free(&in);
	return ret;
}
vector struct_vec(void* in, id type){
	vector ret=vec_new(Var,meta[type].member.len);
	each(meta[type].member, i, struct member* mem){
		set(ret,i)=p_(in+mem[i].offset,mem[i].type);
	}
	return ret;
}
map struct_map(void* in, id type){
	vector keys=vec_new(String,meta[type].member.len);
	each(meta[type].member, i, struct member* mem){
		set(keys,i)=c_(mem[i].name);
	}
	return (map){
		.keys=keys,
		.vals=struct_vec(in,type),
		.index=keys_index(keys)
	};
}
