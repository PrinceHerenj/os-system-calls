#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
using namespace std;

int findOptimal(vector<int>& pages, vector<int>& frame, int current) {
    int farthest = current, victim = -1;
    
    for (int page : frame) {
        int j;
        for (j = current; j < pages.size(); j++) {
            if (pages[j] == page) {
                if (j > farthest) {
                    farthest = j;
                    victim = page;
                }
                break;
            }
        }
        // If page is never referenced again, return it immediately
        if (j == pages.size()) return page;
    }
    return (victim == -1) ? frame[0] : victim;
}

int optimal(vector<int>& pages, int capacity) {
    vector<int> frame;
    int pageFaults = 0;
    
    cout << "Optimal Page Replacement Algorithm\n";
    cout << "==================================\n";
    cout << "Frame Capacity: " << capacity << "\n\n";
    
    for (int i = 0; i < pages.size(); i++) {
        cout << "Accessing page " << pages[i] << ": ";
        
        auto it = find(frame.begin(), frame.end(), pages[i]);
        
        if (it == frame.end()) {
            pageFaults++;
            cout << "Page fault! ";
            
            if (frame.size() == capacity) {
                int victim = findOptimal(pages, frame, i + 1);
                frame.erase(find(frame.begin(), frame.end(), victim));
                cout << "Removed page " << victim << " (optimal choice) ";
            }
            frame.push_back(pages[i]);
            cout << "Added page " << pages[i];
        } else {
            cout << "Page hit!";
        }
        
        // Display current frame state
        cout << " | Frame: ";
        for (int p : frame) {
            cout << p << " ";
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
    
    int faults = optimal(pages, capacity);
    
    cout << "\nTotal Page Faults: " << faults << endl;
    cout << "Page Fault Rate: " << (double)faults / pages.size() * 100 << "%" << endl;
    cout << "\nNote: Optimal algorithm has the lowest possible page fault rate" << endl;
    cout << "as it can see future page references." << endl;
    
    return 0;
}