#include <iostream>
#include <list>
using namespace std;

template<class T>
class Stack {
    public:
        Stack() {}
        Stack(const Stack & rhs): stack(rhs.stack) {}
        ~Stack() {
        }

        bool isEmpty() {
            return stack.size() == 0;
        }

        void makeEmpty() {
            stack.clear();
        }

        void push(const T & x) {
            stack.push_back(x);
        }

        void pop() {
            stack.pop_back();
        }

        void peek() {
            cout << stack.back() << endl;
        }

        void display() {
            for (auto i: stack) cout << i << " ";
            cout << endl;
        }
    private:
        list<T> stack;
};
