#ifndef ROAD_GRAPH_H
#define ROAD_GRAPH_H

#include <vector>
#include <queue>
#include <utility>
#include <limits>
#include <algorithm>
#include <functional>

// Result of one shortest-path query
struct PathResult {
    bool found = false;
    int distance = -1;        // total km
    std::vector<int> path;    // node ids from source to destination
};

// ---------------------------------------------------------------
// Weighted, undirected road network stored as an adjacency list.
// Nodes are numbered 1..n. Edge weight = road length in km.
// Dijkstra's algorithm gives the shortest route in
// O((V + E) log V) using a min-priority-queue for the frontier.
// ---------------------------------------------------------------
class RoadGraph {
private:
    int n;
    std::vector<std::vector<std::pair<int, int>>> adj;   // (neighbour, km)

public:
    explicit RoadGraph(int nodes) : n(nodes), adj(nodes + 1) {}

    // roads are two-way
    void addRoad(int u, int v, int km) {
        adj[u].push_back({v, km});
        adj[v].push_back({u, km});
    }

    PathResult shortestPath(int src, int dst) const {
        const int INF = std::numeric_limits<int>::max();
        std::vector<int> dist(n + 1, INF);
        std::vector<int> parent(n + 1, -1);

        typedef std::pair<int, int> State;   // (distance so far, node)
        std::priority_queue<State, std::vector<State>, std::greater<State> > pq;

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            State top = pq.top();
            pq.pop();
            int d = top.first;
            int u = top.second;

            if (d > dist[u]) continue;   // outdated entry, skip
            if (u == dst) break;         // shortest path to dst is final

            for (const auto& edge : adj[u]) {
                int v = edge.first;
                int w = edge.second;
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        PathResult result;
        if (dist[dst] == INF) return result;   // unreachable

        result.found = true;
        result.distance = dist[dst];
        for (int v = dst; v != -1; v = parent[v]) result.path.push_back(v);
        std::reverse(result.path.begin(), result.path.end());
        return result;
    }
};

#endif
