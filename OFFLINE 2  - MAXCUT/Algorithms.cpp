#include<bits/stdc++.h>
#include "graph.cpp"
using namespace std;

struct Cut{
    set<int> x, y; 

    long long cut_value(const vector<vector<int>> &adj_matrix)
    {
        long long ret = 0;
        for (int i : x)
            for (int j : y)
                ret += adj_matrix[i][j];
        return ret;
    }

    void print() {
        cout << "X: ";
        for (int i : x) cout << i << " ";
        cout << "\nY: ";
        for (int i : y) cout << i << " ";
        cout << endl;
    }
};

class Algorithms{
    public:
    static Cut GreedyMaxcut(Graph &g);
    static Cut SemiGreedyMaxcut(Graph &g, double alpha);
    static Cut Semi_Greedy_Maxcut_Optimized(Graph &g, double alpha);
    static Cut RandomizedMaxcut(Graph &g);
    static pair<Cut,long long> LocalSearchMaxcut(Graph &g, Cut &cut);
    static pair<Cut,long long> GraspMaxcut(Graph &g, double alpha, int maxIterations);
};

Cut Algorithms::GreedyMaxcut(Graph &g){
    set<int> X,Y;

    int u,v;
    long long w=INT_MIN;

    for (int i=0;i<g.n;i++){
        for (int j=0;j<g.n;j++){
            if (g.adjMatrix[i][j] > w){
                u = i;
                v = j;
                w = g.adjMatrix[i][j];
            }
        }  
    }
    X.insert(u);
    Y.insert(v);

    for(int i=0;i<g.n;i++){
        if(X.count(i) || Y.count(i)){
            continue;
        }

        long long contributionIfInX = 0, contributionIfInY = 0;

        for( auto j: X){
            contributionIfInY += g.adjMatrix[i][j];
        }
        
        for( auto j: Y){
            contributionIfInX += g.adjMatrix[i][j];
        }

        if(contributionIfInX > contributionIfInY){
            X.insert(i);
        }
        else{
            Y.insert(i);
        }
    
    }
    Cut result;
    result.x = X;
    result.y = Y;
    return result;
}

Cut Algorithms::RandomizedMaxcut(Graph &g){
    srand(time(0));  
    
    int NUM_ITERATIONS = 5;
    
    Cut bestCut;
    long long bestCutValue = LLONG_MIN;
    
    
    for(int iter = 0; iter < NUM_ITERATIONS; iter++) {
        set<int> X, Y;
        
        for (int i = 0; i < g.n; i++) {
            if(rand() % 2 == 1) {  
                X.insert(i);
            } else {
                Y.insert(i);
            }
        }
        
        Cut currentCut;
        currentCut.x = X;
        currentCut.y = Y;
        
        long long currentCutValue = currentCut.cut_value(g.adjMatrix);
        
        if(currentCutValue > bestCutValue) {
            bestCutValue = currentCutValue;
            bestCut = currentCut;
        }
    }
    
    return bestCut;
}

Cut Algorithms::SemiGreedyMaxcut(Graph &g,double alpha){
    set<int> X,Y;
    set<int> remainingVertices;

    for (int i=0;i<g.n;i++){
        remainingVertices.insert(i);
    }

    int u=-1,v=-1;
    long long w=LLONG_MIN;

    for (int i=0;i<g.n;i++){
        for (int j=0;j<g.n;j++){
            if (g.adjMatrix[i][j] > w){
                u = i;
                v = j;
                w = g.adjMatrix[i][j];
            }
        }  
    }

    X.insert(u);
    Y.insert(v);
    remainingVertices.erase(u);
    remainingVertices.erase(v);

    while(!remainingVertices.empty()){
        //for each edge store the contributions in the form  (vertex, it contributes to x or y more, the max contribution)
        vector<tuple<int,int,long long>> contributions;

        long long maxContribution = LLONG_MIN;
        long long minContribution = LLONG_MAX;

        for(int vertex : remainingVertices){
            long long contributionX =0,contributionY=0;

            for ( auto y:Y){
                contributionX += g.adjMatrix[vertex][y];
            }

            for(auto x:X){
                contributionY += g.adjMatrix[vertex][x];
            }

            long long maxContVertex = max(contributionX, contributionY);
            int maxIndicator = (contributionX > contributionY) ? 0 : 1;

            contributions.push_back(make_tuple(vertex, maxIndicator, maxContVertex));

            if (maxContVertex > maxContribution){
                maxContribution = maxContVertex;
            }

            if (maxContVertex < minContribution){
                minContribution = maxContVertex;
            }
        }

        long long threshold = alpha * (maxContribution - minContribution) + minContribution;

        vector<tuple<int,int,long long>> rcl;
        for (auto &contribution : contributions){
            if (get<2>(contribution) >= threshold){
                rcl.push_back(contribution);
            }
        }

        int idx = rand() % rcl.size();

        int indicator = get<1>(rcl[idx]);
        int vertex = get<0>(rcl[idx]);

        if (indicator == 0){
            X.insert(vertex);
        }
        else{
            Y.insert(vertex);
        }

        remainingVertices.erase(vertex);
    }

    
    Cut result;
    result.x = X;
    result.y = Y;
    return result;
}

