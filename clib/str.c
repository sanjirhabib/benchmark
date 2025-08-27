#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "var.h"
#include "str.h"

string dequote(string in,char* qchars){
	if(in.len<2) return in;
	if(strchr(qchars,in.str[0])) return sub(in,1,End-1);
	return in;
}
string unescape_ex(string in,string find, string replace){
	if(!in.len) return in;
	int extra=0;
	for(int i=1; i<in.len; i++){
		if(in.str[i-1]!='\\') continue;
		extra++;
		i++;
	}
	if(!extra) return in;
	string ret=s_new(NULL,in.len-extra);
	//printf("unescape str=%.*s less=%d\n",ls(in),extra);
	int offset=0;
	for(int i=1; i<in.len; i++){
		char c=in.str[i-1];
		if(c!='\\'){
			ret.str[offset++]=c;
			continue;
		}
		c=in.str[i];
		char* found=memchr(replace.str,c,replace.len);
		ret.str[offset++]=found ? find.str[found-replace.str] : c;
		i++;
	}
	ret.str[ret.len-1]=in.str[in.len-1];
	ptr_free(&in);
	return ret;
}
string unescape(string in){//api
	return unescape_ex(in,c_("0nrt\\e"),cs_("\0\n\r\t\\\033"));
}
string escape(string in){//api
	return escape_ex(in,cs_("\0\n\r\t\\\033"),c_("0nrt\\e"));
}
string escape_ex(string in,string find,string replace){
	int extra=0;
	for(int i=0; i<in.len ; i++)
		if(memchr(find.str,in.str[i],find.len))
			extra++;
	if(!extra) return in;
	string ret=s_new(NULL,in.len+extra);
	int offset=0;
	for(int i=0; i<in.len ; i++){
		char c=in.str[i];
		char* found=memchr(find.str,c,find.len);
		if(found){
			ret.str[offset++]='\\';
			ret.str[offset++]=replace.str[found-find.str];
		}
		else
			ret.str[offset++]=c;
	}
	ptr_free(&in);
	return ret;
}
int s_chr(string in,char has){
	for(int i=0; i<in.len; i++)
		if(in.str[i]==has) return i+1;
	return 0;
}
char* s_has(string in,char* has){
	int sublen=strlen(has);
	for(int i=0; i<=in.len-sublen; i++)
		if(c_eq(in.str+i,has,sublen))
			return in.str+i;
	return NULL;
}
int is_word(string word,char* list){
	char* str=list;
	char* end=list+strlen(list)-word.len;
	while(str<=end){
		if(memcmp(str,word.str,word.len)==0 && (str==end || *(str+word.len)==' ')) return 1;
		while(str<end && *str!=' ') str++;
		str++;
	}
	return 0;
}
string s_join(string in,char* terminator,string add){
	if(!add.len) return in;
	if(!in.len) return add;
	return cat(cat_c(in,terminator),add);
}
string format(char* fmt,...){
	va_list args;
	va_start(args,fmt);
	string ret=valist_s(0,fmt,args);
	va_end(args);
	return ret;
}

