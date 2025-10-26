#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
using namespace std;

class CSCANDiskScheduling {
private:
    vector<int> requests;
    int head;
    int diskSize;
    int totalSeekTime;
    vector<int> seekSequence;
    string direction;
    
public:
    CSCANDiskScheduling(vector<int> req, int initialHead, int size, string dir = "right") {
        requests = req;
        head = initialHead;
        diskSize = size;
        direction = dir;
        totalSeekTime = 0;
    }
    
    void displayInput() {
        cout << "C-SCAN (Circular SCAN) Disk Scheduling Algorithm\n";
        cout << "================================================\n\n";
        
        cout << "Disk Configuration:\n";
        cout << "- Disk Size: 0 to " << diskSize - 1 << " cylinders\n";
        cout << "- Initial Head Position: " << head << "\n";
        cout << "- Initial Direction: " << direction << "\n";
        cout << "- Number of Requests: " << requests.size() << "\n\n";
        
        cout << "Request Queue: ";
        for (int i = 0; i < requests.size(); i++) {
            cout << requests[i];
            if (i < requests.size() - 1) cout << " -> ";
        }
        cout << "\n\n";
    }
    
    void cscanScheduling() {
        cout << "C-SCAN Execution Sequence:\n";
        cout << "Step\tFrom\tTo\tSeek Distance\tTotal Seek Time\tAction\n";
        cout << "----\t----\t--\t-------------\t---------------\t------\n";
        
        vector<int> left, right;
        
        // Separate requests into left and right of current head position
        for (int request : requests) {
            if (request < head) {
                left.push_back(request);
            } else {
                right.push_back(request);
            }
        }
        
        // Sort left in ascending order and right in ascending order
        sort(left.begin(), left.end());
        sort(right.begin(), right.end());
        
        int currentPosition = head;
        seekSequence.push_back(currentPosition);
        int step = 1;
        
        if (direction == "right") {
            // Service all requests to the right first
            for (int request : right) {
                int seekDistance = abs(request - currentPosition);
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << request 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tService Request\n";
                
                currentPosition = request;
                seekSequence.push_back(currentPosition);
                step++;
            }
            
            // Move to end of disk
            if (!left.empty()) {
                int seekDistance = (diskSize - 1) - currentPosition;
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << (diskSize - 1) 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tMove to End\n";
                
                currentPosition = diskSize - 1;
                seekSequence.push_back(currentPosition);
                step++;
                
                // Jump to beginning of disk (circular)
                seekDistance = diskSize - 1;  // From end to start
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << 0 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tCircular Jump\n";
                
                currentPosition = 0;
                seekSequence.push_back(currentPosition);
                step++;
                
                // Service requests to the left (now from beginning)
                for (int request : left) {
                    seekDistance = abs(request - currentPosition);
                    totalSeekTime += seekDistance;
                    
                    cout << step << "\t" << currentPosition << "\t" << request 
                         << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tService Request\n";
                    
                    currentPosition = request;
                    seekSequence.push_back(currentPosition);
                    step++;
                }
            }
        } else {
            // Service all requests to the left first (in descending order)
            reverse(left.begin(), left.end());
            for (int request : left) {
                int seekDistance = abs(request - currentPosition);
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << request 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tService Request\n";
                
                currentPosition = request;
                seekSequence.push_back(currentPosition);
                step++;
            }
            
            // Move to beginning of disk
            if (!right.empty()) {
                int seekDistance = currentPosition - 0;
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << 0 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tMove to Start\n";
                
                currentPosition = 0;
                seekSequence.push_back(currentPosition);
                step++;
                
                // Jump to end of disk (circular)
                seekDistance = diskSize - 1;  // From start to end
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << (diskSize - 1) 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tCircular Jump\n";
                
                currentPosition = diskSize - 1;
                seekSequence.push_back(currentPosition);
                step++;
                
                // Service requests to the right (now from end, in descending order)
                reverse(right.begin(), right.end());
                for (int request : right) {
                    seekDistance = abs(request - currentPosition);
                    totalSeekTime += seekDistance;
                    
                    cout << step << "\t" << currentPosition << "\t" << request 
                         << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tService Request\n";
                    
                    currentPosition = request;
                    seekSequence.push_back(currentPosition);
                    step++;
                }
            }
        }
    }
    
    void displayResults() {
        cout << "\nResults:\n";
        cout << "--------\n";
        cout << "Total Seek Time: " << totalSeekTime << " cylinders\n";
        cout << "Average Seek Time: " << fixed << setprecision(2) 
             << (double)totalSeekTime / requests.size() << " cylinders\n";
        
        cout << "\nSeek Sequence: ";
        for (int i = 0; i < seekSequence.size(); i++) {
            cout << seekSequence[i];
            if (i < seekSequence.size() - 1) cout << " -> ";
        }
        cout << "\n";
    }
    
