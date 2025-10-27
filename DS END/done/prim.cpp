#include <iostream>
#include <vector>
using namespace std;

struct Edge {
    int destination;
    int weight;

    Edge(int d = 0, int w = 0): destination(d), weight(w) {}
};

struct Node {
    int vertex;
    int key;

    Node(int v = 0, int k = 0): vertex(v), key(k) {}

    bool operator<(const Node & other) const {
        return key > other.key;
    }
};

template<class T, class Comparator = less<T>>
class MinHeap {
    public:
        explicit MinHeap(int capacity = 150): array(capacity), currentSize(0) {}

        explicit MinHeap(vector<T> & items): array(items.size() + 10), currentSize(items.size()) {
            for (int i = 0; i < items.size(); i++) {
                array[i + 1] = items[i];
            }
            buildHeap();
        }

        bool isEmpty() {
            return currentSize == 0;
        }

        void makeEmpty() {
            for (int i = 0; i < array.size(); i++) {
                array[i] = 0;
            }
            currentSize = 0;
        }

        const T & findMin() const {
            return array[1];
        }

        void insert(const T & x) {
            if (currentSize == array.size() - 1) array.resize(array.size() * 2);

            int hole = ++currentSize;
            for (; hole > 1 && comp(array[hole/2], x); hole /= 2)
                array[hole] = std::move(array[hole / 2]);
            array[hole] = std::move(x);
        }

        void deleteMin() {
            if (isEmpty()) {cout << "Underflow" << endl; return;}
            array[1] = std::move(array[currentSize--]);
            percolateDown(1);
        }

        void deleteMin(T & minItem) {
            if (isEmpty()) {cout << "Underflow" << endl; return;}
            minItem = array[1];
            array[1] = std::move(array[currentSize--]);
            percolateDown(1);
        }
    private:
        vector<T> array;
        int currentSize;
        Comparator comp;

        void buildHeap() {
            for (int i = currentSize / 2; i > 0; i--)
                percolateDown(i);
        }

        void percolateDown(int hole) {
            int child;
            T temp = std::move(array[hole]);
            for(; hole * 2 <= currentSize; hole = child) {
                child = hole * 2;
                if (child != currentSize && comp(array[child], array[child + 1])) child++;
                if (comp(temp, array[child])) array[hole] = std::move(array[child]);
                else break;
            }
            array[hole] = std::move(temp);
        }
};

class Graph {
    private:
        int V;
        vector<vector<Edge> > adj;
        const int INF = 1e9;
    public:
        Graph(int vertices): V(vertices) {
            adj.resize(vertices);
        }

        void addEdge(int u, int v, int weight) {
            adj[u].push_back(Edge(v, weight));
            adj[v].push_back(Edge(u, weight));
        }

        void primMST(int start = 0) {
            vector<int> distance(V, 1e9);  // Error: Should be named 'key' for clarity in Prim's algorithm
            vector<int> parent(V, -1);
            vector<bool> inMST(V, false);

            MinHeap<Node> pq;

            distance[start] = 0;
            pq.insert(Node(start, 0));

            while (!pq.isEmpty()) {
                int u = pq.findMin().vertex;
                pq.deleteMin();

                if (inMST[u]) continue;  // Skip vertices already in MST
                inMST[u] = true;

                for (const Edge & edge: adj[u]) {
                    int v = edge.destination;
                    int weight = edge.weight;
                    if (!inMST[v] && weight < distance[v]) {
                        distance[v] = weight;  // Error: In Prim's, we update the key to the weight of the edge, not distance from source
                        parent[v] = u;

                        pq.insert(Node(v, distance[v]));  // Error: We should decrease key if vertex already in heap, not insert again
                    }
                }
            }

            printMST(parent, distance);  // Error: Function expects key array, we're passing distance
        }

        void printMST(const vector<int> & parent, const vector<int> & key) {
            int totalWeight = 0;
            for (int i = 1; i < V; i++) {
                cout << parent[i] << " -- " << i << " \tWeight: " << key[i] << endl;
                totalWeight += key[i];
            }

            cout << "Total Weight of MST: " << totalWeight << endl;
        }
};

int main() {
    Graph g(6);
    g.addEdge(0, 1, 2);
    g.addEdge(0, 2, 4);
    g.addEdge(1, 3, 3);
    g.addEdge(2, 3, 1);
    g.addEdge(2, 4, 5);
    g.addEdge(3, 5, 7);
    g.addEdge(4, 5, 2);

    g.primMST(0);
}
