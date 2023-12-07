#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
using namespace std;
map<int,int>memory;
vector<int> seq_of_access;

bool isPowerOfTwo(int number) {
    return (number > 0 && (log2(number) == round(log2(number))) );
}

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
struct cache_entry
{
    bool valid;
    int index;
    string Tag;
    vector<int>data;
    
    
    cache_entry(int i, int d, string t, bool v) : index(i), data(d, 0), Tag(t), valid(v) {
       
    }
    
};
struct cache
{
    
    vector<cache_entry> cache_entries;
    int cycles;
    int cache_size;//total bytes in cache user
    int lines;//cache_size/line_size->block calculated
    int line_size;
    int offset;
    int idx;
    int T;
    
    int accesses;
    int hits;
    int misses;
    
    cache(int size, int line_s, int c) :  cache_size(size), line_size(line_s), cycles(c) {
        lines= cache_size/line_size;
        for (int i = 0; i < lines; i++) {
            cache_entry entry(i, line_size, "", false);
            cache_entries.push_back(entry);
        }
        offset= int(log2(line_size));
        idx=int(log2(lines));
        T = 24-(offset+idx);
    accesses=0;
    hits=0;
    misses=0;
    }
    
    void access_sequence()
    {
        
       
      for (auto mem_address : seq_of_access) {
      string bin_address=decimal_binary(mem_address);
          string x=bin_address.substr(0,T);
         x=bin_address.substr(T,idx);
        x=bin_address.substr(T+idx,offset);
      string tag = bin_address.substr(0,T);
      int index = stoi(bin_address.substr(T,idx),0,2);
      int disp = stoi(bin_address.substr(T+idx,offset),0,2);
          bool hit= false;
       accesses++;
            if (cache_entries[index].Tag == tag && cache_entries[index].valid)
                {
                    hit=true;
                    hits++;
                }
            else
            {
                misses++;
                cache_entries[index].Tag = tag;
                cache_entries[index].valid = true;
            }
            
            cout << "Access: " << dec<< accesses << endl;
          cout<< "Getting address: 0x"<<hex <<mem_address<<endl;
          if (hit) {
              cout<<"HIT!"<< endl;
          }
          else cout<<"MISS!"<< endl;
            for (auto entry : cache_entries)
            {
                cout << "Cache Entry " << dec<<entry.index+1 << ": Valid bit- " << entry.valid << ", Tag bits- " << entry.Tag << endl;
            }
            double miss_ratio= (double) misses / accesses;
            double hit_ratio= (double) hits / accesses;
            cout << "Total Number of Accesses: " << accesses << endl;
            cout << "Hit Ratio: " << hit_ratio << endl;
            cout << "Miss Ratio: " << miss_ratio << endl;
          double AMAT= ((double)cycles + (miss_ratio * 120));
            cout << "Average Memory Access Time (AMAT): " << AMAT << " cycles" << endl<<endl;
        }
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
            seq_of_access.push_back(mem_add);
            }
        }
        file.close();
    } else {
        cout << "Unable to open file" << endl;
    }
}


int main()
{

    int cacheSize, lineSize, cycles;
    do {
        cout << "Enter cache size (in bytes) must be a power of 2 in the form 2^n: ";
        cin >> cacheSize;
    } while (!isPowerOfTwo(cacheSize));
    do {
       cout << "Enter cache line size (in bytes) must be a power of 2 in the form 2^n: ";
       cin >> lineSize;
    } while (!isPowerOfTwo(lineSize) || lineSize>=cacheSize);
       cout << "Enter number of cycles needed to access the cache: ";
       cin >> cycles;
    string filename;
    cout << "Enter the name of the file contatining the access sequence. (eg. input.txt)" << endl;
    cin >> filename;
    loading_data(filename);
    //seq_of_access = {4444,333,111,333,48294,42521,111,28942,123425,1024,532};

 cache my_cache(cacheSize, lineSize, cycles);
 my_cache.access_sequence();
 
    return 0;
}
