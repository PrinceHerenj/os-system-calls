#include <iostream>
using namespace std;

template <class T, class Comparator = less<T> >
class BST {
    private:
        struct Node {
            T el;
            Node * l;
            Node * r;
            int freq;

            Node (const T & x = T(), Node * l = nullptr, Node * r = nullptr): el(x), l(l), r(r), freq(1) {}
        };

        Node * root;
        Comparator lt;

        void insert(const T & x, Node * & t) {
            if (t == nullptr) t = new Node(x);
            else if (lt(x, t->el)) insert(x, t->l);
            else if (lt(t->el, x)) insert(x, t->r);
            else t->freq++;
        }

        void remove(const T & x, Node * & t) {
            if (t == nullptr) return;
            if (lt(x, t->el)) remove(x, t->l);
            else if (lt(t->el, x)) remove(x, t->r);
            else if (t->l != nullptr && t->r != nullptr) {
                t->el = findMax(t->l)->el;
                remove(t->el, t->l);
            } else {
                Node * temp = t;
                t = t->l != nullptr ? t->l : t->r;
                delete temp;
            }
        }
        Node * findMin(Node * t) {
            if (t == nullptr) return nullptr;
            if (t->l == nullptr) return t;
            return findMin(t->l);
        }

        Node * findMax(Node * t) {
            if (t == nullptr) return nullptr;
            if (t->r == nullptr) return t;
            return findMax(t->r);
        }

        bool contains(const T & x, Node * t) {
            if (t == nullptr) return false;
            else if (lt(x, t->el)) return contains(x, t->l); // ERROR: missing return statement
            else if (lt(t->el, x)) return contains(x, t->r); // ERROR: missing return statement
            else return true;
        }

        void makeEmpty(Node * & t) {
            if (t != nullptr) {
                makeEmpty(t->l);
                makeEmpty(t->r);
                delete t;
            }
            t = nullptr; // ERROR: was using == operator instead of assignment =
        }

        Node * clone(Node * t) {
            if (t == nullptr) return nullptr;
            return new Node(t->el, clone(t->l), clone(t->r));
        }

        void traverse(Node * t) {
            if (t == nullptr) return;
            traverse(t->l);
            cout << t->el << " ";
            traverse(t->r);
        }
    public:
        BST(): root(nullptr) {}
        BST(BST & rhs): root(clone(rhs.root)) {}
        ~BST() {makeEmpty();}

        bool isEmpty() const {return root == nullptr; }
        void insert(const T & x) {insert(x, root);}
        void remove(const T & x) {remove(x, root);}
        const T & findMin() const {return findMin(root)->el;} // ERROR: should return el, not Node
        const T & findMax() const {return findMax(root)->el;} // ERROR: should return el, not Node
        bool contains(const T & x) {return contains(x, root);}
        void makeEmpty() {makeEmpty(root);}
        void traverse() {traverse(root);}
};
