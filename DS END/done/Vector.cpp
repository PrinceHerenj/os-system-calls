#include <iostream>  // Not directly used in this file
using namespace std;

template <class T>
class Vector {
    private:
        int theSize;
        int theCapacity;
        T * objects;
        static const int SPARE_SPACE = 16;
    public:
        // Explicit Constructor
        explicit Vector(int initSize = 0): theSize(initSize), theCapacity(initSize + SPARE_SPACE), objects(new T[theCapacity]) {}
        // Copy
        Vector (const Vector & rhs): theSize(rhs.theSize), theCapacity(rhs.theCapacity), objects(new T[theCapacity]) {
            for (int i = 0; i< theSize; i++)
                objects[i] = rhs.objects[i];
        }
        // Destructor
        ~Vector() {
            delete []objects;
        }
        // Assignment
        const Vector & operator=(const Vector & rhs) {
            if (this != &rhs) {
                delete []objects;
                theSize = rhs.theSize;
                theCapacity = rhs.theCapacity;
                objects = new T[theCapacity];

                for (int i = 0; i < theSize; i++)
                    objects[i] = rhs.objects[i];
            }
            return *this;
        }

        // size
        int size() {
            return theSize;
        }
        // capacity
        int capacity() {
            return theCapacity;
        }
        // empty
        int empty() {
            return theSize == 0;
        }

        // reserve
        void reserve(int newCapacity) {
            if (newCapacity < theSize) return;
            T * oldArray = objects;
            objects = new T[newCapacity];
            for (int i = 0; i < theSize; i++) {
                objects[i] = oldArray[i];
            }

            theCapacity = newCapacity;

            delete[] oldArray;
        }
        // resize
        void resize(int newSize) {
            if (newSize > theCapacity) reserve(newSize * 2);
            theSize = newSize;
        }

        // operator[]
        T & operator[](int index) {
            return objects[index];
        }

        // push_back
        void push_back(const T & x) {
            if (theSize == theCapacity) resize(theCapacity);
            objects[theSize++] = x;
        }

        // pop_back
        void pop_back() {
            theSize--;
        }

        const T & front() const {
            return objects[0];
        }

        const T & back() const {
            return objects[theSize - 1];
        }

        typedef T * iterator;

        iterator begin() {
            return &objects[0];
        }

        iterator end() {
            return &objects[size()];
        }
};
