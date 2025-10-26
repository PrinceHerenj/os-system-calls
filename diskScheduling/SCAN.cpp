#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
using namespace std;

class SCANDiskScheduling {
private:
    vector<int> requests;
    int head;
    int diskSize;
    int totalSeekTime;
    vector<int> seekSequence;
    string direction;
    
public:
    SCANDiskScheduling(vector<int> req, int initialHead, int size, string dir = "right") {
        requests = req;
        head = initialHead;
        diskSize = size;
        direction = dir;
        totalSeekTime = 0;
    }
    
    void displayInput() {
        cout << "SCAN (Elevator) Disk Scheduling Algorithm\n";
        cout << "=========================================\n\n";
        
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
    
    void scanScheduling() {
        cout << "SCAN Execution Sequence:\n";
        cout << "Step\tFrom\tTo\tSeek Distance\tTotal Seek Time\tDirection\n";
        cout << "----\t----\t--\t-------------\t---------------\t---------\n";
        
        vector<int> left, right;
        
        // Separate requests into left and right of current head position
        for (int request : requests) {
            if (request < head) {
                left.push_back(request);
            } else {
                right.push_back(request);
            }
        }
        
        // Sort left in descending order and right in ascending order
        sort(left.begin(), left.end(), greater<int>());
        sort(right.begin(), right.end());
        
        int currentPosition = head;
        seekSequence.push_back(currentPosition);
        int step = 1;
        
        if (direction == "right") {
            // First service all requests to the right
            for (int request : right) {
                int seekDistance = abs(request - currentPosition);
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << request 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tRight\n";
                
                currentPosition = request;
                seekSequence.push_back(currentPosition);
                step++;
            }
            
            // Move to end of disk if there were requests to the right
            if (!right.empty() && currentPosition != diskSize - 1) {
                int seekDistance = (diskSize - 1) - currentPosition;
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << (diskSize - 1) 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tRight (End)\n";
                
                currentPosition = diskSize - 1;
                seekSequence.push_back(currentPosition);
                step++;
            }
            
            // Then service requests to the left (in reverse direction)
            for (int request : left) {
                int seekDistance = abs(request - currentPosition);
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << request 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tLeft\n";
                
                currentPosition = request;
                seekSequence.push_back(currentPosition);
                step++;
            }
        } else {
            // First service all requests to the left
            for (int request : left) {
                int seekDistance = abs(request - currentPosition);
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << request 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tLeft\n";
                
                currentPosition = request;
                seekSequence.push_back(currentPosition);
                step++;
            }
            
            // Move to beginning of disk if there were requests to the left
            if (!left.empty() && currentPosition != 0) {
                int seekDistance = currentPosition - 0;
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << 0 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tLeft (Start)\n";
                
                currentPosition = 0;
                seekSequence.push_back(currentPosition);
                step++;
            }
            
            // Then service requests to the right (in reverse direction)
            for (int request : right) {
                int seekDistance = abs(request - currentPosition);
                totalSeekTime += seekDistance;
                
                cout << step << "\t" << currentPosition << "\t" << request 
                     << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\tRight\n";
                
                currentPosition = request;
                seekSequence.push_back(currentPosition);
                step++;
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
        
        sort(left.begin(), left.end(), greater<int>());
        sort(right.begin(), right.end());
        
        cout << "Requests to the LEFT of head (in service order): ";
        if (left.empty()) {
            cout << "None";
        } else {
            for (int i = 0; i < left.size(); i++) {
                cout << left[i];
                if (i < left.size() - 1) cout << " -> ";
            }
        }
        cout << "\n";
        
        cout << "Requests to the RIGHT of head (in service order): ";
        if (right.empty()) {
            cout << "None";
        } else {
            for (int i = 0; i < right.size(); i++) {
                cout << right[i];
                if (i < right.size() - 1) cout << " -> ";
            }
        }
        cout << "\n\n";
        
        cout << "Service Order (Direction: " << direction << "):\n";
        if (direction == "right") {
            cout << "1. Service RIGHT requests: ";
            for (int i = 0; i < right.size(); i++) {
                cout << right[i];
                if (i < right.size() - 1) cout << " -> ";
            }
            if (!right.empty()) cout << " -> End(" << (diskSize-1) << ")";
            cout << "\n";
            
            cout << "2. Service LEFT requests: ";
            for (int i = 0; i < left.size(); i++) {
                cout << left[i];
                if (i < left.size() - 1) cout << " -> ";
            }
            cout << "\n";
        } else {
            cout << "1. Service LEFT requests: ";
            for (int i = 0; i < left.size(); i++) {
                cout << left[i];
                if (i < left.size() - 1) cout << " -> ";
            }
            if (!left.empty()) cout << " -> Start(0)";
            cout << "\n";
            
            cout << "2. Service RIGHT requests: ";
            for (int i = 0; i < right.size(); i++) {
                cout << right[i];
                if (i < right.size() - 1) cout << " -> ";
            }
            cout << "\n";
        }
    }
    
    void displayCharacteristics() {
        cout << "\nSCAN Characteristics:\n";
        cout << "- Also known as 'Elevator Algorithm'\n";
        cout << "- Scans disk in one direction until end, then reverses\n";
        cout << "- Services all requests in current direction before reversing\n";
        cout << "- Provides uniform wait time for all requests\n";
        cout << "- Better than FCFS and SSTF for avoiding starvation\n";
        cout << "- May cause longer wait times for requests just missed\n";
        cout << "- Total seek time includes movement to disk boundaries\n";
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
    
    SCANDiskScheduling scan(requests, initialHead, diskSize, direction);
    
    scan.displayInput();
    scan.displayRequestSeparation();
    scan.scanScheduling();
    scan.displayResults();
    scan.displayGanttChart();
    scan.displayCharacteristics();
    
    cout << "\nNote: SCAN algorithm moves in one direction servicing all\n";
    cout << "requests until it reaches the end, then reverses direction.\n";
    cout << "This ensures no request waits indefinitely (no starvation).\n";
    
    return 0;
}