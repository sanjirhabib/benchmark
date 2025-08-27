#include <assert.h>
#include <errno.h>

#include "map.h"
#include "struct.h"
#include "ids.h"
#include "log.h"

/*header
struct s_log {
	vector lines;
	vector types;
	int is_error;
};
extern struct s_log logs;

end*/

int(*error)(char* format,...)=app_error;
int(*print)(char* format,...)=app_print;
int(*debug)(char* format,...)=app_debug;

vector ring_add(vector in,vector add,int size){
	if(in.len+add.len<size) return cat(in,add);
	if(add.len>=size) return sub(add,add.len-size,size);
	if(in.len<size) in=resize(in,size);
	assert(datasize(in)==datasize(add));
	memmove(in.str+add.len*datasize(add),in.str,(in.len-add.len)*datasize(in));
	memcpy(in.str,add.str,add.len*datasize(add));
	ptr_free(&add);
	return in;
}

void dx(var in){
	dump(in);
	fflush(stdout);
	raise(SIGINT);
}
int app_debug(char* format,...){
	fprintf(stderr,"c> ");
	va_list args;
	va_start(args, format);
	string temp=valist_s(0,format,args);
	fprintf(stderr,"%.*s\n",ls(temp));
	ptr_free(&temp);
	va_end(args);
	return 0;
}
int app_error(char* msg,...){
	fprintf(stderr,"ERROR: ");
	va_list args;
	va_start(args, msg);
	vfprintf(stderr,msg,args);
	va_end(args);
	fprintf(stderr,"\n");
	assert(false);
}
int app_print(char* format,...){
	va_list args;
	va_start(args, format);
	string temp=valist_s(0,format,args);
	printf("%.*s\n",ls(temp));
	ptr_free(&temp);
	va_end(args);
}
void dump(var in){
	_dump(in,0);
	vfree(&in);
}
void map_dump(map in){
	_map_dump(in,0);
	map_free(&in);
}
void _dump(var in,int indent){
	if(in.len){
		_vec_dump(in,indent);
		return;
	}
	if(in.type==StrPair){
		pair_dump(*(pair*)in.ptr,indent);
		return;
	}
	if(in.type==Map){
		_map_dump(*(map*)in.ptr,indent);
		return;
	}
	if(meta[in.type].member.len){
		map temp=struct_map(_p(&in,in.type),in.type);
		_map_dump(temp,indent);
		ptr_free(&temp.index);
		ptr_free(&temp.keys);
		ptr_free(&temp.vals);
		return;
	}
	switch(in.type){
		case Int: printf("%d",in.i); return;
		case Double: printf("%g",in.f); return;
		case Long: printf("%ll",*(long long*)&in); return;
		case Char: printf("\"%s\"",in.ptr); return;
		case Null: printf("null"); return;
		case ID: printf("%.*s",ls(id_s(in.i))); return;
		case CharPtr: printf("\"%s\"",in.str); return;
		default : printf("<%.*s>",ls(id_s(in.type))); return;
	}
}
void _vec_dump(vector in, int indent){
	if(in.type==Char){
		printf("\"%.*s\"",ls(in));
		return;
	}
	printf("[");
	_dump(get(in,0),indent);
	for(int i=1; i<in.len; i++){
		printf(", ");
		_dump(get(in,i),indent);
	}
	printf("]");
}
void _map_dump(map m, int indent){
	if(!m.keys.len){
		printf("{}");
		return;
	}
	printf("{\n");
	int rows=m.vals.len/m.keys.len;
	indent++;
	for(int j=0; j<rows; j++){
		for(int i=0; i<m.keys.len; i++){
			printf("%*s", indent*4, "");
			printf("%.*s: ",ls(get(m.keys,i)));
			_dump(get(m.vals,j*m.keys.len+i),indent);
			printf("\n");
		}
		if(j<rows-1) printf("--\n");
	}
	indent--;
	printf("%*s", indent*4, "");
	printf("}\n");
}
string bin_dump(string in){
	for(int i=0; i<in.len; i++) printf("%02X ",(unsigned char)in.str[i]);
	printf("\n");
}
void pair_dump(pair v,int indent){
	printf("{\n");
	printf("%*shead: ", indent*4,"");
	_dump(v.head,indent);
	printf("\n");
	printf("%*stail: ", indent*4,"");
	_dump(v.tail,indent);
	printf("\n");
	printf("%*s", indent*4, "");
	printf("}");
}
void dlib_dump(dlib in){
	printf("in.ptr:%p\n",in.lib);
	_map_dump(in.funcs,0);
	exit(0);
}
