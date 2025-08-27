#pragma once
#include "map.h"
void struct_free(void* ptr,id type);
var s_type(var in, id type);
string struct_s(void* in, id type);
void* s_struct(string in,id type,void* ret);
void* map_struct(map in, void* ret,id type);
void* member_ptr(id type, void* ptr, int memberno);
void* vec_struct(vector in, void* ret, id type);
vector struct_vec(void* in, id type);
map struct_map(void* in, id type);
