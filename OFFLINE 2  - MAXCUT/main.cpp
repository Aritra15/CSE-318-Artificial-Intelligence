#include <bits/stdc++.h>
#include "Algorithms.cpp"
using namespace std;

int main(int argc, char *argv[])
{
    srand(time(0));
    double alpha = 0.6;
    int maxIterations = 50;

    if (argc != 3)
    {
        cerr << "Input format: " << argv[0] << " <inputFile> <algorithmType>" << endl;
        cerr << "Insert one of these algorithm types: greedy, randomized, semi_greedy, local_search, grasp" << endl;
        return 1;
    }

    string inputFile = argv[1];
    string algorithm = argv[2];

    Graph g(inputFile);

    if (algorithm == "greedy")
    {
        Cut resultCut = Algorithms::GreedyMaxcut(g);
        cout << resultCut.cut_value(g.adjMatrix) << endl;
    }
    else if (algorithm == "randomized")
    {
        Cut resultCut = Algorithms::RandomizedMaxcut(g);
        cout << resultCut.cut_value(g.adjMatrix) << endl;
    }
    else if (algorithm == "semi_greedy")
    {
        //Cut resultCut = Algorithms::SemiGreedyMaxcut(g, alpha);
        Cut resultCut = Algorithms::Semi_Greedy_Maxcut_Optimized(g, alpha);
        cout << resultCut.cut_value(g.adjMatrix) << endl;
    }
    else if (algorithm == "local_search")
    {

        int iterations=10;
        int totalCutValue=0;
        int totalIterationsInLocalSearch=0;
        for(int i=0;i<iterations;i++){
            
            Cut initialCut = Algorithms::Semi_Greedy_Maxcut_Optimized(g, alpha);
            pair<Cut, int> result = Algorithms::LocalSearchMaxcut(g, initialCut);
            totalCutValue += result.first.cut_value(g.adjMatrix);
            totalIterationsInLocalSearch += result.second;
        }
        cout << totalCutValue/iterations << " " << totalIterationsInLocalSearch << endl;
        
    }
    else if (algorithm == "grasp")
    {
        pair<Cut, int> result = Algorithms::GraspMaxcut(g, alpha, maxIterations);
        cout << result.first.cut_value(g.adjMatrix) << " " << result.second << endl;
    }
    else
    {
        cerr << "Error: Unknown algorithm type '" << algorithm << "'" << endl;
        cerr << "Supported algorithm types: greedy, randomized, semi_greedy, local_search, grasp" << endl;
        return 1;
    }
    return 0;
}
