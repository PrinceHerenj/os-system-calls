#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
using namespace std;

int fifo(vector<int>& pages, int capacity) {
    queue<int> q;
    unordered_set<int> s;
    int pageFaults = 0;
    
    cout << "FIFO Page Replacement Algorithm\n";
    cout << "================================\n";
    cout << "Frame Capacity: " << capacity << "\n\n";
    
    for (int page : pages) {
        cout << "Accessing page " << page << ": ";
        
        if (s.find(page) == s.end()) {
            pageFaults++;
            cout << "Page fault! ";
            
            if (q.size() == capacity) {
                int old = q.front();
                q.pop();
                s.erase(old);
                cout << "Removed page " << old << " ";
            }
            
            q.push(page);
            s.insert(page);
            cout << "Added page " << page;
        } else {
            cout << "Page hit!";
        }
        
        // Display current frame state
        cout << " | Frame: ";
        queue<int> temp = q;
        while (!temp.empty()) {
            cout << temp.front() << " ";
            temp.pop();
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
    
    int faults = fifo(pages, capacity);
    
    cout << "\nTotal Page Faults: " << faults << endl;
    cout << "Page Fault Rate: " << (double)faults / pages.size() * 100 << "%" << endl;
    
    return 0;
}