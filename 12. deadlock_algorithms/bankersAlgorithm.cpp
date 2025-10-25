#include <vector>
#include <iostream>
using namespace std;

#define N 5 // number of processes
#define M 3 // types of resources

bool safetyAlgorithm(vector<int> & work, vector<vector<int>> & need, vector<vector<int>> & allocation) {
    vector<bool> finish(N, false);
    vector<int> safeSequence;

    int count = 0;
    while (count < N) {
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
                    for (int j = 0; j < M; j++) {
                        work[j] += allocation[i][j];
                    }

                    finish[i] = true;
                    safeSequence.push_back(i);
                    found = true;
                    count++;
                    break;
                }
            }
        }
        if (!found) {
            cout << "System is not in safe state!" << endl;
            return false;
        }
    }

    cout << "System is in safe state" << endl;
    cout << "Safe sequence: ";
    for (int i = 0; i < safeSequence.size(); i++) {
        cout << "P" << safeSequence[i];
        if (i < safeSequence.size() - 1) cout << " -> ";
    }
    cout << endl;

    return true;
}

int main() {
    vector<int> available(M);
    vector<vector<int>> max(N, vector<int>(M));
    vector<vector<int>> allocation(N, vector<int>(M));
    vector<vector<int>> need(N, vector<int>(M));

    available = {3, 3, 2};

    max = {{7, 5, 3},
            {3, 2, 2},
            {9, 0, 2},
            {2, 2, 2},
            {4, 3, 3}};

    allocation = {{0, 1, 0},
                    {2, 0, 0},
                    {3, 0, 2},
                    {2, 1, 1},
                    {0, 0, 2}};

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }

    safetyAlgorithm(available, need, allocation);

    return 0;
}
