#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>
#include <chrono>
using namespace std;

class Graph {
    int v, e;
    vector<vector<int>> g;
    vector<bool> vis;

public:
    Graph() {
        cout << "Enter vertices and edges: ";
        cin >> v >> e;
        g.resize(v);

        for (int i = 0, a, b; i < e; i++) {
            cout << "Enter edge: ";
            cin >> a >> b;
            g[a].push_back(b);
            g[b].push_back(a);
        }
    }

    void reset() { vis.assign(v, false); }

    void print() {
        for (int i = 0; i < v; i++) {
            cout << i << " -> ";
            for (int j : g[i]) cout << j << " ";
            cout << endl;
        }
    }

    void dfs(int s) {
        stack<int> st;
        st.push(s);
        vis[s] = 1;

        while (!st.empty()) {
            int x = st.top();
            st.pop();
            cout << x << " ";

            for (int i : g[x])
                if (!vis[i]) st.push(i), vis[i] = 1;
        }
    }

    void pdfs(int s) {
        stack<int> st;
        st.push(s);
        vis[s] = 1;

        while (!st.empty()) {
            int x = st.top();
            #pragma omp critical
            st.pop();

            cout << x << " ";

            #pragma omp parallel for
            for (int k = 0; k < g[x].size(); k++) {
                int i = g[x][k];
                if (!vis[i]) {
                    #pragma omp critical
                    st.push(i), vis[i] = 1;
                }
            }
        }
    }

    void bfs(int s) {
        queue<int> q;
        q.push(s);
        vis[s] = 1;

        while (!q.empty()) {
            int x = q.front();
            q.pop();
            cout << x << " ";

            for (int i : g[x])
                if (!vis[i]) q.push(i), vis[i] = 1;
        }
    }

    void pbfs(int s) {
        queue<int> q;
        q.push(s);
        vis[s] = 1;

        while (!q.empty()) {
            int x = q.front();
            #pragma omp critical
            q.pop();

            cout << x << " ";

            #pragma omp parallel for
            for (int k = 0; k < g[x].size(); k++) {
                int i = g[x][k];
                if (!vis[i]) {
                    #pragma omp critical
                    q.push(i), vis[i] = 1;
                }
            }
        }
    }
};

int main() {
    Graph g;

    cout << "\nAdjacency List:\n";
    g.print();

    auto test = [&](string name, auto func) {
        g.reset();
        cout << "\n" << name << ": ";
        auto s = chrono::high_resolution_clock::now();
        func();
        auto e = chrono::high_resolution_clock::now();

        cout << "\nTime: "
             << chrono::duration_cast<chrono::microseconds>(e - s).count()
             << " microseconds\n";
    };

    test("DFS", [&]() { g.dfs(0); });
    test("Parallel DFS", [&]() { g.pdfs(0); });
    test("BFS", [&]() { g.bfs(0); });
    test("Parallel BFS", [&]() { g.pbfs(0); });

    return 0;
}



For compiling
g++ -fopenmp graph.cpp -o graph
./graph
