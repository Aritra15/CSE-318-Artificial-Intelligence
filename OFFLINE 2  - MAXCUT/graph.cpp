#include<bits/stdc++.h>
using namespace std;

struct Graph{
    int n;
    vector<vector<int>> adjMatrix;

    Graph(int n){
        this->n = n;
        adjMatrix.resize(n, vector<int>(n, 0));
    }
    
    
    Graph(const string& filename) {
        ifstream file(filename);
        
        file >> this->n; 
        adjMatrix.resize(this->n, vector<int>(this->n, 0));
        
        int m; 
        file >> m;
        int u, v, w;

        for (int i = 0; i < m; ++i) {
            file >> u >> v >> w;
            u--; v--;
            if (u >= 0 && u < this->n && v >= 0 && v < this->n) {
                addEdge(u, v, w);
            }
        }
        file.close();
    }
    
    void addEdge(int u, int v, int weight) {
        adjMatrix[u][v] = weight;
        adjMatrix[v][u] = weight;
    }

    void loadGraphFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }
        int u, v, weight;
        while (file >> u >> v >> weight) {
            addEdge(u-1, v-1, weight);
        }
        file.close();
    }
};