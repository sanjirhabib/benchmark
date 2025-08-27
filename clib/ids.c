#include "mem.h"
#include "var.h"
#include "str.h"
#include "vector.h"
#include "map.h"
#include "file.h"
#include "ids.h"

#define ids_init(x,y) ids_add(x,sizeof(x)/sizeof(char*),y)

/*header
extern vector cache;
end*/

vector cache=NullVec;

static map id_name=NullMap;
static map name_id=NullMap;

string id_s(id in){//api
	return map_get(id_name,i_(in));
}
id s_id(string name){//api
	return map_get(name_id,name).i;
}
var id_(id in){
	return (var){
		.type=ID,
		.i=in,
	};
}
void ids_add(char** in, int len, int first_id){
	int from=id_name.keys.len;
	string* ids=grow(&id_name.keys,len);
	string* names=grow(&id_name.vals,len);

	for(int i=0; i<len; i++){
		ids[i]=i_(first_id+i);
		names[i]=c_(in[i]);
	}
	ptr_free(&id_name.index);
	ptr_free(&name_id.index);

	name_id.keys=id_name.vals;
	name_id.vals=id_name.keys;

	id_name.index=keys_index(id_name.keys);
	name_id.index=keys_index(name_id.keys);
}
void meta_free(void* ptr){
	meta_type* in=ptr;
	ptr_free(&in->member);
}
void null_free(void*){
}
void reg_cvar(){
	assert(sizeof(var)==16);
//	if(!arena) buddy_init();
	meta[String]=(meta_type){
		.datasize=sizeof(string),
		.isvar=1,
		.embed=1,
		.free=vfree,
	}; 
	meta[Var]=meta[String];
	meta[Vector]=(meta_type){
		.datasize=sizeof(vector),
		.isvar=1,
		.embed=1,
		.free=vfree,
	};
	meta[Char]=(meta_type){
		.datasize=sizeof(char),
	}; 
	meta[CharPtr]=(meta_type){
		.datasize=sizeof(char*),
		.embed=1,
	}; 
	meta[ConstCharPtr]=(meta_type){
		.datasize=sizeof(char*),
	}; 
	meta[Bool]=(meta_type){
		.datasize=sizeof(char),
		.embed=1,
	}; 
	meta[Range]=(meta_type){
		.datasize=sizeof(range),
		.embed=1
	};
	meta[Int]=(meta_type){
		.datasize=sizeof(int),
		.embed=1,
	}; 
	meta[Long]=(meta_type){
		.datasize=sizeof(long long),
		.embed=1,
	}; 
	meta[TimeT]=meta[Long];
	meta[Void]=meta[Null];
	meta[ID]=meta[Int];
	meta[Double]=(meta_type){
		.datasize=sizeof(double),
		.embed=1,
	}; 
	meta[Float]=(meta_type){
		.datasize=sizeof(float),
		.embed=1,
	}; 
	meta[Map]=(meta_type){
		.datasize=sizeof(map),
		.free=map_free,
	}; 
	meta[Rows]=meta[Map];
	meta[MapIndex]=(meta_type){
		.datasize=sizeof(mapindex),
	}; 
	meta[MetaType]=(meta_type){
		.datasize=sizeof(meta_type),
		.free=meta_free,
	};
	meta[Member]=(meta_type){
		.datasize=sizeof(struct member),
	};
	meta[StrPair]=(meta_type){
		.datasize=sizeof(pair),
		.array=1,
		.member=copy(array_vec(Member,((struct member[]){
			{"head",String,offsetof(pair,head)},
			{"tail",String,offsetof(pair,tail)},
			})))
	};
	meta[DLib]=(meta_type){
		.datasize=sizeof(dlib),
		.free=lib_close_p,
	};
	reg_var();

	add(cache)=p_ro(&id_name,Map);
	add(cache)=(var){.ptr=&name_id.index,.type=Vector,.readonly=1};
	add(cache)=array_vec(MetaType,meta);
}
void reg_free(){
	vfree(&cache);
}
void globals_get(struct globals* g){
	g->id_name=id_name;
	g->name_id=name_id;
	array_copy(g->meta,meta);
}
void globals_set(struct globals* g){
	assert(g->meta[Var].datasize==16);
	id_name=g->id_name;
	name_id=g->name_id;
	array_copy(meta,g->meta);
	assert(meta[Var].datasize==16);
}
