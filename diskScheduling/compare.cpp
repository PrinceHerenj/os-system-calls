#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
using namespace std;

// Common test data
vector<int> requests = {98, 183, 37, 122, 14, 124, 65, 67};
int initialHead = 53;
int diskSize = 200;

// FCFS Algorithm
int fcfsAlgorithm(vector<int> req, int head) {
    int totalSeekTime = 0;
    int currentPosition = head;
    
    cout << "FCFS Sequence: " << head;
    for (int request : req) {
        totalSeekTime += abs(request - currentPosition);
        currentPosition = request;
        cout << " -> " << request;
    }
    cout << "\n";
    
    return totalSeekTime;
}

// SSTF Algorithm
int sstfAlgorithm(vector<int> req, int head) {
    int totalSeekTime = 0;
    int currentPosition = head;
    vector<bool> processed(req.size(), false);
    
    cout << "SSTF Sequence: " << head;
    
    for (int i = 0; i < req.size(); i++) {
        int minDistance = INT_MAX;
        int closestIndex = -1;
        
        // Find closest unprocessed request
        for (int j = 0; j < req.size(); j++) {
            if (!processed[j]) {
                int distance = abs(req[j] - currentPosition);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestIndex = j;
                }
            }
        }
        
        if (closestIndex != -1) {
            totalSeekTime += minDistance;
            currentPosition = req[closestIndex];
            processed[closestIndex] = true;
            cout << " -> " << currentPosition;
        }
    }
    cout << "\n";
    
    return totalSeekTime;
}

// SCAN Algorithm
int scanAlgorithm(vector<int> req, int head, string direction = "right") {
    int totalSeekTime = 0;
    int currentPosition = head;
    vector<int> left, right;
    
    // Separate requests
    for (int request : req) {
        if (request < head) {
            left.push_back(request);
        } else {
            right.push_back(request);
        }
    }
    
    sort(left.begin(), left.end(), greater<int>());
    sort(right.begin(), right.end());
    
    cout << "SCAN Sequence: " << head;
    
    if (direction == "right") {
        // Service right requests
        for (int request : right) {
            totalSeekTime += abs(request - currentPosition);
            currentPosition = request;
            cout << " -> " << request;
        }
        
        // Move to end if needed
        if (!right.empty() && currentPosition != diskSize - 1) {
            totalSeekTime += (diskSize - 1) - currentPosition;
            currentPosition = diskSize - 1;
            cout << " -> " << currentPosition;
        }
        
        // Service left requests
        for (int request : left) {
            totalSeekTime += abs(request - currentPosition);
            currentPosition = request;
            cout << " -> " << request;
        }
    }
    cout << "\n";
    
    return totalSeekTime;
}

// C-SCAN Algorithm
int cscanAlgorithm(vector<int> req, int head, string direction = "right") {
    int totalSeekTime = 0;
    int currentPosition = head;
    vector<int> left, right;
    
    // Separate requests
    for (int request : req) {
        if (request < head) {
            left.push_back(request);
        } else {
            right.push_back(request);
        }
    }
    
    sort(left.begin(), left.end());
    sort(right.begin(), right.end());
    
    cout << "C-SCAN Sequence: " << head;
    
    if (direction == "right") {
        // Service right requests
        for (int request : right) {
            totalSeekTime += abs(request - currentPosition);
            currentPosition = request;
            cout << " -> " << request;
        }
        
        // Move to end
        if (!left.empty()) {
            totalSeekTime += (diskSize - 1) - currentPosition;
            currentPosition = diskSize - 1;
            cout << " -> " << currentPosition;
            
            // Circular jump
            totalSeekTime += diskSize - 1;
            currentPosition = 0;
            cout << " -> " << currentPosition;
            
            // Service left requests
            for (int request : left) {
                totalSeekTime += abs(request - currentPosition);
                currentPosition = request;
                cout << " -> " << request;
            }
        }
    }
    cout << "\n";
    
    return totalSeekTime;
}

void displayHeader() {
    cout << "DISK SCHEDULING ALGORITHMS COMPARISON\n";
    cout << "=====================================\n\n";
    
    cout << "Test Configuration:\n";
    cout << "- Disk Size: 0 to " << diskSize - 1 << " cylinders\n";
    cout << "- Initial Head Position: " << initialHead << "\n";
    cout << "- Request Queue: ";
    for (int i = 0; i < requests.size(); i++) {
        cout << requests[i];
        if (i < requests.size() - 1) cout << ", ";
    }
    cout << "\n";
    cout << "- Total Requests: " << requests.size() << "\n\n";
}

