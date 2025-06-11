//
// Created by Катя on 6/2/2025.
//

#ifndef GRAPH_H
#define GRAPH_H
#include <fstream>
#include <random>
#include <set>
#include <sstream>
#include <unordered_set>
#include <string>
#include <stack>
#include <queue>

using namespace std;

class Graph {
public:
    int n; // кількість вершин
    std::vector<std::set<int>> adj;

    Graph() : n(0) {}
    Graph(int n) : n(n), adj(n) {}

    bool hasEdge(int u, int v) const;
    void addEdge(int u, int v);
    void printToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    void dfs(int start);
    void bfs(int start);

    bool bfsEarlyExit(int start, int goal);
    bool dfsEarlyExit(int start, int goal);
};


#endif //GRAPH_H
