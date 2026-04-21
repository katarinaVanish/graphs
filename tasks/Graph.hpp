#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>

using namespace std;

class Graph
{
private:
    struct Edge
    {
        int to;
        double weight;
    };

    struct DSU
    {
        vector<int> parent, rank;
        DSU(int n)
        {
            parent.resize(n);
            rank.resize(n, 0);
            for (int i=0; i<n; i++)
            {
                parent[i] = i;
            }
        }

        int find(int v)
        {
            if (v==parent[v]) return v;
            return parent[v] =find(parent[v]);
        }

        void unite(int a, int b)
        {
            a = find(a);
            b = find(b);
            if (a != b)
            {
                if (rank[a] < rank[b]) swap(a, b);
                parent[b] = a;
                if (rank[a] == rank[b]) rank[a]++;
            }
        }
    };

    vector<vector<Edge>> adj;
    bool directed;
    bool weighted;
    vector<string> vertexNames;
    unordered_map<string, int> nameToIndex;
    void checkVertex(int v) const;

public:
    // Конструкторы
    Graph(bool directed, bool weighted);
    Graph(int vertices, bool directed, bool weighted);
    Graph(const string& filename);
    Graph(const Graph& other);

    // Методы
    void addVertex(const string& name);
    void addEdge(const string& from, const string& to, double weight);
    void addEdgeByIndex(int u, int v, double weight);
    void removeVertex(const string& name);
    void removeEdge(const string& from, const string& to);

    vector<tuple<string,string,double>> getEdgeList() const;
    bool isDirected() const;
    bool isWeighted() const;

    void saveToFile(const string& filename) const;
    void print() const;

    //2
    void printAdjacent(const string& name) const;
    //3
    vector<int> computeInDegrees() const;
    void printPedantVertices() const;
    //5
    bool dfsCycle(int v, int parent, vector<int>& color) const;
    bool isAcyclic() const;

    //6
    void shortestPath(const string& from, const string& to) const;

    //7 
    void kruskalMST() const;

    //8
    void findCenterAndRadius() const;

    //9
    void shortestPathsFrom(const string& startName,
                            const string& v1Name,
                            const string& v2Name) const;
};