void displaySummary(int fcfs, int sstf, int scan, int cscan) {
    cout << "\n" << string(80, '=') << "\n";
    cout << "PERFORMANCE COMPARISON SUMMARY\n";
    cout << string(80, '=') << "\n";
    
    cout << left << setw(12) << "Algorithm" 
         << setw(18) << "Total Seek Time" 
         << setw(20) << "Average Seek Time"
         << setw(15) << "Efficiency"
         << "Rank\n";
    cout << string(80, '-') << "\n";
    
    vector<pair<string, int>> results = {
        {"FCFS", fcfs},
        {"SSTF", sstf},
        {"SCAN", scan},
        {"C-SCAN", cscan}
    };
    
    // Sort by seek time for ranking
    vector<pair<int, string>> sorted;
    for (auto& result : results) {
        sorted.push_back({result.second, result.first});
    }
    sort(sorted.begin(), sorted.end());
    
    // Display results
    for (auto& result : results) {
        string name = result.first;
        int seekTime = result.second;
        double avgSeekTime = (double)seekTime / requests.size();
        
        // Find rank
        int rank = 1;
        for (auto& s : sorted) {
            if (s.second == name) break;
            rank++;
        }
        
        string efficiency;
        if (rank == 1) efficiency = "Excellent";
        else if (rank == 2) efficiency = "Good";
        else if (rank == 3) efficiency = "Fair";
        else efficiency = "Poor";
        
        cout << left << setw(12) << name
             << setw(18) << seekTime
             << setw(20) << fixed << setprecision(2) << avgSeekTime
             << setw(15) << efficiency
             << rank << "\n";
    }
    
    cout << "\nPerformance Analysis:\n";
    cout << "- Best Algorithm: " << sorted[0].second << " (" << sorted[0].first << " cylinders)\n";
    cout << "- Worst Algorithm: " << sorted[3].second << " (" << sorted[3].first << " cylinders)\n";
    cout << "- Performance Gap: " << sorted[3].first - sorted[0].first << " cylinders\n";
    cout << "- Improvement: " << fixed << setprecision(1) 
         << (double)(sorted[3].first - sorted[0].first) / sorted[3].first * 100 << "%\n";
}

void displayCharacteristics() {
    cout << "\n" << string(80, '=') << "\n";
    cout << "ALGORITHM CHARACTERISTICS\n";
    cout << string(80, '=') << "\n";
    
    cout << "\nFCFS (First Come First Serve):\n";
    cout << "✓ Simple implementation\n";
    cout << "✓ Fair to all requests\n";
    cout << "✗ Poor performance\n";
    cout << "✗ High seek time variance\n";
    cout << "✗ No optimization\n";
    
    cout << "\nSSTF (Shortest Seek Time First):\n";
    cout << "✓ Better performance than FCFS\n";
    cout << "✓ Locally optimal choices\n";
    cout << "⚠ May cause starvation\n";
    cout << "✗ Not globally optimal\n";
    
    cout << "\nSCAN (Elevator Algorithm):\n";
    cout << "✓ No starvation\n";
    cout << "✓ Uniform wait times\n";
    cout << "✓ Good overall performance\n";
    cout << "⚠ Extra movement to disk ends\n";
    
    cout << "\nC-SCAN (Circular SCAN):\n";
    cout << "✓ More uniform wait times\n";
    cout << "✓ Better for heavy loads\n";
    cout << "⚠ Higher seek time due to jumps\n";
    cout << "⚠ Circular movement overhead\n";
}

int main() {
    displayHeader();
    
    cout << "Executing Algorithms...\n";
    cout << string(50, '-') << "\n";
    
    int fcfsSeek = fcfsAlgorithm(requests, initialHead);
    cout << "FCFS Total Seek Time: " << fcfsSeek << " cylinders\n\n";
    
    int sstfSeek = sstfAlgorithm(requests, initialHead);
    cout << "SSTF Total Seek Time: " << sstfSeek << " cylinders\n\n";
    
    int scanSeek = scanAlgorithm(requests, initialHead);
    cout << "SCAN Total Seek Time: " << scanSeek << " cylinders\n\n";
    
    int cscanSeek = cscanAlgorithm(requests, initialHead);
    cout << "C-SCAN Total Seek Time: " << cscanSeek << " cylinders\n\n";
    
    displaySummary(fcfsSeek, sstfSeek, scanSeek, cscanSeek);
    displayCharacteristics();
    
    cout << "\n" << string(80, '=') << "\n";
    cout << "CONCLUSION\n";
    cout << string(80, '=') << "\n";
    cout << "For this test case:\n";
    cout << "- SSTF provides the best performance for this specific request pattern\n";
    cout << "- SCAN offers the best balance of performance and fairness\n";
    cout << "- FCFS is simple but inefficient\n";
    cout << "- C-SCAN provides uniform service but with higher overhead\n";
    cout << "\nChoice depends on system requirements:\n";
    cout << "- Interactive systems: SSTF or SCAN\n";
    cout << "- Fair access required: SCAN or C-SCAN\n";
    cout << "- Simple implementation: FCFS\n";
    cout << "- Heavy concurrent load: C-SCAN\n";
    
    return 0;
}