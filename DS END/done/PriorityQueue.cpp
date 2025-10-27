#include <iostream>
#include <utility>
#include <vector>
using namespace std;

template<class T, class Comparator>
class PriorityQueue {
    public:
        explicit PriorityQueue(int cap = 150): array(cap), currentSize(0) {}
        explicit PriorityQueue(vector<T> & items): array(items.size() + 10), currentSize(items.size()) {
            // ERROR: items.size is a function, should be items.size()
            for (int i = 0; i < items.size(); i++)
                array[i+1] = items[i];
            buildHeap();
        }

        bool isEmpty() {
            return currentSize == 0;
        }

        void makeEmpty() {
            for (int i = 0; i < array.size(); i++)
                array[i] = 0;
            currentSize = 0;
        }

        const T & top() {
            return array[1];
        }

        void insert(const T & x) {
            if (currentSize == array.size() - 1) array.resize(array.size() * 2);
            int hole = ++currentSize;
            for(; hole > 1 && c(x, array[hole/2]); hole /= 2)
                array[hole] = std::move(array[hole/2]);
            array[hole] = std::move(x);
        }

        void pop() {
            if (currentSize == 0) {
                cout << "Underflow" << endl; return;
            }
            array[1] = std::move(array[currentSize--]);
            percolateDown(1);
        }

        void pop(T & item) {
            if (currentSize == 0) {
                cout << "Underflow" << endl; return;
            }
            item = array[1];
            array[1] = std::move(array[currentSize--]);
            percolateDown(1);
        }
    private:
        vector<T> array;
        int currentSize;
        Comparator c;

        void buildHeap() {
            for (int i = currentSize / 2; i > 0; --i)
                percolateDown(i);
        }

        void percolateDown(int hole) {
            int child;
            T temp = std::move(array[hole]);
            for (; hole * 2 <= currentSize; hole = child) {
                child = hole * 2;
                if (child != currentSize && c(array[child + 1], array[child])) child++;
                if (c(array[child], temp)) array[hole] = std::move(array[child]);
                else break;
            }
            array[hole] = std::move(temp);
        }
};
