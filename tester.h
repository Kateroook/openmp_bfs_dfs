#include <iostream>
#include <vector>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <map>
#include "TestResult.h"
#include "ParallelGraphTraversal.h"
#include "GraphGenerator.h"

using namespace std;


void printSummaryStatistics(const vector<Result>& results) {
    cout << "\nSUMMARY STATISTICS\n";
    cout << string(70, '=') << "\n";

    // Find best speedups for each graph type and algorithm
    map<string, map<string, double>> bestSpeedups;
    map<string, map<string, int>> bestThreads;
    map<string, map<string, int>> bestVertex;

    for (const auto& r : results) {
        if (r.threads > 1) { // Only parallel results
            string key = r.graphType + " - " + r.algorithmName;
            if (r.speedup > bestSpeedups[r.graphType][r.algorithmName]) {
                bestSpeedups[r.graphType][r.algorithmName] = r.speedup;
                bestThreads[r.graphType][r.algorithmName] = r.threads;
                bestVertex[r.graphType][r.algorithmName] = r.vertices;
            }
        }
    }

    cout << left << setw(20) << "Graph Type"
         << setw(15) << "Algorithm"
         << setw(15) << "Best Speedup"
         << setw(12) << "Threads"
         << setw(12) << "Vertices" << "\n";
    cout << string(70, '-') << "\n";

    for (const auto& graphType : bestSpeedups) {
        for (const auto& algorithm : graphType.second) {
            cout << left << setw(20) << graphType.first
                 << setw(15) << algorithm.first
                 << setw(15) << fixed << setprecision(2) << algorithm.second
                 << setw(12) << bestThreads[graphType.first][algorithm.first]
                 << setw(12) << bestVertex[graphType.first][algorithm.first] << "\n";
        }
    }

    cout << string(70, '=') << "\n";
}

void runTests() {
    vector<int> sizes = {100, 1000, 10000};
    vector<int> threads = {2, 4, 8, 16, 32};
    vector<Result> results;

    // Define graph types and their generators
    struct GraphType {
        string name;
        function<Graph(int)> generator;
    };

    vector<GraphType> graphTypes = {
        {"Random Tree", [](int n) { return GraphGenerator::tree(n); }},
        {"Grid Graph", [](int n) {
            int side = (int)sqrt(n);
            return GraphGenerator::grid(side, side);
        }},
        {"Dense Graph", [](int n) {
            if (n >= 5000) {
                cout << "    Skipping Dense Graph for size " << n << " (too large)\n";
                return Graph(0); // Return empty graph as placeholder
            }
            return GraphGenerator::denseGraph(n);
        }},
        {"Sparse Graph", [](int n) {
            /*int edges = n * 2; // About 2 edges per vertex
            return GraphGenerator::randomGraphFast(n, edges);*/
            return GraphGenerator::sparseGraph(n);
        }}
    };

    // Define algorithms
    struct Algorithm {
        string name;
        function<void(const Graph&, int)> seqFunc;
        function<int(const Graph&, int, int)> parFunc;
    };

    vector<Algorithm> algorithms = {
        {"DFS",
         [](const Graph& g, int start) { const_cast<Graph&>(g).dfs(start); },
         [](const Graph& g, int start, int t) { return Parallel::parallelDFS(g, start, t); }},
        {"BFS",
         [](const Graph& g, int start) { const_cast<Graph&>(g).bfs(start); },
         [](const Graph& g, int start, int t) { return Parallel::parallelBFS(g, start, t); }}
    };

    cout << "Running comprehensive graph traversal tests...\n";
    cout << "Graph sizes: ";
    for (int s : sizes) cout << s << " ";
    cout << "\nThread counts: ";
    for (int t : threads) cout << t << " ";
    cout << "\n" << string(100, '=') << "\n";

    for (const auto& graphType : graphTypes) {
        cout << "Testing " << graphType.name << "...\n";

        for (int size : sizes) {
            cout << "  Size " << size << "... ";

            // Generate graph once for this size
            Graph graph = graphType.generator(size);

            if (graph.n == 0) {
                cout << "Skipped (too large for dense graph)\n";
                continue;
            }

            for (const auto& algorithm : algorithms) {
                // Sequential baseline
                Graph seqGraph = graph; // Copy for sequential test
                auto start_seq = chrono::high_resolution_clock::now();
                algorithm.seqFunc(seqGraph, 0);
                auto end_seq = chrono::high_resolution_clock::now();
                double time_seq = chrono::duration<double, std::milli>(end_seq - start_seq).count();

                results.push_back({
                    graphType.name,
                    "Seq " + algorithm.name,
                    "Traversal",
                    size,
                    1,
                    time_seq,
                    1.0,
                    size
                });

                // Parallel versions
                for (int t : threads) {
                    auto start_par = chrono::high_resolution_clock::now();
                    int visited = algorithm.parFunc(graph, 0, t);
                    auto end_par = chrono::high_resolution_clock::now();
                    double time_par = chrono::duration<double, std::milli>(end_par - start_par).count();

                    double speedup = (time_par > 0) ? time_seq / time_par : 0.0;

                    results.push_back({
                        graphType.name,
                        algorithm.name,
                        "Traversal",
                        size,
                        t,
                        time_par,
                        speedup,
                        visited
                    });
                }
            }
            cout << "Done\n";
        }
        cout << "\n";
    }

    // Print results in a formatted table
    cout << "\n" << string(120, '=') << "\n";
    cout << "COMPREHENSIVE GRAPH TRAVERSAL PERFORMANCE RESULTS\n";
    cout << string(120, '=') << "\n";

    cout << left << setw(15) << "Graph Type"
         << setw(15) << "Algorithm"
         << setw(12) << "Test Type"
         << setw(10) << "Vertices"
         << setw(8) << "Threads"
         << setw(12) << "Time (ms)"
         << setw(10) << "Speedup"
         << setw(12) << "Visited" << "\n";
    cout << string(120, '-') << "\n";

    // Group results by graph type for better readability
    string currentGraphType = "";
    for (const auto& r : results) {
        if (r.graphType != currentGraphType) {
            if (!currentGraphType.empty()) {
                cout << string(120, '-') << "\n";
            }
            currentGraphType = r.graphType;
        }

        cout << left << setw(15) << r.graphType
             << setw(15) << r.algorithmName
             << setw(12) << r.testType
             << setw(10) << r.vertices
             << setw(8) << r.threads
             << setw(12) << fixed << setprecision(2) << r.timeMs
             << setw(10) << fixed << setprecision(2) << r.speedup
             << setw(12) << r.visited << "\n";
    }

    cout << string(120, '=') << "\n";

    // Print summary statistics
    printSummaryStatistics(results);
}

