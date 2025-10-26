#include <iostream>
#include <vector>
using namespace std;

int additionalReferenceBit(vector<int>& pages, int capacity, int bits = 8) {
    vector<pair<int, int>> frame; // page, reference byte
    int pageFaults = 0;
    
    cout << "Additional Reference Bit Page Replacement Algorithm\n";
    cout << "==================================================\n";
    cout << "Frame Capacity: " << capacity << "\n";
    cout << "Reference Bits: " << bits << "\n\n";
    
    for (int page : pages) {
        cout << "Accessing page " << page << ": ";
        
        bool found = false;
        
        // Shift all reference bytes right
        for (auto& p : frame) {
            p.second >>= 1;
        }
        
        // Check if page exists and set MSB
        for (auto& p : frame) {
            if (p.first == page) {
                p.second |= (1 << (bits - 1));
                found = true;
                cout << "Page hit! Set MSB to 1";
                break;
            }
        }
        
        if (!found) {
            pageFaults++;
            cout << "Page fault! ";
            
            if (frame.size() == capacity) {
                int minVal = frame[0].second;
                int victim = 0;
                
                // Find page with smallest reference byte value
                for (int i = 1; i < frame.size(); i++) {
                    if (frame[i].second < minVal) {
                        minVal = frame[i].second;
                        victim = i;
                    }
                }
                
                cout << "Removed page " << frame[victim].first 
                     << " (ref byte = " << frame[victim].second << ") ";
                frame.erase(frame.begin() + victim);
            }
            frame.push_back({page, 1 << (bits - 1)});
            cout << "Added page " << page << " (ref byte = " << (1 << (bits - 1)) << ")";
        }
        
        // Display current frame state with reference bytes
        cout << " | Frame: ";
        for (auto& p : frame) {
            cout << p.first << "(ref=" << p.second << ") ";
        }
        cout << "\n";
    }
    
    return pageFaults;
}

int main() {
    vector<int> pages = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    int capacity = 3;
    int bits = 8;
    
    cout << "Page Reference String: ";
    for (int p : pages) cout << p << " ";
    cout << "\n\n";
    
    int faults = additionalReferenceBit(pages, capacity, bits);
    
    cout << "\nTotal Page Faults: " << faults << endl;
    cout << "Page Fault Rate: " << (double)faults / pages.size() * 100 << "%" << endl;
    cout << "\nNote: Additional Reference Bit algorithm uses " << bits << " bits" << endl;
    cout << "to track the history of page references. The page with the" << endl;
    cout << "smallest reference byte value is replaced." << endl;
    
    return 0;
}