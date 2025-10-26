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
};

void fcfsScheduling(vector<Process>& processes) {
    int n = processes.size();
    
    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });
    
    int currentTime = 0;
    
    cout << "FCFS (First Come First Serve) CPU Scheduling\n";
    cout << "============================================\n\n";
    
    cout << "Execution Order:\n";
    cout << "Time\tProcess\tAction\n";
    cout << "----\t-------\t------\n";
    
    for (int i = 0; i < n; i++) {
        // If current time is less than arrival time, CPU is idle
        if (currentTime < processes[i].arrivalTime) {
            if (currentTime != processes[i].arrivalTime) {
                cout << currentTime << "\t--\tCPU Idle\n";
            }
            currentTime = processes[i].arrivalTime;
        }
        
        // Process starts execution
        cout << currentTime << "\tP" << processes[i].pid << "\tStart\n";
        processes[i].responseTime = currentTime - processes[i].arrivalTime;
        
        // Process completes
        currentTime += processes[i].burstTime;
        processes[i].completionTime = currentTime;
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
        
        cout << currentTime << "\tP" << processes[i].pid << "\tComplete\n";
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

void displayGanttChart(const vector<Process>& processes) {
    cout << "\nGantt Chart:\n";
    cout << "|";
    
    // Display process names
    int currentTime = 0;
    for (const auto& p : processes) {
        if (currentTime < p.arrivalTime) {
            cout << " IDLE |";
            currentTime = p.arrivalTime;
        }
        cout << " P" << p.pid << " |";
        currentTime += p.burstTime;
    }
    cout << "\n";
    
    // Display time markers
    currentTime = 0;
    cout << currentTime;
    for (const auto& p : processes) {
        if (currentTime < p.arrivalTime) {
            cout << "    " << p.arrivalTime;
            currentTime = p.arrivalTime;
        }
        currentTime += p.burstTime;
        cout << "   " << currentTime;
    }
    cout << "\n";
}

int main() {
    vector<Process> processes = {
        {1, 0, 10, 0, 0, 0, 0},  // P1: AT=0, BT=10
        {2, 1, 4, 0, 0, 0, 0},   // P2: AT=1, BT=4
        {3, 2, 6, 0, 0, 0, 0},   // P3: AT=2, BT=6
        {4, 3, 8, 0, 0, 0, 0},   // P4: AT=3, BT=8
        {5, 4, 3, 0, 0, 0, 0}    // P5: AT=4, BT=3
    };
    
    cout << "Input Processes:\n";
    cout << "PID\tArrival Time\tBurst Time\n";
    cout << "---\t------------\t----------\n";
    for (const auto& p : processes) {
        cout << "P" << p.pid << "\t" << p.arrivalTime << "\t\t" << p.burstTime << "\n";
    }
    cout << "\n";
    
    fcfsScheduling(processes);
    displayResults(processes);
    displayGanttChart(processes);
    
    cout << "\nFCFS Characteristics:\n";
    cout << "- Non-preemptive algorithm\n";
    cout << "- Processes are executed in order of arrival\n";
    cout << "- Simple to implement but may cause convoy effect\n";
    cout << "- Average waiting time depends on arrival order\n";
    
    return 0;
}