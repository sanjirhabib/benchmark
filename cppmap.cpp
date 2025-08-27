#include <iostream> 
#include <map> 
using namespace std; 
  
int main(){ 
	map<string, string> ret; 
	for(int i=0; i<5000000; i++){
		ret[string("id.")+to_string(i%500000)] = string("val.")+to_string(i); 
	}
	cout << ret.size() << endl;
	cout << ret["id.10000"] << endl;
	return 0;
} 
