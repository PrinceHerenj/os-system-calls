#include <vector>
using namespace std;

void merge(vector<int> & a, int low, int mid, int high) {
    int i = low;
    int j = mid + 1;
    vector<int> temp;

    while (i <= mid && j <= high) {
        if (a[i] <= a[j]) temp.push_back(a[i++]);
        else temp.push_back(a[j++]);
    }

    while (i <= mid) temp.push_back(a[i++]);
    while (j <= high) temp.push_back(a[j++]);

    for (int i = 0; i < a.size(); i++) {
        a[i] = temp[i - low];
    }
}

void mergesort(vector<int> & a, int low, int high) {
    if (low < high) {
        int mid = (low + high ) / 2;
        mergesort(a, low, mid);
        mergesort(a, mid+1, high);
        merge(a, low, mid, high);
    }
}
