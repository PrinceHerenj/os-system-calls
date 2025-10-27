using namespace std;

template<class T>
class List {
    private:
        struct Node {
            T el;
            Node * next;

            Node(const T & x = T(), Node * next = nullptr): el(x), next(next) {}
        };
        int theSize;
        Node * head;
    public:
        class iterator {
            private:
                Node * current;
            public:
                // constructor
                iterator(): current(nullptr) {}
                // copy constructor
                iterator(const iterator & rhs): current(rhs.current) {}
                // derefference
                T & operator*() {
                    return current->el;
                }
                // arrow
                T * operator->() {
                    return &(current->el);
                }
                // pre-increment
                iterator & operator++() {
                    if (current != nullptr) current = current->next;
                    return *this;
                }
                // post-increment
                iterator & operator++(int) {
                    iterator old = *this;
                    ++(*this);
                    return old;
                }
                // equals
                bool operator==(const iterator & rhs) const {
                    return current = rhs.current;
                }
                // not equals
                bool operator!=(const iterator & rhs) const {
                    return current != rhs.current;
                }
        };

        iterator begin() {
            return iterator(head);
        }

        iterator end() {
            return iterator(nullptr);
        }

        explicit List(int init = 0): theSize(init), head(nullptr) {}

        List(const List & rhs):theSize(0), head(nullptr) {
            Node * curr = rhs.head;
            while (curr != nullptr) {
                push_back(curr->el);
                curr = curr->next;
            }
        }

        ~List() {
            clear();
        }

        const List & operator= (const List & rhs) {
            if (this != &rhs) {
                clear();
                Node * curr = rhs.head;
                while (curr != nullptr) {
                    push_back(curr->el);
                    curr = curr->next;
                }
            }
            return *this;
        }

        int size() {
            return theSize;
        }

        bool empty() {
            return theSize == 0;
        }

        void clear() {
            while (!empty()) pop_front();
        }

        T & front() {
            return head->el;
        }

        T & back() {
            Node * curr = head;
            while (curr != nullptr) curr = curr->next;
            return curr->el;
        }

        void push_front() {}
        void push_back() {}
        void insertAtIndex(const T & x, int pos) {
            if (pos < 1 || pos > theSize + 1) return;
            if (pos == 1) {push_front(x); return;}
            else {
                Node * curr = head;
                while (--pos - 1) {
                    curr = curr->next;
                }
                Node * newNode = new Node(x, curr->next);
            }
        }
};
