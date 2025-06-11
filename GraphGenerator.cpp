#include "GraphGenerator.h"
#include <random>
#include <unordered_set>
using namespace std;

Graph GraphGenerator::randomGraph(int n, double edgeProb) {
    Graph g(n);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    uniform_int_distribution<> dist(0, n - 1);

    // Step 1: Create a spanning tree to ensure connectivity
    std::vector<int> nodes(n);
    iota(nodes.begin(), nodes.end(), 0);
    shuffle(nodes.begin(), nodes.end(), gen);

    for (int i = 1; i < n; ++i) {
        int u = nodes[i];
        int v = nodes[dist(gen) % i]; // connect to one of the previous nodes
        g.addEdge(u, v);
    }

    // Step 2: Add remaining edges randomly
    for (int u = 0; u < n; ++u) {
        for (int v = u + 1; v < n; ++v) {
            if (!g.hasEdge(u, v) && dis(gen) < edgeProb) {
                g.addEdge(u, v);
            }
        }
    }

    return g;
}

Graph GraphGenerator::tree(int n) {
    Graph g(n);
    for (int i = 1; i < n; ++i) {
        int parent = rand() % i;
        g.addEdge(i, parent);
    }
    return g;
}

Graph GraphGenerator::grid(int rows, int cols) {
    int n = rows * cols;
    Graph g(n);
    auto idx = [&](int r, int c) { return r * cols + c; };

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int u = idx(r, c);
            if (r + 1 < rows) g.addEdge(u, idx(r + 1, c));
            if (c + 1 < cols) g.addEdge(u, idx(r, c + 1));
        }
    }
    return g;
}

Graph GraphGenerator::denseGraph(int n) {
    return randomGraph(n, 0.8);
}

Graph GraphGenerator::sparseGraph(int n) {
    return randomGraph(n, 10. / n);
}

Graph GraphGenerator::randomGraphFast(int n, int edgeCount) {
    Graph g(n);
    unordered_set<long long> used;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, n - 1);

    while ((int)used.size() < edgeCount) {
        int u = dist(gen);
        int v = dist(gen);
        if (u == v) continue;

        long long key = min(u, v) * (long long)n + max(u, v);
        if (used.count(key)) continue;

        used.insert(key);
        g.addEdge(u, v);
    }
    return g;
}
