#include <iostream>
#include <list>
using namespace std;

template<class T>
class Queue {
    public:
        Queue() {}
        Queue(const Queue & rhs): queue(rhs.queue) {}
        ~Queue() {
            queue.clear();
        }

        const Queue & operator= (const Queue & rhs) {
            if (this != &rhs) {
                // Cannot use delete on std::list, it's not a pointer
                delete queue;
                // Should use deep copy with assignment operator
                queue = rhs.queue;
            }
            return *this;
        }

        void enqueue(const T & x) {
            queue.push_back(x);
        }

        T dequeue() {
            // No check if queue is empty before accessing front
            T temp = queue.front();
            queue.pop_front();
            return temp;
        }

        void top() {
            // No check if queue is empty before accessing front
            // This should return the value, not print it (based on function name)
            cout << queue.front() << endl;
        }

        void traverse() {
            for (auto i: queue) cout << i << " ";
            cout << endl;
        }
    private:
        list<T> queue;

};
