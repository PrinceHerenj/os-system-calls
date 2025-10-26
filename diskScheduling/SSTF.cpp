#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
using namespace std;

class SSTFDiskScheduling {
private:
    vector<int> requests;
    vector<bool> processed;
    int head;
    int diskSize;
    int totalSeekTime;
    vector<int> seekSequence;
    
public:
    SSTFDiskScheduling(vector<int> req, int initialHead, int size) {
        requests = req;
        processed.resize(req.size(), false);
        head = initialHead;
        diskSize = size;
        totalSeekTime = 0;
    }
    
    void displayInput() {
        cout << "SSTF (Shortest Seek Time First) Disk Scheduling Algorithm\n";
        cout << "=========================================================\n\n";
        
        cout << "Disk Configuration:\n";
        cout << "- Disk Size: 0 to " << diskSize - 1 << " cylinders\n";
        cout << "- Initial Head Position: " << head << "\n";
        cout << "- Number of Requests: " << requests.size() << "\n\n";
        
        cout << "Request Queue: ";
        for (int i = 0; i < requests.size(); i++) {
            cout << requests[i];
            if (i < requests.size() - 1) cout << " -> ";
        }
        cout << "\n\n";
    }
    
    int findClosestRequest(int currentPosition) {
        int minDistance = INT_MAX;
        int closestIndex = -1;
        
        for (int i = 0; i < requests.size(); i++) {
            if (!processed[i]) {
                int distance = abs(requests[i] - currentPosition);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestIndex = i;
                }
            }
        }
        
        return closestIndex;
    }
    
    void sstfScheduling() {
        cout << "SSTF Execution Sequence:\n";
        cout << "Step\tFrom\tTo\tSeek Distance\tTotal Seek Time\tRemaining Requests\n";
        cout << "----\t----\t--\t-------------\t---------------\t------------------\n";
        
        int currentPosition = head;
        seekSequence.push_back(currentPosition);
        
        for (int step = 0; step < requests.size(); step++) {
            int closestIndex = findClosestRequest(currentPosition);
            
            if (closestIndex == -1) break;
            
            int nextRequest = requests[closestIndex];
            int seekDistance = abs(nextRequest - currentPosition);
            totalSeekTime += seekDistance;
            
            cout << (step + 1) << "\t" << currentPosition << "\t" << nextRequest 
                 << "\t" << seekDistance << "\t\t" << totalSeekTime << "\t\t";
            
            // Display remaining requests
            bool first = true;
            cout << "[";
            for (int i = 0; i < requests.size(); i++) {
                if (!processed[i] && i != closestIndex) {
                    if (!first) cout << ", ";
                    cout << requests[i];
                    first = false;
                }
            }
            cout << "]\n";
            
            processed[closestIndex] = true;
            currentPosition = nextRequest;
            seekSequence.push_back(currentPosition);
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
    
    void displayAnalysis() {
        cout << "\nStep-by-Step Analysis:\n";
        cout << "Starting at head position: " << head << "\n\n";
        
        int currentPos = head;
        for (int i = 1; i < seekSequence.size(); i++) {
            int nextPos = seekSequence[i];
            int distance = abs(nextPos - currentPos);
            
            cout << "Step " << i << ": Move from " << currentPos << " to " << nextPos;
            cout << " (distance = " << distance << ")\n";
            
            // Show which requests were considered
            cout << "  Available requests: ";
            vector<pair<int, int>> distances;
            for (int j = 0; j < requests.size(); j++) {
                bool wasProcessed = false;
                for (int k = 1; k <= i; k++) {
                    if (seekSequence[k] == requests[j]) {
                        wasProcessed = true;
                        break;
                    }
                }
                if (!wasProcessed || (i == 1 && requests[j] == nextPos)) {
                    if (requests[j] == nextPos && i > 1) continue;
                    distances.push_back({requests[j], abs(requests[j] - currentPos)});
                }
            }
            
            for (int d = 0; d < distances.size(); d++) {
                cout << distances[d].first << "(dist=" << distances[d].second << ")";
                if (d < distances.size() - 1) cout << ", ";
            }
            cout << "\n";
            cout << "  Closest request: " << nextPos << " (distance = " << distance << ")\n\n";
            
            currentPos = nextPos;
        }
    }
    
    void displayCharacteristics() {
        cout << "\nSSTF Characteristics:\n";
        cout << "- Selects request with minimum seek time from current position\n";
        cout << "- Reduces average seek time compared to FCFS\n";
        cout << "- Greedy algorithm (locally optimal choice)\n";
        cout << "- May cause starvation for requests far from current position\n";
        cout << "- Better performance than FCFS but not globally optimal\n";
        cout << "- Can lead to localized disk access patterns\n";
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
    
    SSTFDiskScheduling sstf(requests, initialHead, diskSize);
    
    sstf.displayInput();
    sstf.sstfScheduling();
    sstf.displayResults();
    sstf.displayGanttChart();
    sstf.displayAnalysis();
    sstf.displayCharacteristics();
    
    cout << "\nNote: SSTF always chooses the request that requires minimum\n";
    cout << "head movement from the current position, providing better\n";
    cout << "performance than FCFS but may cause starvation.\n";
    
    return 0;
}