#include <iostream>
#include <vector>
using namespace std;

template<class T>
class HashTable {
    public:
        explicit HashTable(int size = 101): array(nextPrime(size)), currentSize(0) {
            makeEmpty();
        }

        bool contains(const T & x) const {
            return isActive(findPos(x));
        }

        void makeEmpty() {
            currentSize = 0;
            for (auto & entry: array) entry.info = EMPTY;
        }

        bool insert(const T & x) {
            int currentPos = findPos(x);
            if (isActive(currentPos)) return false;
            array[currentPos].el = x;
            array[currentPos].info = ACTIVE;

            if (++currentSize > array.size() / 2) rehash();
            return true;
        }

        bool remove(const T & x) {
            int currentPos = findPos(x);
            if (!isActive(currentPos)) return false;
            array[currentPos].info = DELETED;
            return true;
        }

        void displayTable() {
            cout << "Key\tValue" << endl;
            for (int i = 0; i < array.size(); i++) {
                cout << i << "\t";
                if (isActive(i)) cout << array[i].el;
                cout << endl;
            }
            cout << endl;
        }

        enum EntryType {
            ACTIVE,
            EMPTY,
            DELETED
        };
    private:
        struct HashEntry {
            T el;
            EntryType info;

            HashEntry(const T & x = T(), EntryType info = EMPTY): el(x), info(info) {}
        };
        vector<HashEntry> array;
        int currentSize;

        int findPos(const T & x) {
            int offset = 1;
            int currentPos = myhash(x);
            while (array[currentPos].info != EMPTY && array[currentPos].el != x) {
                currentPos += offset;
                // offset += 2;  // This line is commented out - should be uncommented for quadratic probing
                if (currentPos >= array.size()) currentPos -= array.size();
            }
            return currentPos;
        }

        bool isActive(int currentPos) const {
            return array[currentPos].info == ACTIVE;
        }

        void rehash() {
            vector<HashEntry> oldArray = array;
            array.resize(nextPrime(2 * oldArray.size()));
            for(auto & entry: array) entry.info = EMPTY;

            currentSize = 0;
            for (auto & entry: oldArray)
                if (entry.info == ACTIVE)
                    insert(std::move(entry.el));
        }

        int myhash(const T & x) const {
            int hashVal = 0;
            for (char ch: x) hashVal = 37 * hashVal + ch;  // ERROR: This assumes T can be iterated like a string
            return hashVal % array.size();
        }

        int nextPrime(int size) {
            while (!isPrime(size)) size += 1;
            return size;
        }

        bool isPrime(int n) {
            if (n < 2) return false;
            for (int i = 2; i * i <= n; i++)
                if (n % i == 0) return false;
            return true;
        }
};
