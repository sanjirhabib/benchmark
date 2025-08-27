#include <iostream>
#include <google/dense_hash_map>
#include <string>

using namespace std;

int main() {
    google::dense_hash_map<string, string> ret;
    ret.set_empty_key("");
    for (int i = 0; i < 5000000; i++) {
        ret[string("id.") + to_string(i % 500000)] = string("val.") + to_string(i);
    }
    cout << ret.size() << endl;
    cout << ret["id.10000"] << endl;
    return 0;
}
