//
// Created by Катя on 6/2/2025.
//

#ifndef GENERATEGRAPH_H
#define GENERATEGRAPH_H
#include "Graph.h"
namespace GraphGenerator {
    Graph randomGraph(int n, double edgeProb);
    Graph tree(int n);
    Graph grid(int rows, int cols);
    Graph denseGraph(int n);
    Graph sparseGraph(int n);
    Graph randomGraphFast(int n, int edgeCount);
}

#endif //GENERATEGRAPH_H
