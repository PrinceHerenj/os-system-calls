#include <iostream> // This header is not directly used in the code
using namespace std;

void insertionsort(vector<int> & arr) { // Error: Missing #include <vector>
    for (int i = 1; i < arr.size(); i++) {
        int p = i;
        while (p > 0 && arr[p] < arr[p-1]){
            swap(arr[p], arr[p-1]);
            p--;
        }
    }
}

void shellsort(vector<int> & arr) {
    for (int gap = arr.size() / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < arr.size(); i++) {
            int p = i;
            while (p >= gap && arr[p] < arr[p - gap]) {
                swap(arr[p], arr[p - gap]);
                p -= gap;
            }
        }
    }
}
