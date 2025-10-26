#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
using namespace std;

class FCFSDiskScheduling {
private:
    vector<int> requests;
    int head;
    int diskSize;
    int totalSeekTime;
    vector<int> seekSequence;
    
public:
    FCFSDiskScheduling(vector<int> req, int initialHead, int size) {
        requests = req;
        head = initialHead;
        diskSize = size;
        totalSeekTime = 0;
    }
    
    void displayInput() {
        cout << "FCFS (First Come First Serve) Disk Scheduling Algorithm\n";
        cout << "=======================================================\n\n";
        
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
    
    void fcfsScheduling() {
        cout << "FCFS Execution Sequence:\n";
        cout << "Step\tFrom\tTo\tSeek Distance\tTotal Seek Time\n";
        cout << "----\t----\t--\t-------------\t---------------\n";
        
        int currentPosition = head;
        seekSequence.push_back(currentPosition);
        
        for (int i = 0; i < requests.size(); i++) {
            int seekDistance = abs(requests[i] - currentPosition);
            totalSeekTime += seekDistance;
            
            cout << (i + 1) << "\t" << currentPosition << "\t" << requests[i] 
                 << "\t" << seekDistance << "\t\t" << totalSeekTime << "\n";
            
            currentPosition = requests[i];
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
    
    void displayCharacteristics() {
        cout << "\nFCFS Characteristics:\n";
        cout << "- Simple and fair algorithm\n";
        cout << "- Processes requests in arrival order\n";
        cout << "- No reordering of requests\n";
        cout << "- May cause excessive head movement\n";
        cout << "- Can lead to high seek time variance\n";
        cout << "- No optimization for disk performance\n";
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
    
    FCFSDiskScheduling fcfs(requests, initialHead, diskSize);
    
    fcfs.displayInput();
    fcfs.fcfsScheduling();
    fcfs.displayResults();
    fcfs.displayGanttChart();
    fcfs.displayCharacteristics();
    
    cout << "\nNote: FCFS serves requests in the exact order they arrive,\n";
    cout << "without considering the current head position for optimization.\n";
    
    return 0;
}