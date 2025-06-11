//
// Created by Катя on 6/2/2025.
//

#include "Graph.h"
bool Graph::hasEdge(int u, int v) const {
    return adj[u].find(v) != adj[u].end();
}

void Graph::addEdge(int u, int v) {
    if (u != v) {
        adj[u].insert(v);
        adj[v].insert(u); // неорієнтований граф
    }
}

void Graph::printToFile(const std::string& filename) const {
    ofstream out(filename);
    for (int i = 0; i < n; ++i) {
        out << i << ":";
        for (int neighbor : adj[i]) {
            out << " " << neighbor;
        }
        out << "\n";
    }
    out.close();
}

void Graph::loadFromFile(const string& filename) {
    ifstream in(filename);
    string line;
    adj.clear();
    int maxNode = 0;

    while (getline(in, line)) {
        istringstream iss(line);
        int u;
        char colon;
        iss >> u >> colon;
        maxNode = max(maxNode, u);
        if (adj.size() <= u)
            adj.resize(u + 1);
        int v;
        while (iss >> v) {
            adj[u].insert(v);
        }
    }

    // Додати зворотні ребра для неорієнтованого графа
    for (int u = 0; u < adj.size(); ++u) {
        for (int v : adj[u]) {
            adj[v].insert(u);
        }
    }

    n = adj.size();
    in.close();
}

void Graph::dfs(int start) {
    vector<bool> visited(n, false);
    stack<int> s;
    s.push(start);

    while (!s.empty()) {
        int u = s.top();
        s.pop();
        if (!visited[u]) {
            visited[u] = true;
            for (int v : adj[u]) {
                if (!visited[v]) {
                    s.push(v);
                }
            }
        }
    }
}



void Graph::bfs(int start) {
    vector<bool> visited(n, false);
    queue<int> q;
    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
}

bool Graph::bfsEarlyExit(int start, int goal) {
    vector<bool> visited(n, false);
    queue<int> q;

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        if (u == goal) return true;

        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    return false;  // Ціль не знайдено
}

bool Graph::dfsEarlyExit(int start, int goal) {
    vector<bool> visited(n, false);
    stack<int> s;
    s.push(start);

    while (!s.empty()) {
        int u = s.top();
        s.pop();

        if (visited[u]) continue;
        visited[u] = true;

        if (u == goal) return true;

        for (int v : adj[u]) {
            if (!visited[v]) {
                s.push(v);
            }
        }
    }

    return false;
}
