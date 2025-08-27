#pragma once
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "var.h"
string dequote(string in,char* qchars);
string unescape_ex(string in,string find, string replace);
string unescape(string in);//api
string escape(string in);//api
string escape_ex(string in,string find,string replace);
int s_chr(string in,char has);
char* s_has(string in,char* has);
int is_word(string word,char* list);
string s_join(string in,char* terminator,string add);
string format(char* fmt,...);
string valist_s(int readonly,char* fmt,va_list args);
string c_repeat(char* in, int times);
string sub_a(string in, string sub);
string sub_bc(string in, string sub);
string s_upper(string in);
string s_lower(string in);
void s_catchar(string* in,char c);
string c_nullterm(char* in);
char* s_cptr(string in);
int char_count(string in,char c);
string s_insert(string in,int offset,string by);
string trim_ex(string in, char* chars);
string trim(string in);
int c_is(char c, char* chars);
int s_is(string in, char* chars);
string head_add(string in, int add);
