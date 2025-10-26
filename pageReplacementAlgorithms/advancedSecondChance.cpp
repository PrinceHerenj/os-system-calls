#include <iostream>
#include <vector>
#include <tuple>
using namespace std;

int advancedSecondChance(vector<int>& pages, int capacity) {
    vector<tuple<int, bool, bool>> frame; // page, reference, modified
    int pageFaults = 0;
    int pointer = 0;
    
    cout << "Advanced Second Chance Page Replacement Algorithm\n";
    cout << "================================================\n";
    cout << "Frame Capacity: " << capacity << "\n";
    cout << "Class priorities: (0,0) > (0,1) > (1,0) > (1,1)\n";
    cout << "Format: page(ref,mod)\n\n";
    
    for (int page : pages) {
        cout << "Accessing page " << page << ": ";
        
        bool found = false;
        
        // Check if page exists in frame
        for (auto& [p, ref, mod] : frame) {
            if (p == page) {
                ref = true;
                found = true;
                cout << "Page hit! Set reference bit to 1";
                break;
            }
        }
        
        if (!found) {
            pageFaults++;
            cout << "Page fault! ";
            
            if (frame.size() == capacity) {
                // Find victim using the four classes
                // Class 0: (0,0) - not recently used, not modified
                // Class 1: (0,1) - not recently used, modified
                // Class 2: (1,0) - recently used, not modified
                // Class 3: (1,1) - recently used, modified
                
                bool found_victim = false;
                
                // First pass: look for class 0 (0,0)
                for (int i = 0; i < frame.size() && !found_victim; i++) {
                    int idx = (pointer + i) % frame.size();
                    auto& [p, ref, mod] = frame[idx];
                    if (!ref && !mod) {
                        cout << "Removed page " << p << " (class 0: ref=0,mod=0) ";
                        frame.erase(frame.begin() + idx);
                        pointer = idx % frame.size();
                        found_victim = true;
                    }
                }
                
                // Second pass: look for class 1 (0,1)
                for (int i = 0; i < frame.size() && !found_victim; i++) {
                    int idx = (pointer + i) % frame.size();
                    auto& [p, ref, mod] = frame[idx];
                    if (!ref && mod) {
                        cout << "Removed page " << p << " (class 1: ref=0,mod=1) ";
                        frame.erase(frame.begin() + idx);
                        pointer = idx % frame.size();
                        found_victim = true;
                    }
                }
                
                // Third pass: clear reference bits and look for class 0 again
                if (!found_victim) {
                    for (auto& [p, ref, mod] : frame) {
                        ref = false;
                    }
                    
                    for (int i = 0; i < frame.size() && !found_victim; i++) {
                        int idx = (pointer + i) % frame.size();
                        auto& [p, ref, mod] = frame[idx];
                        if (!ref && !mod) {
                            cout << "Removed page " << p << " (class 0 after clearing ref bits) ";
                            frame.erase(frame.begin() + idx);
                            pointer = idx % frame.size();
                            found_victim = true;
                        }
                    }
                }
                
                // Fourth pass: look for class 1 (0,1) after clearing reference bits
                for (int i = 0; i < frame.size() && !found_victim; i++) {
                    int idx = (pointer + i) % frame.size();
                    auto& [p, ref, mod] = frame[idx];
                    if (!ref && mod) {
                        cout << "Removed page " << p << " (class 1 after clearing ref bits) ";
                        frame.erase(frame.begin() + idx);
                        pointer = idx % frame.size();
                        found_victim = true;
                    }
                }
            }
            
            // Add new page (assume not modified initially)
            frame.push_back({page, true, false});
            cout << "Added page " << page << " (ref=1,mod=0)";
            if (frame.size() <= capacity) {
                pointer = 0;
            }
        }
        
        // Display current frame state
        cout << " | Frame: ";
        for (auto& [p, ref, mod] : frame) {
            cout << p << "(" << ref << "," << mod << ") ";
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
    
    int faults = advancedSecondChance(pages, capacity);
    
    cout << "\nTotal Page Faults: " << faults << endl;
    cout << "Page Fault Rate: " << (double)faults / pages.size() * 100 << "%" << endl;
    cout << "\nNote: Advanced Second Chance algorithm considers both" << endl;
    cout << "reference and modify bits to classify pages into 4 classes." << endl;
    cout << "It replaces pages in order of class priority." << endl;
    
    return 0;
}