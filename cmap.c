#include <cvar.h>


int benchmark(){
	map mp=NullMap;
	for(int i=0; i<5000000; i++){
		map_addv(mp,cat(c_("id."),i_s(i%500000)))=cat(c_("val."),i_s(i));
	}
	printf("%d\n",mp.keys.len);
	printf("%.*s\n",ls(cget(mp,"id.10000")));
	map_free(&mp);
	return 1;
}
int main(int argc, char** argv){
	reg_cvar();
	benchmark();
	reg_free();
	return 0;
}
