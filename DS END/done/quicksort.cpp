#include <iostream>
#include <vector>
using namespace std;

int partition(vector<int> & a, int low, int high) {
    int pivot = a[low];
    int i = low;
    int j = high;
    while (i < j) {
        while (a[i] <= pivot && i <= high - 1) i++;
        while(a[j] >= pivot && j >= low + 1) j--;
        if (i < j) swap(a[i], a[j]);
        else break;
    }
    swap(a[low], a[j]);
    return j;
}

int partitionWithMedian(vector<int> & a, int low, int high) {
    int mid = low + (high - low) / 2;
    // Sort low, mid, high values
    if (a[mid] < a[low]) swap(a[mid], a[low]);
    if (a[high] < a[low]) swap(a[high], a[low]);
    if (a[high] < a[mid]) swap(a[high], a[mid]);

    // Place pivot (median) at low
    swap(a[low], a[mid]);
    return partition(a, low, high);
}

int partitionWithLast(vector<int> & a, int low, int high) {
    // Use last element as pivot
    swap(a[low], a[high]);
    return partition(a, low, high);
}

int partitionWithRandom(vector<int> & a, int low, int high) {
    // Use random element as pivot
    int random = low + rand() % (high - low + 1);
    swap(a[low], a[random]);
    return partition(a, low, high);
}

void quicksort(vector<int> & a, int low, int high) {
    if (low < high) {
        int pi = partitionWithMedian(a, low, high);
        quicksort(a, low, pi - 1);
        quicksort(a, pi + 1, high);
    }
}
int main() {
    // Test case 1: Random array
    vector<int> arr1 = {10, 7, 8, 9, 1, 5};
    cout << "Original array 1: ";
    for (int num : arr1) cout << num << " ";
    cout << endl;

    quicksort(arr1, 0, arr1.size() - 1);

    cout << "Sorted array 1: ";
    for (int num : arr1) cout << num << " ";
    cout << endl;

    // Test case 2: Already sorted array
    vector<int> arr2 = {1, 2, 3, 4, 5};
    cout << "Original array 2: ";
    for (int num : arr2) cout << num << " ";
    cout << endl;

    quicksort(arr2, 0, arr2.size() - 1);

    cout << "Sorted array 2: ";
    for (int num : arr2) cout << num << " ";
    cout << endl;

    // Test case 3: Reverse sorted array
    vector<int> arr3 = {5, 4, 3, 2, 1};
    cout << "Original array 3: ";
    for (int num : arr3) cout << num << " ";
    cout << endl;

    quicksort(arr3, 0, arr3.size() - 1);

    cout << "Sorted array 3: ";
    for (int num : arr3) cout << num << " ";
    cout << endl;

    // Test case 4: Array with duplicates
    vector<int> arr4 = {4, 2, 2, 8, 3, 3, 1};
    cout << "Original array 4: ";
    for (int num : arr4) cout << num << " ";
    cout << endl;

    quicksort(arr4, 0, arr4.size() - 1);

    cout << "Sorted array 4: ";
    for (int num : arr4) cout << num << " ";
    cout << endl;

    return 0;
}
