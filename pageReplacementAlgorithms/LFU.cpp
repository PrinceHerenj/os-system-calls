#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
using namespace std;

int lfu(vector<int>& pages, int capacity) {
    unordered_map<int, int> freq;
    vector<int> frame;
    int pageFaults = 0;
    
    cout << "LFU (Least Frequently Used) Page Replacement Algorithm\n";
    cout << "======================================================\n";
    cout << "Frame Capacity: " << capacity << "\n\n";
    
    for (int page : pages) {
        cout << "Accessing page " << page << ": ";
        
        auto it = find(frame.begin(), frame.end(), page);
        
        if (it == frame.end()) {
            pageFaults++;
            cout << "Page fault! ";
            
            if (frame.size() == capacity) {
                int lfuPage = frame[0];
                int minFreq = freq[frame[0]];
                
                // Find the least frequently used page
                for (int p : frame) {
                    if (freq[p] < minFreq) {
                        minFreq = freq[p];
                        lfuPage = p;
                    }
                }
                
                frame.erase(find(frame.begin(), frame.end(), lfuPage));
                freq.erase(lfuPage);
                cout << "Removed LFU page " << lfuPage << " (frequency=" << minFreq << ") ";
            }
            frame.push_back(page);
            cout << "Added page " << page;
        } else {
            cout << "Page hit!";
        }
        
        freq[page]++;
        
        // Display current frame state with frequencies
        cout << " | Frame: ";
        for (int p : frame) {
            cout << p << "(freq=" << freq[p] << ") ";
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
    
    int faults = lfu(pages, capacity);
    
    cout << "\nTotal Page Faults: " << faults << endl;
    cout << "Page Fault Rate: " << (double)faults / pages.size() * 100 << "%" << endl;
    
    return 0;
}