#include "ParallelGraphTraversal.h"

#include <iostream>
#include <omp.h>
#include <thread>

int Parallel::parallelDFS(const Graph& g, int start, int threads) {
    std::vector<std::atomic<bool>> visited(g.n);
    std::atomic<int> count = 0;

    // Each thread has its own work deque
    std::vector<std::deque<int>> workDeques(threads);
    std::vector<std::mutex> dequeMutexes(threads);
    std::atomic<int> activeThreads = threads;

    // Initialize first thread's deque with start node
    workDeques[0].push_back(start);

#pragma omp parallel num_threads(threads)
    {
        int threadId = omp_get_thread_num();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, threads - 1);

        while (activeThreads.load() > 0) {
            int node = -1;

            // Try to get work from own deque
            {
                std::lock_guard<std::mutex> lock(dequeMutexes[threadId]);
                if (!workDeques[threadId].empty()) {
                    node = workDeques[threadId].back(); // LIFO for DFS
                    workDeques[threadId].pop_back();
                }
            }

            // If no local work, try to steal from other threads
            if (node == -1) {
                for (int attempts = 0; attempts < threads && node == -1; attempts++) {
                    int victimId = dis(gen);
                    if (victimId != threadId) {
                        std::lock_guard<std::mutex> lock(dequeMutexes[victimId]);
                        if (!workDeques[victimId].empty()) {
                            node = workDeques[victimId].front(); // FIFO for stealing
                            workDeques[victimId].pop_front();
                        }
                    }
                }
            }

            if (node != -1) {
                // Process the node
                if (!visited[node].exchange(true)) {
                    count.fetch_add(1);

                    // Add neighbors to local deque
                    std::lock_guard<std::mutex> lock(dequeMutexes[threadId]);
                    for (int v : g.adj[node]) {
                        if (!visited[v].load()) {
                            workDeques[threadId].push_back(v);
                        }
                    }
                }
            } else {
                // No work found - check if should terminate
                bool hasGlobalWork = false;
                for (int i = 0; i < threads; i++) {
                    std::lock_guard<std::mutex> lock(dequeMutexes[i]);
                    if (!workDeques[i].empty()) {
                        hasGlobalWork = true;
                        break;
                    }
                }

                if (!hasGlobalWork) {
                    activeThreads.fetch_sub(1);
                    break;
                }

                std::this_thread::yield();
            }
        }
    }

    return count.load();
}

int Parallel::parallelBFS(const Graph& g, int start, int threads) {
    std::vector<std::atomic<bool>> visited(g.n);
    std::vector<int> frontier = { start };
    visited[start] = true;
    std::atomic<int> count = 1;

    while (!frontier.empty()) {
        std::vector<std::vector<int>> localNext(threads);

#pragma omp parallel num_threads(threads)
        {
            int tid = omp_get_thread_num();
            std::vector<int>& next = localNext[tid];

#pragma omp for schedule(dynamic) nowait
            for (int i = 0; i < frontier.size(); ++i) {
                int u = frontier[i];
                for (int v : g.adj[u]) {

                        if (!visited[v].exchange(true)) {
                            next.push_back(v);
                            count++;
                        }

                }
            }
        }

        std::vector<int> newFrontier;
        for (const auto& vec : localNext)
            newFrontier.insert(newFrontier.end(), vec.begin(), vec.end());

        frontier = std::move(newFrontier);
    }

    return count;
}