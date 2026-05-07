#include <omp.h>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

class Graph {

private:
    int vertices;
    vector<vector<int>> adjList;

public:
    Graph(int vertices){
        this->vertices = vertices;
        adjList.resize(vertices);
    }

    void addEdge(int u, int v){
        adjList[u].push_back(v);
        adjList[v].push_back(u);
    }

    void printGraph(){
        for(auto ai:adjList){
            for(auto x:ai){
                cout<<x<<"  ";
            }
            cout<<endl;
        }
    }

    // ================= BFS Parallel =================
    void BFS(int start){
        vector<bool> visited(vertices, false);
        queue<int> q;

        q.push(start);
        visited[start] = true;

        while(!q.empty()){
            int current = q.front();
            q.pop();

            cout << "Thread " << omp_get_thread_num() 
                 << " visited " << current << endl;

            // Parallelizing neighbor traversal
            // #pragma omp parallel for num_threads(4)
            #pragma omp parallel for
            for(int i = 0; i < adjList[current].size(); i++){
                int neighbor = adjList[current][i];

                bool shouldAdd = false;

                #pragma omp critical
                {
                    if(!visited[neighbor]){
                        visited[neighbor] = true;
                        shouldAdd = true;
                    }
                }

                if(shouldAdd){
                    #pragma omp critical
                    q.push(neighbor);
                }
            }
        }
    }

    // ================= DFS Parallel =================
    void DFS(int start){
        vector<bool> visited(vertices, false);
        DFSUntil(start, visited);
    }

    void DFSUntil(int v, vector<bool> &visited){

        bool shouldVisit = false;

        #pragma omp critical
        {
            if(!visited[v]){
                visited[v] = true;
                shouldVisit = true;
            }
        }

        if(!shouldVisit) return;

        cout << "Thread " << omp_get_thread_num() 
             << " visited " << v << endl;

        // Parallel recursion (limited usefulness)
        #pragma omp parallel for num_threads(6)
        //  #pragma omp parallel for
        for(int i = 0; i < adjList[v].size(); i++){
            int u = adjList[v][i];
            if(!visited[u]){
                DFSUntil(u, visited);
            }
        }
    }
};

int main() {

    Graph g(12);
        g.addEdge(0, 1);
        g.addEdge(0, 2);
        g.addEdge(1, 3);
        g.addEdge(2, 1);
        g.addEdge(2, 0);
        g.addEdge(2, 4);
        g.addEdge(3, 4);
        g.addEdge(3, 5);
        g.addEdge(4, 5);
        g.addEdge(4, 0);
        g.addEdge(5, 6);
        g.addEdge(5, 7);
        g.addEdge(6, 7);
        g.addEdge(6, 8);
        g.addEdge(7, 0);
        g.addEdge(7, 8);
        g.addEdge(7, 9);
        g.addEdge(8, 9);
        g.addEdge(8, 10);
        g.addEdge(9, 10);
        g.addEdge(9, 11);
        g.addEdge(10, 11);
        g.addEdge(10, 0);
        g.addEdge(11, 0);

    cout << "Graph:\n";
    g.printGraph();

    cout << "\nBFS traversal:\n";
    g.BFS(0);

    cout << "\nDFS traversal:\n";
    g.DFS(0);

    return 0;
}

// control the number of threads using omp_set_num_threads()
// /opt/homebrew/bin/g++-15 -fopenmp graph.cpp -o graph
// ./graph