    void displayGanttChart() {
        cout << "\nGantt Chart (Head Movement):\n";
        cout << "|";
        
        for (int i = 1; i < seekSequence.size(); i++) {
            cout << " " << seekSequence[i] << " |";
        }
        cout << "\n";
        
        // Display seek distances
        cout << " ";
        for (int i = 1; i < seekSequence.size(); i++) {
            int distance = abs(seekSequence[i] - seekSequence[i-1]);
            cout << "(" << distance << ")";
            if (i < seekSequence.size() - 1) cout << "  ";
        }
        cout << "\n";
    }
    
    void displayRequestSeparation() {
        cout << "\nRequest Separation Analysis:\n";
        cout << "Head Position: " << head << "\n";
        
        vector<int> left, right;
        
        for (int request : requests) {
            if (request < head) {
                left.push_back(request);
            } else {
                right.push_back(request);
            }
        }
        
        sort(left.begin(), left.end());
        sort(right.begin(), right.end());
        
        cout << "Requests to the LEFT of head: ";
        if (left.empty()) {
            cout << "None";
        } else {
            for (int i = 0; i < left.size(); i++) {
                cout << left[i];
                if (i < left.size() - 1) cout << ", ";
            }
        }
        cout << "\n";
        
        cout << "Requests to the RIGHT of head: ";
        if (right.empty()) {
            cout << "None";
        } else {
            for (int i = 0; i < right.size(); i++) {
                cout << right[i];
                if (i < right.size() - 1) cout << ", ";
            }
        }
        cout << "\n\n";
        
        cout << "C-SCAN Service Order (Direction: " << direction << "):\n";
        if (direction == "right") {
            cout << "1. Service RIGHT requests (ascending): ";
            for (int i = 0; i < right.size(); i++) {
                cout << right[i];
                if (i < right.size() - 1) cout << " -> ";
            }
            cout << "\n2. Move to end (" << (diskSize-1) << ") then jump to start (0)\n";
            cout << "3. Service LEFT requests (ascending): ";
            for (int i = 0; i < left.size(); i++) {
                cout << left[i];
                if (i < left.size() - 1) cout << " -> ";
            }
            cout << "\n";
        } else {
            cout << "1. Service LEFT requests (descending): ";
            for (int i = left.size() - 1; i >= 0; i--) {
                cout << left[i];
                if (i > 0) cout << " -> ";
            }
            cout << "\n2. Move to start (0) then jump to end (" << (diskSize-1) << ")\n";
            cout << "3. Service RIGHT requests (descending): ";
            for (int i = right.size() - 1; i >= 0; i--) {
                cout << right[i];
                if (i > 0) cout << " -> ";
            }
            cout << "\n";
        }
    }
    
    void displayComparison() {
        cout << "\nC-SCAN vs SCAN Comparison:\n";
        cout << "---------------------------\n";
        cout << "SCAN (Elevator):\n";
        cout << "- Reverses direction at disk ends\n";
        cout << "- May cause longer waits for requests just missed\n";
        cout << "- Non-uniform wait times\n\n";
        
        cout << "C-SCAN (Circular SCAN):\n";
        cout << "- Always scans in same direction\n";
        cout << "- Provides more uniform wait times\n";
        cout << "- Treats disk as circular queue\n";
        cout << "- Better for heavy load conditions\n";
    }
    
    void displayCharacteristics() {
        cout << "\nC-SCAN Characteristics:\n";
        cout << "- Circular version of SCAN algorithm\n";
        cout << "- Scans in one direction only, then jumps to other end\n";
        cout << "- Provides more uniform wait time than SCAN\n";
        cout << "- Treats disk as a circular list\n";
        cout << "- Better performance under heavy load\n";
        cout << "- Avoids bias toward middle cylinders\n";
        cout << "- Higher seek time due to circular jumps\n";
    }
    
    int getTotalSeekTime() {
        return totalSeekTime;
    }
};

int main() {
    // Common test data for all disk scheduling algorithms
    vector<int> requests = {98, 183, 37, 122, 14, 124, 65, 67};
    int initialHead = 53;
    int diskSize = 200;
    string direction = "right";  // or "left"
    
    CSCANDiskScheduling cscan(requests, initialHead, diskSize, direction);
    
    cscan.displayInput();
    cscan.displayRequestSeparation();
    cscan.cscanScheduling();
    cscan.displayResults();
    cscan.displayGanttChart();
    cscan.displayComparison();
    cscan.displayCharacteristics();
    
    cout << "\nNote: C-SCAN provides more uniform response times by\n";
    cout << "always scanning in the same direction and treating the\n";
    cout << "disk as a circular structure.\n";
    
    return 0;
}