#include <wordexp.h>
#include <dlfcn.h>

#include "mem.h"
#include "map.h"
#include "file.h"

/*header
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
end*/

int is_file(char* filename){
	struct stat st={0};
	stat(filename, &st);
	return S_ISREG(st.st_mode) || S_ISLNK(st.st_mode);
}
int is_dir(char* filename){
	struct stat st={0};
	stat(filename, &st);
	return S_ISDIR(st.st_mode);
}
int file_size(char* filename){
	struct stat st={0};
	stat(filename, &st);
	return st.st_size;
}
string path_cat(string path1, string path2){
	if(!path1.len) return path2;
	if(!path2.len) return path1;
	if(path2.str[0]=='.') path2=cut(path2,".").tail;
	if(path2.str[0]=='/') path2=cut(path2,"/").tail;
	return cat(path1,path2);
}
int fp_write(FILE* fp,string out){
	if(!fp) return 0;
	string parts=out;
	while(parts.len>0){
		int ret=fwrite(parts.str,1,parts.len,fp);
		if(!ret){
			error("Writing to file pointer failed");
			return 0;
		}
		parts=sub(parts,ret,parts.len);
	}
	ptr_free(&out);
	return 1;
}
int s_file(string in,char* filename){//api
	FILE* fp=file_fp(filename,"w");
	if(!fp) return 0;
	int ret=fp_write(fp,in);
	if(!ret){
		error("failed to save into file %v",filename);
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}
FILE* file_fp(char* filename,char* rw){
	if(!filename || filename[0]=='-') return NULL;
	FILE* ret=fopen(filename,rw);
	if(!ret){
		error("failed to open file %s",filename);
		return NULL;
	}
	return ret;
}
string file_s(char* filename){//api
	FILE* fp=file_fp(filename,"r");
	if(!fp) return NullStr;
	fseek(fp,0,SEEK_END);
	size_t size=ftell(fp);
	fseek(fp,0,SEEK_SET);
	string ret=s_new(NULL,size);
	size_t read=fread(ret.str,1,size,fp);
	fclose(fp);
	if(read!=size) ptr_free(&ret);
	return ret;
}
char* filename_os(string filename){
	if(!filename.len) return NULL;
	char* temp=s_cptr(filename);
    wordexp_t exp_result;
    int err=wordexp(temp, &exp_result, 0);
	allocator(temp,0);
	if(err) return NULL;
	string ret=c_(exp_result.we_wordv[0]);
	ret.len+=1;
	ret=copy(ret);
	wordfree(&exp_result);
	return ret.ptr;
}
void lib_close(dlib in){//api
	lib_close_p(&in);
}
void lib_close_p(void* lib){
	dlib in=*(dlib*)lib;
	if(in.lib){
		in.release();
		dlclose(in.lib);
	}
	map_free(&in.funcs);
}

void globals_get(void*);
void dlib_dump(dlib);
void globals_set(void*);
dlib lib_conn(char* file){//api
	dlib ret={0};
	//ret.lib=dlopen(file,RTLD_LAZY|RTLD_NODELETE);
	ret.lib=dlopen(file,RTLD_LAZY);
	if(!ret.lib){
		fprintf(stderr,"%s",dlerror());
		return ret;
	}
	ret.init=dlsym(ret.lib,"init");
	ret.reflect=dlsym(ret.lib,"reflect");
	ret.invoke=dlsym(ret.lib,"invoke");
	ret.release=dlsym(ret.lib,"release");
	if(!ret.init || !ret.reflect || !ret.invoke || !ret.release){
		fprintf(stderr,"not a capp library. init() not found.");
		lib_close(ret);
		return (dlib){0};
	}
	struct globals* g=allocator(NULL,sizeof(struct globals));
	globals_get(g);
	ret.init(g);
	globals_set(g);
	allocator(g,0);
	ret.funcs=ret.reflect();
	if(!ret.funcs.keys.len){
		fprintf(stderr,"lib.funcs() return empty list");
		lib_close(ret);
		return (dlib){0};
	}
//	dlib_dump(ret);
	return ret;
}
