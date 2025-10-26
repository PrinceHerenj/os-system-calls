#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
    bool completed;
};

void sjfScheduling(vector<Process>& processes) {
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    
    // Initialize completed flag
    for (auto& p : processes) {
        p.completed = false;
    }
    
    cout << "SJF (Shortest Job First) CPU Scheduling\n";
    cout << "=======================================\n\n";
    
    cout << "Execution Order:\n";
    cout << "Time\tProcess\tAction\n";
    cout << "----\t-------\t------\n";
    
    vector<Process> executionOrder;
    
    while (completed < n) {
        int shortestJob = -1;
        int minBurstTime = INT_MAX;
        
        // Find the shortest job among arrived processes
        for (int i = 0; i < n; i++) {
            if (!processes[i].completed && 
                processes[i].arrivalTime <= currentTime && 
                processes[i].burstTime < minBurstTime) {
                minBurstTime = processes[i].burstTime;
                shortestJob = i;
            }
        }
        
        if (shortestJob == -1) {
            // No process has arrived yet, advance time
            currentTime++;
            cout << currentTime-1 << "\t--\tCPU Idle\n";
        } else {
            // Execute the shortest job
            cout << currentTime << "\tP" << processes[shortestJob].pid << "\tStart\n";
            
            processes[shortestJob].responseTime = currentTime - processes[shortestJob].arrivalTime;
            currentTime += processes[shortestJob].burstTime;
            processes[shortestJob].completionTime = currentTime;
            processes[shortestJob].turnaroundTime = processes[shortestJob].completionTime - processes[shortestJob].arrivalTime;
            processes[shortestJob].waitingTime = processes[shortestJob].turnaroundTime - processes[shortestJob].burstTime;
            processes[shortestJob].completed = true;
            
            executionOrder.push_back(processes[shortestJob]);
            completed++;
            
            cout << currentTime << "\tP" << processes[shortestJob].pid << "\tComplete\n";
        }
    }
}

void displayResults(const vector<Process>& processes) {
    cout << "\nProcess Details:\n";
    cout << "PID\tAT\tBT\tCT\tTAT\tWT\tRT\n";
    cout << "---\t--\t--\t--\t---\t--\t--\n";
    
    double totalTAT = 0, totalWT = 0, totalRT = 0;
    
    for (const auto& p : processes) {
        cout << "P" << p.pid << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t" 
             << p.completionTime << "\t" << p.turnaroundTime << "\t" 
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
        int shortestJob = -1;
        int minBurstTime = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (!temp[i].completed && 
                temp[i].arrivalTime <= currentTime && 
                temp[i].burstTime < minBurstTime) {
                minBurstTime = temp[i].burstTime;
                shortestJob = i;
            }
        }
        
        if (shortestJob == -1) {
            currentTime++;
            if (gantt.empty() || gantt.back().first != 0) {
                gantt.push_back({0, 1}); // IDLE
            } else {
                gantt.back().second++;
            }
        } else {
            gantt.push_back({temp[shortestJob].pid, temp[shortestJob].burstTime});
            currentTime += temp[shortestJob].burstTime;
            temp[shortestJob].completed = true;
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

int main() {
    vector<Process> processes = {
        {1, 0, 10, 0, 0, 0, 0, false},  // P1: AT=0, BT=10
        {2, 1, 4, 0, 0, 0, 0, false},   // P2: AT=1, BT=4
        {3, 2, 6, 0, 0, 0, 0, false},   // P3: AT=2, BT=6
        {4, 3, 8, 0, 0, 0, 0, false},   // P4: AT=3, BT=8
        {5, 4, 3, 0, 0, 0, 0, false}    // P5: AT=4, BT=3
    };
    
    cout << "Input Processes:\n";
    cout << "PID\tArrival Time\tBurst Time\n";
    cout << "---\t------------\t----------\n";
    for (const auto& p : processes) {
        cout << "P" << p.pid << "\t" << p.arrivalTime << "\t\t" << p.burstTime << "\n";
    }
    cout << "\n";
    
    sjfScheduling(processes);
    displayResults(processes);
    displayGanttChart(processes);
    
    cout << "\nSJF Characteristics:\n";
    cout << "- Non-preemptive algorithm\n";
    cout << "- Selects process with shortest burst time among arrived processes\n";
    cout << "- Optimal for minimizing average waiting time\n";
    cout << "- May cause starvation for longer processes\n";
    cout << "- Requires knowledge of burst times in advance\n";
    
    return 0;
}