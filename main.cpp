#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
using namespace std;

map<int, int> memory;
vector<pair<int, char>> seq_of_access; // pair of address and operation (r/w)
bool writeThrough= false;
bool writeAllocate= false;

bool isPowerOfTwo(int number) {
    return (number > 0 && (log2(number) == round(log2(number))));
}

string decimal_binary(int n) {
    string binary = "";

    if (n < 0 || n >= (1 << 24)) {
        return "Error: Number out of 24-bit range";
    }
    for (int i = 23; i >= 0; i--) {
        binary += ((n >> i) & 1) ? "1" : "0";
    }

    return binary;
}

struct cache_entry {
    bool valid;
    bool dirty;
    int index;
    string Tag;
    vector<int> data;

    cache_entry(int i, int d, string t, bool v, bool dir) : index(i), data(d, 0), Tag(t), valid(v), dirty(dir) {}
};

struct cache {
    vector<cache_entry> cache_entries;
    int cycles;
    int cache_size;
    int lines;
    int line_size;
    int offset;
    int idx;
    int T;

    int accesses;
    int hits;
    int misses;

    cache(int size, int line_s, int c) : cache_size(size), line_size(line_s), cycles(c) {
        lines = cache_size / line_size;
        for (int i = 0; i < lines; i++) {
            cache_entry entry(i, line_size, "", false, false);
            cache_entries.push_back(entry);
        }
        offset = int(log2(line_size));
        idx = int(log2(lines));
        T = 24 - (offset + idx);
        accesses = 0;
        hits = 0;
        misses = 0;
    }

    void access_sequence() {
        int totalcycles=0;
        
        for (auto access : seq_of_access) {
            int mem_address = access.first;
            char operation = access.second;

            string bin_address = decimal_binary(mem_address);
            string tag = bin_address.substr(0, T);
            int index = stoi(bin_address.substr(T, idx), 0, 2);
            int disp = stoi(bin_address.substr(T + idx, offset), 0, 2);
            bool hit = false;
            accesses++;
            cout << "Access: " << dec << accesses << endl;
            cout << "Getting address: 0x" << hex << mem_address << " (" << (operation == 'w' ? "Write" : "Read") << ")" << endl;
            
            if (cache_entries[index].Tag == tag && cache_entries[index].valid) {
                hit = true;
         
                    cout << "HIT!" << endl;
                hits++;
                if(operation=='w')
                {
                    
                    if(writeThrough)
                    {   
                        totalcycles += cycles+120; //simulating writing to memory and cache at the same time as there is a hit and write policy is write through
                        cout << "Writing to memory and cache as of Write Through policy....." << endl;
                    }
                    else
                    {
                        totalcycles +=cycles;
                        cache_entries[index].dirty=true; //simulating writing to cache only and changing dirty bit to true as write back policy for hit is used
                        cout << "Writing to cache as of Write Back Policy....." << endl;
                    }
                 
                }
                else
                { 
                    totalcycles +=cycles; //simulating reading from cache as there is a hit and it is a read operation
                }
            } else {
            
                misses++;
                    cout << "MISS!" << endl;
                if(cache_entries[index].dirty == true)
                {
                    totalcycles += 120;
                    cache_entries[index].dirty=false; // simuliating writing cache with dirty bit to memory after miss as data has not been written to memory yet
                    cout << "Wait... Dirty Cache entry found Writing data in cache entry to memory...." << endl;
                }

                if(operation!='w')
                {
                    cache_entries[index].Tag = tag;
                    cache_entries[index].valid = true;

                    totalcycles += cycles+120; //reading from memory
                    
                }
                else{
                    if(writeAllocate)
                    {   // simulating write allocate miss policy putting data of the address in the cache and writing based on the writing policy
                        cache_entries[index].Tag = tag;
                        cache_entries[index].valid = true;
                        if(writeThrough)
                        {
                            totalcycles += cycles+240+cycles;
                            cout << "Data fetched. Writing to memory and cache as of Write Through policy....." << endl;
                        }
                        else
                        {
                            totalcycles += cycles+120+cycles;
                            cache_entries[index].dirty=true;
                            cout << "Data fetched. Writing to cache as of Write Back Policy....." << endl;
                        }
                    }
                    else
                    {
                        totalcycles += cycles+120; // simulating no write allocate for miss writing to the memory directly
                        cout << "No Data fetched. Writing to memory directly as of no write allocate Policy....." << endl;
                    }
                }
            }

            
            for (auto entry : cache_entries) {
                cout << "Cache Entry " << dec << entry.index + 1 << ": Valid bit- " << entry.valid << ", Tag bits- " << entry.Tag << endl;
            }
            double miss_ratio = (double)misses / accesses;
            double hit_ratio = (double)hits / accesses;
        
            cout << "Total Number of Accesses: " << accesses << endl;
            cout << "Total number cycles spent accessing and writing data for whole sequence:" << totalcycles<< " cycles" <<endl;
            cout << "Hit Ratio: " << hit_ratio << endl;
            cout << "Miss Ratio: " << miss_ratio << endl;
            double AMAT = ((double)totalcycles/ accesses);
            cout << "Average Memory Access Time (AMAT): " << AMAT << " cycles" << endl
                 << endl;
        }
    }
};

void loading_data(string &filename) {
    ifstream file(filename);
    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            istringstream iss(line);
            string part;
            while (std::getline(iss, part, ',')) {
                int mem_add;
                char operation;
                stringstream ss(part);
                ss >> mem_add >> operation;
                memory[mem_add] = 0;
                seq_of_access.push_back(make_pair(mem_add, operation));
            }
        }
        file.close();
    } else {
        cout << "Unable to open file" << endl;
    }
}

int main() {
    int cacheSize, lineSize, cycles;
    string writePolicy_hit, writePolicy_miss;
    do {
        cout << "Enter cache size (in bytes) must be a power of 2 in the form 2^n: ";
        cin >> cacheSize;
    } while (!isPowerOfTwo(cacheSize));
    do {
        cout << "Enter cache line size (in bytes) must be a power of 2 in the form 2^n: ";
        cin >> lineSize;
    } while (!isPowerOfTwo(lineSize) || lineSize >= cacheSize);
    cout << "Enter number of cycles needed to access the cache: ";
    cin >> cycles;
    do {
           cout << "Enter write policy for hit (w for write-back, t for write-through): ";
           cin >> writePolicy_hit;
       } while (writePolicy_hit != "w" && writePolicy_hit != "t");
    do {
           cout << "Enter write policy for miss (1 for write-allocate, 2 for no write-allocate): ";
           cin >> writePolicy_miss;
       } while (writePolicy_miss != "1" && writePolicy_miss != "2");
    
    if(writePolicy_hit=="t")
        writeThrough=true;
    
    if(writePolicy_miss=="1")
        writeAllocate=true;

    
    
    string filename;
    cout << "Enter the name of the file containing the access sequence. (eg. input.txt)" << endl;
    cin >> filename;
    loading_data(filename);

    cache my_cache(cacheSize, lineSize, cycles);
    my_cache.access_sequence();

    return 0;
}

