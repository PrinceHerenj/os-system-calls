#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
using namespace std;

int lruStack(vector<int>& pages, int capacity) {
    list<int> stack;
    unordered_map<int, list<int>::iterator> map;
    int pageFaults = 0;
    
    cout << "LRU Stack Page Replacement Algorithm\n";
    cout << "====================================\n";
    cout << "Frame Capacity: " << capacity << "\n\n";
    
    for (int page : pages) {
        cout << "Accessing page " << page << ": ";
        
        if (map.find(page) == map.end()) {
            pageFaults++;
            cout << "Page fault! ";
            
            if (stack.size() == capacity) {
                int lru = stack.back();
                stack.pop_back();
                map.erase(lru);
                cout << "Removed LRU page " << lru << " ";
            }
        } else {
            cout << "Page hit! ";
            stack.erase(map[page]);
            cout << "Moved page " << page << " to front ";
        }
        
        stack.push_front(page);
        map[page] = stack.begin();
        
        // Display current stack state (front to back = MRU to LRU)
        cout << "| Stack (MRU->LRU): ";
        for (auto it = stack.begin(); it != stack.end(); ++it) {
            cout << *it << " ";
        }
        cout << "\n";
    }
    
    return pageFaults;
}

int main() {
    vector<int> pages = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    int capacity = 3;
    
    cout << "Page Reference String: ";
    for (int p : pages) cout << p << " ";
    cout << "\n\n";
    
    int faults = lruStack(pages, capacity);
    
    cout << "\nTotal Page Faults: " << faults << endl;
    cout << "Page Fault Rate: " << (double)faults / pages.size() * 100 << "%" << endl;
    
    return 0;
}