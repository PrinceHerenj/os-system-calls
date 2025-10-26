#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int responseTime;
    bool started;
};

void srtfScheduling(vector<Process>& processes) {
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    int lastExecuted = -1;
    
    // Initialize remaining time and started flag
    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
        p.started = false;
        p.responseTime = -1;
    }
    
    cout << "SRTF (Shortest Remaining Time First) CPU Scheduling\n";
    cout << "===================================================\n\n";
    
    cout << "Execution Timeline:\n";
    cout << "Time\tProcess\tRemaining\tAction\n";
    cout << "----\t-------\t---------\t------\n";
    
    vector<pair<int, int>> ganttChart; // {pid, start_time}
    
    while (completed < n) {
        int shortestJob = -1;
        int minRemainingTime = INT_MAX;
        
        // Find process with shortest remaining time among arrived processes
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= currentTime && 
                processes[i].remainingTime > 0 && 
                processes[i].remainingTime < minRemainingTime) {
                minRemainingTime = processes[i].remainingTime;
                shortestJob = i;
            }
        }
        
        if (shortestJob == -1) {
            // No process has arrived yet
            cout << currentTime << "\t--\t--\t\tCPU Idle\n";
            ganttChart.push_back({0, currentTime}); // 0 for IDLE
            currentTime++;
        } else {
            // Check if we're switching to a different process
            if (lastExecuted != shortestJob) {
                if (lastExecuted != -1) {
                    cout << currentTime << "\tP" << processes[lastExecuted].pid 
                         << "\t" << processes[lastExecuted].remainingTime 
                         << "\t\tPreempted\n";
                }
                cout << currentTime << "\tP" << processes[shortestJob].pid 
                     << "\t" << processes[shortestJob].remainingTime 
                     << "\t\tStart/Resume\n";
                
                ganttChart.push_back({processes[shortestJob].pid, currentTime});
                
                // Set response time when process starts for the first time
                if (!processes[shortestJob].started) {
                    processes[shortestJob].responseTime = currentTime - processes[shortestJob].arrivalTime;
                    processes[shortestJob].started = true;
                }
            }
            
            // Execute the process for 1 time unit
            processes[shortestJob].remainingTime--;
            currentTime++;
            lastExecuted = shortestJob;
            
            // Check if process is completed
            if (processes[shortestJob].remainingTime == 0) {
                processes[shortestJob].completionTime = currentTime;
                processes[shortestJob].turnaroundTime = processes[shortestJob].completionTime - processes[shortestJob].arrivalTime;
                processes[shortestJob].waitingTime = processes[shortestJob].turnaroundTime - processes[shortestJob].burstTime;
                completed++;
                
                cout << currentTime << "\tP" << processes[shortestJob].pid 
                     << "\t0\t\tCompleted\n";
            }
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
    // Recreate the execution for Gantt chart
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    int lastExecuted = -1;
    
    // Reset for simulation
    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
    }
    
    vector<pair<int, pair<int, int>>> gantt; // {pid, {start_time, duration}}
    
    while (completed < n) {
        int shortestJob = -1;
        int minRemainingTime = INT_MAX;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime <= currentTime && 
                processes[i].remainingTime > 0 && 
                processes[i].remainingTime < minRemainingTime) {
                minRemainingTime = processes[i].remainingTime;
                shortestJob = i;
            }
        }
        
        if (shortestJob == -1) {
            if (gantt.empty() || gantt.back().first != 0) {
                gantt.push_back({0, {currentTime, 1}}); // IDLE
            } else {
                gantt.back().second.second++;
            }
            currentTime++;
        } else {
            if (lastExecuted != shortestJob) {
                gantt.push_back({processes[shortestJob].pid, {currentTime, 1}});
            } else {
                gantt.back().second.second++;
            }
            
            processes[shortestJob].remainingTime--;
            currentTime++;
            lastExecuted = shortestJob;
            
            if (processes[shortestJob].remainingTime == 0) {
                completed++;
            }
        }
    }
    
    cout << "\nGantt Chart:\n";
    cout << "|";
    
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
        currentTime += g.second.second;
        cout << "   " << currentTime;
    }
    cout << "\n";
}

int main() {
    vector<Process> processes = {
        {1, 0, 10, 0, 0, 0, 0, 0, false},  // P1: AT=0, BT=10
        {2, 1, 4, 0, 0, 0, 0, 0, false},   // P2: AT=1, BT=4
        {3, 2, 6, 0, 0, 0, 0, 0, false},   // P3: AT=2, BT=6
        {4, 3, 8, 0, 0, 0, 0, 0, false},   // P4: AT=3, BT=8
        {5, 4, 3, 0, 0, 0, 0, 0, false}    // P5: AT=4, BT=3
    };
    
    cout << "Input Processes:\n";
    cout << "PID\tArrival Time\tBurst Time\n";
    cout << "---\t------------\t----------\n";
    for (const auto& p : processes) {
        cout << "P" << p.pid << "\t" << p.arrivalTime << "\t\t" << p.burstTime << "\n";
    }
    cout << "\n";
    
    srtfScheduling(processes);
    displayResults(processes);
    displayGanttChart(processes);
    
    cout << "\nSRTF Characteristics:\n";
    cout << "- Preemptive version of SJF\n";
    cout << "- Process with shortest remaining time gets CPU\n";
    cout << "- Optimal for minimizing average waiting time\n";
    cout << "- Higher context switching overhead\n";
    cout << "- May cause starvation for longer processes\n";
    
    return 0;
}