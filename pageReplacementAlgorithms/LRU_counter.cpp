#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
using namespace std;

int lruCounter(vector<int>& pages, int capacity) {
    unordered_map<int, int> counter; // page -> last used time
    vector<int> frame;
    int pageFaults = 0;
    int time = 0;
    
    cout << "LRU Counter Page Replacement Algorithm\n";
    cout << "======================================\n";
    cout << "Frame Capacity: " << capacity << "\n\n";
    
    for (int page : pages) {
        time++;
        cout << "Accessing page " << page << " (time=" << time << "): ";
        
        auto it = find(frame.begin(), frame.end(), page);
        
        if (it == frame.end()) {
            pageFaults++;
            cout << "Page fault! ";
            
            if (frame.size() == capacity) {
                int lruPage = frame[0];
                int lruTime = counter[frame[0]];
                
                // Find the least recently used page
                for (int p : frame) {
                    if (counter[p] < lruTime) {
                        lruTime = counter[p];
                        lruPage = p;
                    }
                }
                
                frame.erase(find(frame.begin(), frame.end(), lruPage));
                counter.erase(lruPage);
                cout << "Removed LRU page " << lruPage << " (last used at time " << lruTime << ") ";
            }
            frame.push_back(page);
            cout << "Added page " << page;
        } else {
            cout << "Page hit!";
        }
        
        counter[page] = time;
        
        // Display current frame state with timestamps
        cout << " | Frame: ";
        for (int p : frame) {
            cout << p << "(" << counter[p] << ") ";
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
    
    int faults = lruCounter(pages, capacity);
    
    cout << "\nTotal Page Faults: " << faults << endl;
    cout << "Page Fault Rate: " << (double)faults / pages.size() * 100 << "%" << endl;
    
    return 0;
}