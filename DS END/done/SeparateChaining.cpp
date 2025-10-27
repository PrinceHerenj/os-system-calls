#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <vector>
using namespace std;

template<class T>
class HashTable {
    public:
        explicit HashTable(int size = 101): theLists(size), currentSize(0) {}

        bool contains(const T & x) const {
            auto & whichList = theLists[myhash(x)];
            return find(begin(whichList), end(whichList), x) != end(whichList);
        }

        void makeEmpty() {
            for (auto & thisList: theLists) thisList.clear();
            currentSize = 0;
        }

        bool insert(const T & x) {
            auto & whichList = theLists[myhash(x)];
            if (find(begin(whichList), end(whichList), x) != end(whichList)) return false;
            whichList.push_back(x);
            if (++currentSize > theLists.size()) rehash();
            return true;
        }

        bool remove(const T & x) {
            auto & whichList = theLists[myhash(x)];
            auto itr = find(begin(whichList), end(whichList), x);
            if (itr == end(whichList)) return false;

            whichList.erase(itr);
            --currentSize;
            return true;
        }

        int size() const {
            return currentSize;
        }
        int tableSize() const {
            return theLists.size();
        }

        void displayTable() {
            cout << "Bucket\tValues" << endl;
            for (int i = 0; i < theLists.size(); i++) {
                cout << i << "\t";
                auto & whichList = theLists[i];
                for (auto & item: whichList) cout << item << " -> ";
                cout << "NULL" << endl;
            }
            cout << "Total items: " << size() << endl << endl;
        }
    private:
        vector<list<T> > theLists;
        int currentSize;

        void rehash() {
            vector<list<T>> oldLists = theLists;
            theLists.resize(2 * theLists.size());

            for (auto & thisList : theLists) thisList.clear();

            currentSize = 0;
            for (auto & oldList: oldLists)
                for (auto & item: oldList)
                    insert(item);
        }

        int myhash(const T & x) const  {
            int hashVal = 0;
            for (char ch: x) hashVal = 37 * hashVal + ch;
            return hashVal % theLists.size();
        }
}; // Missing semicolon here was fixed
