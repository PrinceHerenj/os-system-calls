#include <iostream>
#include <vector>
using namespace std;

int referenceBit(vector<int>& pages, int capacity) {
    vector<pair<int, int>> frame; // page, reference bit
    int pageFaults = 0;
    
    cout << "Reference Bit Page Replacement Algorithm\n";
    cout << "========================================\n";
    cout << "Frame Capacity: " << capacity << "\n\n";
    
    for (int page : pages) {
        cout << "Accessing page " << page << ": ";
        
        bool found = false;
        for (auto& p : frame) {
            if (p.first == page) {
                p.second = 1;
                found = true;
                cout << "Page hit! Set reference bit to 1";
                break;
            }
        }
        
        if (!found) {
            pageFaults++;
            cout << "Page fault! ";
            
            if (frame.size() == capacity) {
                // Find first page with reference bit 0
                while (true) {
                    if (frame[0].second == 0) {
                        cout << "Removed page " << frame[0].first << " (ref bit = 0) ";
                        frame.erase(frame.begin());
                        break;
                    } else {
                        cout << "Page " << frame[0].first << " has ref bit = 1, set to 0 and move to end ";
                        frame[0].second = 0;
                        frame.push_back(frame[0]);
                        frame.erase(frame.begin());
                    }
                }
            }
            frame.push_back({page, 1});
            cout << "Added page " << page << " (ref bit = 1)";
        }
        
        // Display current frame state with reference bits
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
    
    cout << "Page Reference String: ";
    for (int p : pages) cout << p << " ";
    cout << "\n\n";
    
    int faults = referenceBit(pages, capacity);
    
    cout << "\nTotal Page Faults: " << faults << endl;
    cout << "Page Fault Rate: " << (double)faults / pages.size() * 100 << "%" << endl;
    cout << "\nNote: Reference bit algorithm gives second chance to pages" << endl;
    cout << "by checking and clearing reference bits before replacement." << endl;
    
    return 0;
}