pair<Cut, long long> Algorithms::LocalSearchMaxcut(Graph &g, Cut &cut){
    srand(time(0));
    bool modified = true;
    long long iterations = 0;
    while (modified){
        modified = false;
        iterations++;

        for (int i=0;i<g.n;i++){
            long long contributionX = 0, contributionY = 0;

            for (auto y:cut.y){
                contributionX += g.adjMatrix[i][y];
            }

            for (auto x:cut.x){
                contributionY += g.adjMatrix[i][x];
            }

            if (cut.x.count(i) && contributionY > contributionX){
                cut.x.erase(i);
                cut.y.insert(i);
                modified = true;
                break;
            }
            else if (cut.y.count(i) && contributionX > contributionY){
                cut.y.erase(i);
                cut.x.insert(i);
                modified = true;
                break;
            }
        }
    }
    pair<Cut, long long> result;
    result.first = cut;
    result.second = iterations;
    return result;
}

pair<Cut,long long > Algorithms::GraspMaxcut(Graph &g, double alpha, int maxIterations){
    Cut bestCut;
    long long bestCutValue = LLONG_MIN;
    long long totalIterations = 0;

    for(int i=0;i<maxIterations;i++){
        //Cut cut = SemiGreedyMaxcut(g, alpha);
        Cut cut = Semi_Greedy_Maxcut_Optimized(g, alpha);
        pair<Cut,int> localResult = LocalSearchMaxcut(g, cut);
        Cut localCut = localResult.first;
        
        totalIterations += localResult.second;

        long long cutValue = localCut.cut_value(g.adjMatrix);

        if (cutValue > bestCutValue){
            bestCutValue = cutValue;
            bestCut = localCut;
        }
    }
    //return make_pair(bestCut, maxIterations);
    return make_pair(bestCut,totalIterations);
}

Cut Algorithms::Semi_Greedy_Maxcut_Optimized(Graph &g, double alpha){
    set<int> X, Y;
    set<int> remainingVertices;

    for (int i = 0; i < g.n; i++) {
        remainingVertices.insert(i);
    }

    int u = -1, v = -1;
    long long w = LLONG_MIN;

    for (int i = 0; i < g.n; i++) {
        for (int j = 0; j < g.n; j++) {
            if (g.adjMatrix[i][j] > w) {
                u = i;
                v = j;
                w = g.adjMatrix[i][j];
            }
        }  
    }

    X.insert(u);
    Y.insert(v);
    remainingVertices.erase(u);
    remainingVertices.erase(v);

    vector<tuple<int, long long, long long>> vertexContributions;
    
    for (int vertex : remainingVertices) {
        long long contributionX = g.adjMatrix[vertex][v]; 
        long long contributionY = g.adjMatrix[vertex][u]; 
        
        vertexContributions.push_back(make_tuple(vertex, contributionX, contributionY));
    }

    while (!remainingVertices.empty()) {
      
        long long maxContribution = LLONG_MIN;
        long long minContribution = LLONG_MAX;
        
        vector<tuple<int, int, long long>> contributions;
        
        for (auto &cont : vertexContributions) {
            int vertex = get<0>(cont);
            long long contribX = get<1>(cont);
            long long contribY = get<2>(cont);
            
            long long maxContVertex = max(contribX, contribY);
            int maxIndicator = (contribX > contribY) ? 0 : 1;
            
            contributions.push_back(make_tuple(vertex, maxIndicator, maxContVertex));
            
            maxContribution = max(maxContribution, maxContVertex);
            minContribution = min(minContribution, maxContVertex);
        }
    
        long long threshold = alpha * (maxContribution - minContribution) + minContribution;
        
        vector<tuple<int, int, long long>> rcl;
        for (auto &contribution : contributions) {
            if (get<2>(contribution) >= threshold) {
                rcl.push_back(contribution);
            }
        }
        
        int idx = rand() % rcl.size();
        int indicator = get<1>(rcl[idx]);
        int selectedVertex = get<0>(rcl[idx]);
        
        if (indicator == 0) {
            X.insert(selectedVertex);
        } else {
            Y.insert(selectedVertex);
        }

        remainingVertices.erase(selectedVertex);
        
        vector<tuple<int, long long, long long>> updatedContributions;
        
        for (auto &contrib : vertexContributions) {
            int vertex = get<0>(contrib);
            
            if (vertex == selectedVertex) continue; 
            
            long long contribX = get<1>(contrib);
            long long contribY = get<2>(contrib);
            
            if (indicator == 0) { 
                contribY += g.adjMatrix[vertex][selectedVertex];
            } else { 
                contribX += g.adjMatrix[vertex][selectedVertex];
            }
            
            updatedContributions.push_back(make_tuple(vertex, contribX, contribY));
        }
        
        vertexContributions = updatedContributions;
    }
    
    Cut result;
    result.x = X;
    result.y = Y;
    return result;
}