string valist_s(int readonly,char* fmt,va_list args){
	string format=c_(fmt);
	string tok=NullStr;
	pair temp=cut(format,"%");
	string ret=temp.head;
	while((tok=next(temp.tail,"%",tok)).type!=Terminator){
		string subs=tok;
		int flen=0;
		string nums=subs;
		nums.len=0;
		while(subs.len && strchr("-.*0123456789",subs.str[0])){
			nums.len++;
			subs.str++;
			subs.len--;
		}
		struct s_align {
			int neg;
			int lenparam;
			int width;
			int zero;
			int deci;
		} align={0};
		if(nums.len && nums.str[0]=='-'){
			align.neg=1;
			nums.str++;
			nums.len--;
		}
		if(nums.len && nums.str[0]=='.'){
			nums.str++;
			nums.len--;
		}
		if(nums.len && nums.str[0]=='*'){
			align.lenparam=1;
			nums.str++;
			nums.len--;
		}
		if(nums.len && nums.str[0]=='0'){
			align.zero=1;
			nums.str++;
			nums.len--;
		}
		if(nums.len){
			pair temp=cut(nums,".");
			align.width=_i(temp.head);
			align.deci=_i(temp.tail);
		}

		if(!subs.len){
			ret=cat(ret,(string){.str=subs.str-1,.len=subs.len+1});
		}
		else if(subs.str[0]=='s'){
			if(align.lenparam){
				int len=va_arg(args,int);
				char* by=va_arg(args,char*);
				if(len>=0 && !c_len(by)){
					memset(grow(&ret,len),' ',len);
				}
				else if(!len){
				}
				else assert(0);
				flen=1;
			}
			else{
				ret=cat(ret,c_(va_arg(args,char*)));
			}
		}
		else if(subs.str[0]=='p'){
			void* ptr=va_arg(args,void*);
			char buff[sizeof(void*)*2+1];
			long long a=(long long)ptr;
			buff[sizeof(void*)*2]=0;
			for(int i=0; i<sizeof(void*)*2; i++){
				int val=(a & (((long long)0xF)<<(i*4)))>>(i*4);
				char c=val<10 ? '0'+val : 'a'+(val-10);
				buff[sizeof(void*)*2-i-1]=c;
			}
			char* head=buff;
			while(head[1] && *head=='0') head++;
			ret=cat_c(ret,"0x");
			ret=cat_c(ret,head);
		}
		else if(subs.str[0]=='v'){
			var param=va_arg(args,var);
			if(readonly) param.readonly=1;
			if(!param.len){}
			else if(param.type==Char)
				ret=cat(ret,param);
			else if(param.type==Int)
				ret=cat(ret,i_s(param.i));
			else if(param.type==Terminator)
				ret=cat(ret,c_("Error"));
			else if(param.type!=Null && param.type!=Void)
				assert(0);
		}
		else if(subs.str[0]=='d'){
			int i=va_arg(args,int);
			string num=i_s(i);
			int spaces=align.width-num.len;
			if(spaces>0 && !align.neg) memset(grow(&ret,spaces),' ',spaces);
			ret=cat(ret,num);
			if(spaces>0 && align.neg) memset(grow(&ret,spaces),' ',spaces);
		}
		else if(subs.str[0]=='c'){
			*(char*)grow(&ret,1)=va_arg(args,int);
		}
		ret=cat(ret,sub(subs,1+flen,End));
	}
	return ret;
}
string c_repeat(char* in, int times){
	int len=strlen(in);
	string ret=s_new(NULL,len*times);
	for(int i=0; i<times; i++){
		memcpy(ret.str+i*times,in,len);
	}
	return ret;
}
string sub_a(string in, string sub){
	return cl_(in.str, sub.str-in.str);
}
string sub_bc(string in, string sub){
	return cl_(sub.str, in.len-(sub.str-in.str));
}
string s_upper(string in){
	in=rw(in);
	for(int i=0; i<in.len; i++){
		if(in.str[i]>='a' && in.str[i]<='z') in.str[i]-=('a'-'A');
	}
	return in;
}
string s_lower(string in){
	in=rw(in);
	for(int i=0; i<in.len; i++){
		if(in.str[i]>='A' && in.str[i]<='Z') in.str[i]+=('a'-'A');
	}
	return in;
}
void s_catchar(string* in,char c){
	*(char*)grow(in,1)=c;
}
string c_nullterm(char* in){
	return cl_(in,strlen(in)+1);
}
char* s_cptr(string in){
	if(in.len && !in.str[in.len-1]) return in.str;
	*(char*)grow(&in,1)='\0';
	return in.str;
}
int char_count(string in,char c){
	int ret=0;
	for(int i=0; i<in.len; i++) if(in.str[i]==c) ret++;
	return ret;
}
string s_insert(string in,int offset,string by){
	offset=between(0,offset,in.len);
	if(!by.len) return in;
	int len=in.len;
	in=resize(in,in.len+by.len);
	memmove(in.str+offset+by.len,in.str+offset,len-offset);
	memcpy(in.str+offset,by.str,by.len);
	return in;
}
string trim_ex(string in, char* chars){
	char* ptr=in.ptr;
	char* end=in.ptr+in.len;
	while(ptr<end && strchr(chars,*ptr)) ptr++;
	while(end>ptr && strchr(chars,*(end-1))) end--;
	return cl_(ptr,end-ptr);
}
string trim(string in){
	return trim_ex(in," \t\n\r");
}
int c_is(char c, char* chars){
	int len=strlen(chars);
	for(int i=0; i<len; i++){
		if(chars[i]=='A' && c>='A' && c<='Z') return 1;
		else if(chars[i]=='a' && c>='a' && c<='z') return 1;
		else if(chars[i]=='0' && c>='0' && c<='9') return 1;
		else if(chars[i]==c) return 1;
	}
	return 0;
}
int s_is(string in, char* chars){
	for(int i=0; i<in.len; i++){
		if(!c_is(in.str[i],chars)) return 0;
	}
	return 1;
}
string head_add(string in, int add){
	if(add>0 && in.len<=add) return NullStr;
	in.ptr+=add;
	in.len-=add;
	return in;
}
