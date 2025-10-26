#include <iostream>
#include <vector>
#include <queue>
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

void roundRobinScheduling(vector<Process>& processes, int timeQuantum) {
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    
    // Initialize remaining time and started flag
    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
        p.started = false;
        p.responseTime = -1;
    }
    
    queue<int> readyQueue;
    vector<bool> inQueue(n, false);
    
    cout << "Round Robin CPU Scheduling (Time Quantum = " << timeQuantum << ")\n";
    cout << "============================================================\n\n";
    
    cout << "Execution Timeline:\n";
    cout << "Time\tProcess\tRemaining\tAction\t\tReady Queue\n";
    cout << "----\t-------\t---------\t------\t\t-----------\n";
    
    vector<pair<int, pair<int, int>>> ganttChart; // {pid, {start_time, duration}}
    
    // Check for processes arriving at time 0
    for (int i = 0; i < n; i++) {
        if (processes[i].arrivalTime == 0) {
            readyQueue.push(i);
            inQueue[i] = true;
        }
    }
    
    while (completed < n) {
        // Check for new arrivals
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime == currentTime && !inQueue[i] && processes[i].remainingTime > 0) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }
        
        if (readyQueue.empty()) {
            // No process ready, CPU idle
            cout << currentTime << "\t--\t--\t\tCPU Idle\t";
            ganttChart.push_back({0, {currentTime, 1}}); // 0 for IDLE
            currentTime++;
            
            // Print ready queue status
            cout << "[ ";
            queue<int> temp = readyQueue;
            while (!temp.empty()) {
                cout << "P" << processes[temp.front()].pid << " ";
                temp.pop();
            }
            cout << "]\n";
            continue;
        }
        
        int currentProcess = readyQueue.front();
        readyQueue.pop();
        inQueue[currentProcess] = false;
        
        // Set response time when process runs for the first time
        if (!processes[currentProcess].started) {
            processes[currentProcess].responseTime = currentTime - processes[currentProcess].arrivalTime;
            processes[currentProcess].started = true;
        }
        
        // Calculate execution time for this burst
        int executionTime = min(timeQuantum, processes[currentProcess].remainingTime);
        
        cout << currentTime << "\tP" << processes[currentProcess].pid 
             << "\t" << processes[currentProcess].remainingTime 
             << "\t\tStart (TQ=" << executionTime << ")\t";
        
        ganttChart.push_back({processes[currentProcess].pid, {currentTime, executionTime}});
        
        // Execute the process
        processes[currentProcess].remainingTime -= executionTime;
        currentTime += executionTime;
        
        // Check for new arrivals during execution
        for (int i = 0; i < n; i++) {
            if (processes[i].arrivalTime > (currentTime - executionTime) && 
                processes[i].arrivalTime <= currentTime && 
                !inQueue[i] && processes[i].remainingTime > 0) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }
        
        // Check if process completed
        if (processes[currentProcess].remainingTime == 0) {
            processes[currentProcess].completionTime = currentTime;
            processes[currentProcess].turnaroundTime = processes[currentProcess].completionTime - processes[currentProcess].arrivalTime;
            processes[currentProcess].waitingTime = processes[currentProcess].turnaroundTime - processes[currentProcess].burstTime;
            completed++;
            
            cout << "[ ";
            queue<int> temp = readyQueue;
            while (!temp.empty()) {
                cout << "P" << processes[temp.front()].pid << " ";
                temp.pop();
            }
            cout << "] - Completed\n";
        } else {
            // Process not completed, add back to ready queue
            readyQueue.push(currentProcess);
            inQueue[currentProcess] = true;
            
            cout << "[ ";
            queue<int> temp = readyQueue;
            while (!temp.empty()) {
                cout << "P" << processes[temp.front()].pid << " ";
                temp.pop();
            }
            cout << "] - Preempted\n";
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

void displayGanttChart(const vector<pair<int, pair<int, int>>>& ganttChart) {
    cout << "\nGantt Chart:\n";
    cout << "|";
    
    // Display process names
    for (const auto& g : ganttChart) {
        if (g.first == 0) {
            cout << " IDLE |";
        } else {
            cout << " P" << g.first << " |";
        }
    }
    cout << "\n";
    
    // Display time markers
    int currentTime = 0;
    cout << currentTime;
    for (const auto& g : ganttChart) {
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
    
    int timeQuantum = 2;
    
    cout << "Input Processes:\n";
    cout << "PID\tArrival Time\tBurst Time\n";
    cout << "---\t------------\t----------\n";
    for (const auto& p : processes) {
        cout << "P" << p.pid << "\t" << p.arrivalTime << "\t\t" << p.burstTime << "\n";
    }
    cout << "\nTime Quantum: " << timeQuantum << "\n\n";
    
    // Store gantt chart for later display
    vector<pair<int, pair<int, int>>> gantt;
    
    // Need to recreate gantt chart
    vector<Process> tempProcesses = processes;
    roundRobinScheduling(tempProcesses, timeQuantum);
    
    // Copy results back
    for (int i = 0; i < processes.size(); i++) {
        processes[i] = tempProcesses[i];
    }
    
    displayResults(processes);
    
    // Recreate gantt chart for display
    vector<Process> ganttProcesses = processes;
    for (auto& p : ganttProcesses) {
        p.remainingTime = p.burstTime;
        p.started = false;
    }
    
    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    queue<int> readyQueue;
    vector<bool> inQueue(n, false);
    
    // Check for processes arriving at time 0
    for (int i = 0; i < n; i++) {
        if (ganttProcesses[i].arrivalTime == 0) {
            readyQueue.push(i);
            inQueue[i] = true;
        }
    }
    
    vector<pair<int, pair<int, int>>> ganttChart;
    
    while (completed < n) {
        // Check for new arrivals
        for (int i = 0; i < n; i++) {
            if (ganttProcesses[i].arrivalTime == currentTime && !inQueue[i] && ganttProcesses[i].remainingTime > 0) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }
        
        if (readyQueue.empty()) {
            ganttChart.push_back({0, {currentTime, 1}});
            currentTime++;
            continue;
        }
        
        int currentProcess = readyQueue.front();
        readyQueue.pop();
        inQueue[currentProcess] = false;
        
        int executionTime = min(timeQuantum, ganttProcesses[currentProcess].remainingTime);
        ganttChart.push_back({ganttProcesses[currentProcess].pid, {currentTime, executionTime}});
        
        ganttProcesses[currentProcess].remainingTime -= executionTime;
        currentTime += executionTime;
        
        // Check for new arrivals during execution
        for (int i = 0; i < n; i++) {
            if (ganttProcesses[i].arrivalTime > (currentTime - executionTime) && 
                ganttProcesses[i].arrivalTime <= currentTime && 
                !inQueue[i] && ganttProcesses[i].remainingTime > 0) {
                readyQueue.push(i);
                inQueue[i] = true;
            }
        }
        
        if (ganttProcesses[currentProcess].remainingTime == 0) {
            completed++;
        } else {
            readyQueue.push(currentProcess);
            inQueue[currentProcess] = true;
        }
    }
    
    displayGanttChart(ganttChart);
    
    cout << "\nRound Robin Characteristics:\n";
    cout << "- Preemptive algorithm with fixed time quantum\n";
    cout << "- Fair allocation of CPU time to all processes\n";
    cout << "- Good response time for interactive systems\n";
    cout << "- Performance depends on time quantum size\n";
    cout << "- Higher context switching overhead\n";
    
    return 0;
}