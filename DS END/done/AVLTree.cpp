#include <iostream>
using namespace std;

template <class T, class Comparator = less<T> >
class AVLTree {
    private:
        struct AVLNode {
            T el;
            AVLNode * l;
            AVLNode * r;
            int height;

            AVLNode(const T & el = T(), AVLNode * l = nullptr, AVLNode * r = nullptr): el(el), l(l), r(r), height(0) {}
        };

        AVLNode * root;
        Comparator lt;

        int height(AVLNode * t) const {
            return t == nullptr ? - 1 : t->height;
        }

        AVLNode * findMin(AVLNode * t) {
            if (t == nullptr) return nullptr;
            if (t->l == nullptr) return t;
            return findMin(t->l);
        }

        AVLNode * findMax(AVLNode * t) {
            if (t == nullptr) return nullptr;
            if (t->r == nullptr) return t;
            return findMax(t->r);
        }

        void rotateRightWithHoldingLeftChild(AVLNode * & k2) {
            AVLNode * k1 = k2->l;
            k2->l = k1->r;
            k1->r = k2;
            k2->height = max(height(k2->l), height(k2->r)) + 1;
            k1->height = max(height(k1->l), k2->height) + 1;
            k2 = k1;
        }

        void rotateLeftWithHoldingRightChild(AVLNode * & k1) {
            AVLNode * k2 = k1->r;
            k1->r = k2->l;
            k2->l = k1;
            k1->height = max(height(k1->l), height(k1->r)) + 1;
            k2->height = max(height(k2->r), k1->height) + 1;
            k1 = k2;
        }

        void doubleRotateWithHoldingLeftChild(AVLNode * & k3) {
            rotateLeftWithHoldingRightChild(k3->l);
            rotateRightWithHoldingLeftChild(k3);
        }

        void doubleRotateWithHoldingRightChild(AVLNode * & k3) {
            rotateRightWithHoldingLeftChild(k3->r);
            rotateLeftWithHoldingRightChild(k3);
        }

        void insert(const T & x, AVLNode * & t) {
            if (t == nullptr) t = new AVLNode(x);
            else if (lt(x, t->el)) insert(x, t->l);
            else if (lt(t->el, x)) insert(x, t->r);
            else ; // Duplicates are silently ignored
            balance(t);
        }

        void remove(const T & x, AVLNode * & t) {
            if (t == nullptr) return;
            if (lt(x, t->el)) remove(x, t->l);
            else if (lt(t->el, x)) remove(x, t->r);
            else if (t->l != nullptr && t->r != nullptr) {
                t->el = findMin(t->r)->el;
                remove(t->el, t->r);
            } else {
                AVLNode * old = t;
                t = t->l != nullptr ? t->l : t->r;
                delete old;
            }

            balance(t);
        }

        void balance(AVLNode * & t) {
            if (t == nullptr) return;
            if (height(t->l) - height(t->r) > 1) {
                if (height(t->l->l) >= height(t->l->r))
                    rotateRightWithHoldingLeftChild(t);
                else doubleRotateWithHoldingLeftChild(t);
            }
            else if (height(t->r) - height(t->l) > 1) {
                if (height(t->r->r) >= height(t->r->l))
                    rotateLeftWithHoldingRightChild(t);
                else doubleRotateWithHoldingRightChild(t);
            }
            t->height = max(height(t->l), height(t->r)) + 1;
        }

        void traverse(AVLNode * t) const {
            if (t == nullptr) return;
            traverse(t->l);
            cout << t->el << " ";
            traverse(t->r);
        }
    public:
        AVLTree(): root(nullptr) {}

        void insert(const T & x) {
            insert(x, root);
        }

        void remove(const T & x) {
            remove(x, root);
        }

        void traverse() const {
            traverse(root);
            cout << endl;
        }
};
