

#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
map<int,int>memory;
string decimal_binary(int n)
{
    string binary = "";

    if (n < 0 || n >= (1 << 24)) {
        return "Error: Number out of 24-bit range";
    }
    for (int i = 23; i >= 0; i--) {
        binary += ((n >> i) & 1) ? "1" : "0";
    }

    return binary;
}
struct cache_Input
{
    bool valid;
    int index;
    int Tag;
    vector<int>data;
    cache_Input(int i, int d, int t, bool v) : valid(v), index(i), Tag(t), data(d, 0) {
       
    }
    
};
struct cache
{
    
    cache_Input cc;
    vector<cache_Input>inputs;
    int cycles;
    int cache_size;//total bytes in cache user
    //
    int lines;//cache_size/line_size->block calculated 
    int line_size;//user 
     //bits number
        int offset;
        int idx;
        int T;
    cache(int size, int line_s, int c) :  cache_size(size),line_size(line_s), cycles(c) {
        lines=cache_size/line_size;
        for (int i = 0; i < lines; i++) {
            cache_Input entry(i, line_size, 0, 0); 
            inputs.push_back(entry);
        }
        offset= int(log2(line_size));
        idx=int(log2(lines));
        int T=24-(offset+idx);
    }
    
    void Cache_Intialization(int Mem_address)
    {
        
       
        
        
    }
};
void loading_data(string& filename) {
    ifstream file(filename);
    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            istringstream iss(line);
            string part;
            while (std::getline(iss, part, ',')) {
            int mem_add=stoi(part);
             memory[mem_add]=0;
            }
        }
        file.close();
    } else {
        cout << "Unable to open file" << endl;
    }
}


int main()
{ 

    return 0;
}
