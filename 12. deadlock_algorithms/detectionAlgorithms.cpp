#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#define N 5 // number of processes
#define M 3 // types of resources

class DeadlockDetection {
private:
    vector<vector<int>> allocation;
    vector<vector<int>> request;
    vector<int> available;
    vector<bool> finish;
    
public:
    DeadlockDetection() {
        allocation.resize(N, vector<int>(M));
        request.resize(N, vector<int>(M));
        available.resize(M);
        finish.resize(N);
    }
    
    void displayMatrix(const vector<vector<int>>& matrix, const string& name) {
        cout << "\n" << name << " Matrix:\n";
        cout << "Process\t";
        for (int j = 0; j < M; j++) {
            cout << "R" << j << "\t";
        }
        cout << "\n";
        
        for (int i = 0; i < N; i++) {
            cout << "P" << i << "\t";
            for (int j = 0; j < M; j++) {
                cout << matrix[i][j] << "\t";
            }
            cout << "\n";
        }
    }
    
    void displayAvailable() {
        cout << "\nAvailable Resources: ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << available[i] << " ";
        }
        cout << "\n";
    }
    
    bool detectionAlgorithm() {
        cout << "\n=== Deadlock Detection Algorithm ===\n";
        
        // Initialize work array with available resources
        vector<int> work = available;
        
        // Initialize finish array
        for (int i = 0; i < N; i++) {
            bool hasResources = false;
            for (int j = 0; j < M; j++) {
                if (allocation[i][j] > 0) {
                    hasResources = true;
                    break;
                }
            }
            finish[i] = !hasResources; // true if process holds no resources
        }
        
        cout << "\nInitial state:\n";
        cout << "Work array: ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << work[i] << " ";
        }
        cout << "\n";
        
        cout << "Finish array: ";
        for (int i = 0; i < N; i++) {
            cout << "P" << i << "=" << (finish[i] ? "T" : "F") << " ";
        }
        cout << "\n\n";
        
        vector<int> processOrder;
        int step = 1;
        
        while (true) {
            bool found = false;
            
            for (int i = 0; i < N; i++) {
                if (!finish[i]) {
                    bool canComplete = true;
                    
                    // Check if Request[i] <= Work
                    for (int j = 0; j < M; j++) {
                        if (request[i][j] > work[j]) {
                            canComplete = false;
                            break;
                        }
                    }
                    
                    if (canComplete) {
                        cout << "Step " << step << ": Process P" << i << " can complete\n";
                        cout << "  Request P" << i << ": ";
                        for (int j = 0; j < M; j++) {
                            cout << "R" << j << "=" << request[i][j] << " ";
                        }
                        cout << " <= Work: ";
                        for (int j = 0; j < M; j++) {
                            cout << "R" << j << "=" << work[j] << " ";
                        }
                        cout << "\n";
                        
                        // Work = Work + Allocation[i]
                        for (int j = 0; j < M; j++) {
                            work[j] += allocation[i][j];
                        }
                        
                        cout << "  Updated Work: ";
                        for (int j = 0; j < M; j++) {
                            cout << "R" << j << "=" << work[j] << " ";
                        }
                        cout << "\n";
                        
                        finish[i] = true;
                        processOrder.push_back(i);
                        found = true;
                        step++;
                        break;
                    }
                }
            }
            
            if (!found) break;
        }
        
        // Check for deadlock
        vector<int> deadlockedProcesses;
        for (int i = 0; i < N; i++) {
            if (!finish[i]) {
                deadlockedProcesses.push_back(i);
            }
        }
        
        cout << "\n=== Detection Result ===\n";
        
        if (deadlockedProcesses.empty()) {
            cout << "No deadlock detected!\n";
            cout << "All processes can complete in order: ";
            for (int i = 0; i < processOrder.size(); i++) {
                cout << "P" << processOrder[i];
                if (i < processOrder.size() - 1) cout << " -> ";
            }
            cout << "\n";
            return false;
        } else {
            cout << "DEADLOCK DETECTED!\n";
            cout << "Deadlocked processes: ";
            for (int i = 0; i < deadlockedProcesses.size(); i++) {
                cout << "P" << deadlockedProcesses[i];
                if (i < deadlockedProcesses.size() - 1) cout << ", ";
            }
            cout << "\n";
            
            cout << "\nProcesses that completed: ";
            if (processOrder.empty()) {
                cout << "None";
            } else {
                for (int i = 0; i < processOrder.size(); i++) {
                    cout << "P" << processOrder[i];
                    if (i < processOrder.size() - 1) cout << " -> ";
                }
            }
            cout << "\n";
            return true;
        }
    }
    
    void setTestData1() {
        cout << "\n=== Test Case 1: No Deadlock ===\n";
        
        // Available resources
        available = {0, 0, 0};
        
        // Current allocation
        allocation = {{0, 1, 0},
                     {2, 0, 0},
                     {3, 0, 3},
                     {2, 1, 1},
                     {0, 0, 2}};
        
        // Current requests
        request = {{0, 0, 0},
                  {2, 0, 2},
                  {0, 0, 0},
                  {1, 0, 0},
                  {0, 0, 2}};
    }
    
    void setTestData2() {
        cout << "\n=== Test Case 2: Deadlock Present ===\n";
        
        // Available resources
        available = {0, 0, 0};
        
        // Current allocation
        allocation = {{0, 1, 0},
                     {2, 0, 0},
                     {3, 0, 3},
                     {2, 1, 1},
                     {0, 0, 2}};
        
        // Current requests (modified to create deadlock)
        request = {{0, 0, 1},
                  {1, 0, 1},
                  {2, 1, 0},
                  {0, 1, 0},
                  {2, 0, 0}};
    }
    
    void setTestData3() {
        cout << "\n=== Test Case 3: Mixed Scenario ===\n";
        
        // Available resources
        available = {1, 0, 2};
        
        // Current allocation
        allocation = {{2, 1, 0},
                     {0, 1, 2},
                     {1, 0, 3},
                     {1, 1, 0},
                     {0, 1, 1}};
        
        // Current requests
        request = {{1, 0, 1},
                  {0, 1, 0},
                  {2, 0, 0},
                  {0, 0, 1},
                  {1, 0, 0}};
    }
    
    void runDetection() {
        displayAvailable();
        displayMatrix(allocation, "Allocation");
        displayMatrix(request, "Request");
        
        bool hasDeadlock = detectionAlgorithm();
        
        cout << "\n" << string(50, '=') << "\n";
    }
};

int main() {
    cout << "DEADLOCK DETECTION ALGORITHMS\n";
    cout << "==============================\n";
    cout << "Number of Processes: " << N << "\n";
    cout << "Number of Resource Types: " << M << "\n";
    
    DeadlockDetection detector;
    
    // Test Case 1: No Deadlock
    detector.setTestData1();
    detector.runDetection();
    
    // Test Case 2: Deadlock Present
    detector.setTestData2();
    detector.runDetection();
    
    // Test Case 3: Mixed Scenario
    detector.setTestData3();
    detector.runDetection();
    
    cout << "\nAlgorithm Explanation:\n";
    cout << "1. Initialize Work = Available\n";
    cout << "2. Initialize Finish[i] = false if process holds resources, true otherwise\n";
    cout << "3. Find process i where Finish[i] = false and Request[i] <= Work\n";
    cout << "4. Set Work = Work + Allocation[i] and Finish[i] = true\n";
    cout << "5. Repeat until no such process found\n";
    cout << "6. If any Finish[i] = false, deadlock exists\n";
    
    return 0;
}