#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
using namespace std;

#define N 5 // number of processes
#define M 3 // types of resources

class ResourceAllocation {
private:
    vector<vector<int>> allocation;
    vector<vector<int>> max;
    vector<vector<int>> need;
    vector<int> available;
    vector<int> totalResources;
    
public:
    ResourceAllocation() {
        allocation.resize(N, vector<int>(M));
        max.resize(N, vector<int>(M));
        need.resize(N, vector<int>(M));
        available.resize(M);
        totalResources.resize(M);
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
    
    void displayVector(const vector<int>& vec, const string& name) {
        cout << "\n" << name << ": ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << vec[i] << " ";
        }
        cout << "\n";
    }
    
    void calculateNeed() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                need[i][j] = max[i][j] - allocation[i][j];
            }
        }
    }
    
    bool isSafeState(vector<int> work) {
        vector<bool> finish(N, false);
        vector<int> safeSequence;
        
        cout << "\n=== Safety Algorithm ===\n";
        cout << "Initial Work: ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << work[i] << " ";
        }
        cout << "\n\n";
        
        int step = 1;
        
        while (true) {
            bool found = false;
            
            for (int i = 0; i < N; i++) {
                if (!finish[i]) {
                    bool canAllocate = true;
                    
                    for (int j = 0; j < M; j++) {
                        if (need[i][j] > work[j]) {
                            canAllocate = false;
                            break;
                        }
                    }
                    
                    if (canAllocate) {
                        cout << "Step " << step << ": Process P" << i << " can complete\n";
                        cout << "  Need P" << i << ": ";
                        for (int j = 0; j < M; j++) {
                            cout << "R" << j << "=" << need[i][j] << " ";
                        }
                        cout << " <= Work: ";
                        for (int j = 0; j < M; j++) {
                            cout << "R" << j << "=" << work[j] << " ";
                        }
                        cout << "\n";
                        
                        for (int j = 0; j < M; j++) {
                            work[j] += allocation[i][j];
                        }
                        
                        cout << "  Updated Work: ";
                        for (int j = 0; j < M; j++) {
                            cout << "R" << j << "=" << work[j] << " ";
                        }
                        cout << "\n\n";
                        
                        finish[i] = true;
                        safeSequence.push_back(i);
                        found = true;
                        step++;
                        break;
                    }
                }
            }
            
            if (!found) break;
        }
        
        // Check if all processes finished
        for (int i = 0; i < N; i++) {
            if (!finish[i]) {
                cout << "System is NOT in safe state!\n";
                cout << "Processes that cannot complete: ";
                for (int j = 0; j < N; j++) {
                    if (!finish[j]) {
                        cout << "P" << j << " ";
                    }
                }
                cout << "\n";
                return false;
            }
        }
        
        cout << "System is in SAFE state!\n";
        cout << "Safe sequence: ";
        for (int i = 0; i < safeSequence.size(); i++) {
            cout << "P" << safeSequence[i];
            if (i < safeSequence.size() - 1) cout << " -> ";
        }
        cout << "\n";
        
        return true;
    }
    
    bool requestResources(int processId, vector<int> request) {
        cout << "\n" << string(60, '=') << "\n";
        cout << "RESOURCE REQUEST by Process P" << processId << "\n";
        cout << string(60, '=') << "\n";
        
        cout << "Requested: ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << request[i] << " ";
        }
        cout << "\n";
        
        // Step 1: Check if Request <= Need
        cout << "\nStep 1: Check if Request <= Need\n";
        bool validRequest = true;
        for (int i = 0; i < M; i++) {
            if (request[i] > need[processId][i]) {
                validRequest = false;
                break;
            }
        }
        
        cout << "Request: ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << request[i] << " ";
        }
        cout << "\nNeed P" << processId << ": ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << need[processId][i] << " ";
        }
        cout << "\n";
        
        if (!validRequest) {
            cout << "ERROR: Request exceeds maximum need!\n";
            return false;
        }
        cout << "✓ Request is valid (Request <= Need)\n";
        
        // Step 2: Check if Request <= Available
        cout << "\nStep 2: Check if Request <= Available\n";
        bool resourcesAvailable = true;
        for (int i = 0; i < M; i++) {
            if (request[i] > available[i]) {
                resourcesAvailable = false;
                break;
            }
        }
        
        cout << "Request: ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << request[i] << " ";
        }
        cout << "\nAvailable: ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << available[i] << " ";
        }
        cout << "\n";
        
        if (!resourcesAvailable) {
            cout << "✗ Resources not available. Process must wait.\n";
            return false;
        }
        cout << "✓ Resources are available\n";
        
        // Step 3: Temporarily allocate resources
        cout << "\nStep 3: Temporary allocation and safety check\n";
        
        // Save current state
        vector<int> oldAvailable = available;
        vector<vector<int>> oldAllocation = allocation;
        vector<vector<int>> oldNeed = need;
        
        // Make temporary allocation
        for (int i = 0; i < M; i++) {
            available[i] -= request[i];
            allocation[processId][i] += request[i];
            need[processId][i] -= request[i];
        }
        
        cout << "After temporary allocation:\n";
        displayVector(available, "New Available");
        cout << "New Allocation P" << processId << ": ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << allocation[processId][i] << " ";
        }
        cout << "\nNew Need P" << processId << ": ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << need[processId][i] << " ";
        }
        cout << "\n";
        
        // Check if new state is safe
        if (isSafeState(available)) {
            cout << "\n✓ REQUEST GRANTED - System remains in safe state\n";
            return true;
        } else {
            // Restore old state
            available = oldAvailable;
            allocation = oldAllocation;
            need = oldNeed;
            cout << "\n✗ REQUEST DENIED - Would lead to unsafe state\n";
            cout << "State restored to previous safe state\n";
            return false;
        }
    }
    
    void releaseResources(int processId) {
        cout << "\n" << string(60, '=') << "\n";
        cout << "RESOURCE RELEASE by Process P" << processId << "\n";
        cout << string(60, '=') << "\n";
        
        cout << "Resources being released: ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << allocation[processId][i] << " ";
        }
        cout << "\n";
        
        // Release all resources held by the process
        for (int i = 0; i < M; i++) {
            available[i] += allocation[processId][i];
            allocation[processId][i] = 0;
            need[processId][i] = max[processId][i]; // Reset need to max
        }
        
        cout << "After release:\n";
        displayVector(available, "New Available");
        cout << "P" << processId << " allocation reset to: ";
        for (int i = 0; i < M; i++) {
            cout << "R" << i << "=" << allocation[processId][i] << " ";
        }
        cout << "\n";
    }
    
    void setInitialState() {
        // Total resources in the system
        totalResources = {10, 5, 7};
        
        // Maximum demand matrix
        max = {{7, 5, 3},
               {3, 2, 2},
               {9, 0, 2},
               {2, 2, 2},
               {4, 3, 3}};
        
        // Current allocation matrix
        allocation = {{0, 1, 0},
                     {2, 0, 0},
                     {3, 0, 2},
                     {2, 1, 1},
                     {0, 0, 2}};
        
        // Calculate available resources
        available = totalResources;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                available[j] -= allocation[i][j];
            }
        }
        
        calculateNeed();
    }
    
    void displayCurrentState() {
        cout << "\n" << string(60, '=') << "\n";
        cout << "CURRENT SYSTEM STATE\n";
        cout << string(60, '=') << "\n";
        
        displayVector(totalResources, "Total Resources");
        displayVector(available, "Available Resources");
        displayMatrix(max, "Maximum Demand");
        displayMatrix(allocation, "Current Allocation");
        displayMatrix(need, "Need");
    }
    
    void runSimulation() {
        cout << "RESOURCE ALLOCATION ALGORITHM SIMULATION\n";
        cout << "=========================================\n";
        cout << "Number of Processes: " << N << "\n";
        cout << "Number of Resource Types: " << M << "\n";
        
        setInitialState();
        displayCurrentState();
        
        // Check initial safety
        cout << "\n" << string(60, '=') << "\n";
        cout << "INITIAL SAFETY CHECK\n";
        cout << string(60, '=') << "\n";
        isSafeState(available);
        
        // Test various resource requests
        vector<int> request1 = {1, 0, 2}; // P1 requests
        requestResources(1, request1);
        
        vector<int> request4 = {3, 3, 0}; // P4 requests (should be denied)
        requestResources(4, request4);
        
        vector<int> request0 = {0, 2, 0}; // P0 requests
        requestResources(0, request0);
        
        // Release resources from P1
        releaseResources(1);
        
        // Try P4 request again
        requestResources(4, request4);
        
        displayCurrentState();
    }
};

int main() {
    ResourceAllocation allocator;
    allocator.runSimulation();
    
    cout << "\n" << string(60, '=') << "\n";
    cout << "ALGORITHM SUMMARY\n";
    cout << string(60, '=') << "\n";
    cout << "Resource Allocation Algorithm Steps:\n";
    cout << "1. Check if Request[i] <= Need[i]\n";
    cout << "2. Check if Request[i] <= Available\n";
    cout << "3. Temporarily allocate resources:\n";
    cout << "   - Available = Available - Request[i]\n";
    cout << "   - Allocation[i] = Allocation[i] + Request[i]\n";
    cout << "   - Need[i] = Need[i] - Request[i]\n";
    cout << "4. Run safety algorithm to check if state is safe\n";
    cout << "5. If safe, grant request; otherwise, restore state and deny\n";
    
    return 0;
}