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
    int distance;
    Node(int v = 0, int d = 0): vertex(v), distance(d) {}

    bool operator<(const Node & other) const {
        return distance < other.distance;
    }
};

template<class T, class Comparator = less<T>>
class MinHeap {
    public:
        explicit MinHeap(int cap = 150): array(cap), currentSize(0) {}

        explicit MinHeap(vector<T> & items): array(items.size() + 10), currentSize(items.size()) {
            for (int i = 0; i < items.size(); i++)
                array[i + 1] = items[i];
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

        const T & findMin() {
            if (currentSize == 0) return T();
            return array[1];
        }

        void insert(const T & x) {
            if (currentSize == array.size() - 1) array.resize(array.size()  * 2);
            int hole = ++currentSize;
            for (; hole > 1 && comp(x, array[hole/2]); hole /= 2)
                array[hole] = std::move(array[hole/2]);
            array[hole] = std::move(x);
        }

        void deleteMin() {
            if (currentSize == 0) return;
            array[1] = std::move(array[currentSize--]);
            percolateDown(1);
        }

        void deleteMin(T & minItem) {
            if (currentSize == 0) return;
            minItem = std::move(array[1]);
            array[1] = std::move(array[currentSize--]);
            percolateDown(1);
        }

    private:
        vector<T> array;
        int currentSize;
        Comparator comp;
        void buildHeap() {
            for (int i = currentSize /2; i > 0; i--)
                percolateDown(i);
        }

        void percolateDown(int hole) {
            int child;
            T temp = std::move(array[hole]);
            for (; hole * 2 <= currentSize; hole = child) {
                child = hole * 2;
                if (child != currentSize && comp(array[child+1], array[child])) child++;
                if (comp(array[child], temp)) array[hole] = std::move(array[child]);
                else break;
            }
            array[hole] = std::move(temp);
        }
};

template<class T>
class Graph {
    private:
        int V;
        vector<Edge> adj;
    public:
        Graph(int vertices): V(vertices) {
            adj.resize(vertices);
        }

        void addEdge(int u, int v, int weight) {
            adj[u].push_back(Edge(v, weight));
        }

        void dijkstra(int source, int destination) {
            MinHeap<Node> pq;
            vector<int> distance(V, 1e9);
            vector<int> parent(V, -1);

            distance[source] = 0;
            pq.insert(Node(source, 0));

            while (!pq.isEmpty()) {
                int u = pq.findMin().vertex;
                int dist_u = pq.findMin().distance;

                pq.deleteMin();

                if (u == destination) break;
                if (dist_u > distance[u]) continue;

                for (const Edge & edge: adj[u]) {
                    int v = edge.destination;
                    int weight = edge.weight;

                    if (distance[u] + weight < distance[v]) {
                        distance[v] = distance[u] + weight;
                        parent[v] = u;

                        pq.insert(Node(v, distance[v]));
                    }
                }
            }

            if (distance[destination] == 1e9) {
                cout << "No path Exists" << endl;
                return;
            }

            vector<int> path;
            int current = destination;
            path.push_back(destination);
            while (parent[current] != -1) {
                path.push_back(parent[current]);
                current = parent[current];
            }

            for (int i = path.size() -1 ; i >= 0; i--)
                cout << path[i] << " ";
        }
};
