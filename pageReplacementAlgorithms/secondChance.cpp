#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

int secondChance(vector<int>& pages, int capacity) {
    queue<int> q;
    unordered_map<int, bool> refBit;
    int pageFaults = 0;
    
    cout << "Second Chance Page Replacement Algorithm\n";
    cout << "========================================\n";
    cout << "Frame Capacity: " << capacity << "\n\n";
    
    for (int page : pages) {
        cout << "Accessing page " << page << ": ";
        
        if (refBit.find(page) == refBit.end()) {
            pageFaults++;
            cout << "Page fault! ";
            
            if (q.size() == capacity) {
                // Find a page with reference bit 0
                while (true) {
                    int front = q.front();
                    q.pop();
                    if (refBit[front] == false) {
                        refBit.erase(front);
                        cout << "Removed page " << front << " (ref bit = 0) ";
                        break;
                    } else {
                        refBit[front] = false;
                        q.push(front);
                        cout << "Page " << front << " gets second chance (ref bit set to 0) ";
                    }
                }
            }
            q.push(page);
            refBit[page] = false;
            cout << "Added page " << page << " (ref bit = 0)";
        } else {
            refBit[page] = true;
            cout << "Page hit! Set reference bit to 1";
        }
        
        // Display current queue state with reference bits
        cout << " | Queue: ";
        queue<int> temp = q;
        while (!temp.empty()) {
            int p = temp.front();
            temp.pop();
            cout << p << "(ref=" << refBit[p] << ") ";
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
    
    int faults = secondChance(pages, capacity);
    
    cout << "\nTotal Page Faults: " << faults << endl;
    cout << "Page Fault Rate: " << (double)faults / pages.size() * 100 << "%" << endl;
    cout << "\nNote: Second Chance algorithm is a modification of FIFO" << endl;
    cout << "that gives pages a 'second chance' if they were recently referenced." << endl;
    
    return 0;
}