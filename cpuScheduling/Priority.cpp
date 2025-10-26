#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
    bool completed;
};

void priorityScheduling(vector<Process>& processes) {
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    
    // Initialize completed flag
    for (auto& p : processes) {
        p.completed = false;
    }
    
    cout << "Priority Scheduling CPU Scheduling (Lower number = Higher priority)\n";
    cout << "==================================================================\n\n";
    
    cout << "Execution Order:\n";
    cout << "Time\tProcess\tPriority\tAction\n";
    cout << "----\t-------\t--------\t------\n";
    
    vector<Process> executionOrder;
    
    while (completed < n) {
        int highestPriorityJob = -1;
        int highestPriority = INT_MAX;
        
        // Find the highest priority job among arrived processes
        for (int i = 0; i < n; i++) {
            if (!processes[i].completed && 
                processes[i].arrivalTime <= currentTime && 
                processes[i].priority < highestPriority) {
                highestPriority = processes[i].priority;
                highestPriorityJob = i;
            }
        }
        
        if (highestPriorityJob == -1) {
            // No process has arrived yet, advance time
            currentTime++;
            cout << currentTime-1 << "\t--\t--\t\tCPU Idle\n";
        } else {
            // Execute the highest priority job
            cout << currentTime << "\tP" << processes[highestPriorityJob].pid 
                 << "\t" << processes[highestPriorityJob].priority << "\t\tStart\n";
            
            processes[highestPriorityJob].responseTime = currentTime - processes[highestPriorityJob].arrivalTime;
            currentTime += processes[highestPriorityJob].burstTime;
            processes[highestPriorityJob].completionTime = currentTime;
            processes[highestPriorityJob].turnaroundTime = processes[highestPriorityJob].completionTime - processes[highestPriorityJob].arrivalTime;
            processes[highestPriorityJob].waitingTime = processes[highestPriorityJob].turnaroundTime - processes[highestPriorityJob].burstTime;
            processes[highestPriorityJob].completed = true;
            
            executionOrder.push_back(processes[highestPriorityJob]);
            completed++;
            
            cout << currentTime << "\tP" << processes[highestPriorityJob].pid 
                 << "\t" << processes[highestPriorityJob].priority << "\t\tComplete\n";
        }
    }
}

void displayResults(const vector<Process>& processes) {
    cout << "\nProcess Details:\n";
    cout << "PID\tAT\tBT\tPriority\tCT\tTAT\tWT\tRT\n";
    cout << "---\t--\t--\t--------\t--\t---\t--\t--\n";
    
    double totalTAT = 0, totalWT = 0, totalRT = 0;
    
    for (const auto& p : processes) {
        cout << "P" << p.pid << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t" 
             << p.priority << "\t\t" << p.completionTime << "\t" << p.turnaroundTime << "\t" 
             << p.waitingTime << "\t" << p.responseTime << "\n";
        
        totalTAT += p.turnaroundTime;
        totalWT += p.waitingTime;
        totalRT += p.responseTime;
    }
    
    int n = processes.size();
    cout << "\nAverage Turnaround Time: " << fixed << setprecision(2) << totalTAT / n << endl;
    cout << "Average Waiting Time: " << fixed << setprecision(2) << totalWT / n << endl;
    cout << "Average Response Time: " << fixed << setprecision(2) << totalRT / n << endl;
    
    cout << "\nLegend:\n";
    cout << "PID = Process ID, AT = Arrival Time, BT = Burst Time\n";
    cout << "CT = Completion Time, TAT = Turnaround Time\n";
    cout << "WT = Waiting Time, RT = Response Time\n";
}

void displayGanttChart(vector<Process> processes) {
    // Sort by completion order for Gantt chart
    vector<Process> temp = processes;
    for (auto& p : temp) p.completed = false;
    
    cout << "\nGantt Chart:\n";
    cout << "|";
    
    int currentTime = 0;
    int completed = 0;
    int n = processes.size();
    
    // Recreate execution order for Gantt chart
    vector<pair<int, int>> gantt; // {pid, duration}
    
    while (completed < n) {
        int highestPriorityJob = -1;
        int highestPriority = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (!temp[i].completed && 
                temp[i].arrivalTime <= currentTime && 
                temp[i].priority < highestPriority) {
                highestPriority = temp[i].priority;
                highestPriorityJob = i;
            }
        }
        
        if (highestPriorityJob == -1) {
            currentTime++;
            if (gantt.empty() || gantt.back().first != 0) {
                gantt.push_back({0, 1}); // IDLE
            } else {
                gantt.back().second++;
            }
        } else {
            gantt.push_back({temp[highestPriorityJob].pid, temp[highestPriorityJob].burstTime});
            currentTime += temp[highestPriorityJob].burstTime;
            temp[highestPriorityJob].completed = true;
            completed++;
        }
    }
    
    // Display process names
    for (const auto& g : gantt) {
        if (g.first == 0) {
            cout << " IDLE |";
        } else {
            cout << " P" << g.first << " |";
        }
    }
    cout << "\n";
    
    // Display time markers
    currentTime = 0;
    cout << currentTime;
    for (const auto& g : gantt) {
        currentTime += g.second;
        cout << "   " << currentTime;
    }
    cout << "\n";
}

void displayPriorityOrder(const vector<Process>& processes) {
    vector<Process> sortedByPriority = processes;
    sort(sortedByPriority.begin(), sortedByPriority.end(), 
         [](const Process& a, const Process& b) {
             return a.priority < b.priority;
         });
    
    cout << "\nProcess Priority Order (Higher to Lower Priority):\n";
    cout << "Process\tPriority\tBurst Time\tArrival Time\n";
    cout << "-------\t--------\t----------\t------------\n";
    for (const auto& p : sortedByPriority) {
        cout << "P" << p.pid << "\t" << p.priority << "\t\t" 
             << p.burstTime << "\t\t" << p.arrivalTime << "\n";
    }
}

int main() {
    vector<Process> processes = {
        {1, 0, 10, 3, 0, 0, 0, 0, false},  // P1: AT=0, BT=10, Priority=3
        {2, 1, 4, 1, 0, 0, 0, 0, false},   // P2: AT=1, BT=4,  Priority=1 (highest)
        {3, 2, 6, 4, 0, 0, 0, 0, false},   // P3: AT=2, BT=6,  Priority=4
        {4, 3, 8, 2, 0, 0, 0, 0, false},   // P4: AT=3, BT=8,  Priority=2
        {5, 4, 3, 5, 0, 0, 0, 0, false}    // P5: AT=4, BT=3,  Priority=5 (lowest)
    };
    
    cout << "Input Processes:\n";
    cout << "PID\tArrival Time\tBurst Time\tPriority\n";
    cout << "---\t------------\t----------\t--------\n";
    for (const auto& p : processes) {
        cout << "P" << p.pid << "\t" << p.arrivalTime << "\t\t" << p.burstTime 
             << "\t\t" << p.priority << "\n";
    }
    cout << "\n";
    
    displayPriorityOrder(processes);
    cout << "\n";
    
    priorityScheduling(processes);
    displayResults(processes);
    displayGanttChart(processes);
    
    cout << "\nPriority Scheduling Characteristics:\n";
    cout << "- Non-preemptive algorithm (in this implementation)\n";
    cout << "- Processes are executed based on priority levels\n";
    cout << "- Lower priority number = Higher priority\n";
    cout << "- May cause starvation for low priority processes\n";
    cout << "- Can be combined with aging to prevent starvation\n";
    
    return 0;
}