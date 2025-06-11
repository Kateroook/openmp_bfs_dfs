//
// Created by Катя on 6/2/2025.
//

#ifndef PARALLELGRAPHTRAVERSAL_H
#define PARALLELGRAPHTRAVERSAL_H
#ifndef PARALLELGRAPH_H
#define PARALLELGRAPH_H

#include "Graph.h"
#include <chrono>

namespace Parallel {
    int parallelDFS(const Graph& g, int start, int threads);
    int parallelBFS(const Graph& g, int start, int threads);
}

#endif

#endif //PARALLELGRAPHTRAVERSAL_H
