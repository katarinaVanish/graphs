#include "Graph.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <tuple>
#include <queue>
#include <limits>
#include <functional>

using namespace std;


// Конструктор по умолчанию
Graph::Graph(bool directed, bool weighted)
{
    this->directed = directed;
    this->weighted = weighted;
}

// Конструктор с количеством вершин
Graph::Graph(int vertices, bool directed, bool weighted)
{
    if (vertices < 0)
    {
        throw runtime_error("Количество вершин должно быть неотрицательным");
    }
    this->directed = directed;
    this->weighted = weighted;
    adj.resize(vertices);
    vertexNames.resize(vertices);

    for (int i=0; i< vertices; ++i)
    {
        vertexNames[i]= "v" + to_string(i);
        nameToIndex[vertexNames[i]] = i;
    }
}

// Конструктор из файла
Graph::Graph(const string& filename)
{
    ifstream file(filename);
    if (!file)
    {
        throw runtime_error("Ошибка открытия файла");
    }
        
    int vertices;
    int dirFlag;

    if (!(file >> vertices >> dirFlag))
        throw runtime_error("Файл пустой или неверный формат");

    if (vertices <= 0)
        throw runtime_error("Некорректное количество вершин");

    directed = static_cast<bool>(dirFlag);
    weighted = false;

    adj.clear();
    adj.resize(vertices);
    vertexNames.resize(vertices);

    unordered_map<string, int> tempNameToIndex;
    vector<string> tempNames;
    
    string from,to;
    double weight;
    while(file >> from >> to)
    {
        if (file.peek() == '\n')
        {
            weight = 1;
            weighted = false;
        }
        else
        {
            file >> weight;
            weighted = true;
        }
        if (!tempNameToIndex.count(from))
        {
            if((int)tempNames.size() >= vertices)
                throw runtime_error("Количество уникальных вершин превышает указанное в заголовке");
            tempNameToIndex[from] = tempNames.size();
            tempNames.push_back(from);
        }

        if (!tempNameToIndex.count(to))
        {
            if((int)tempNames.size() >= vertices)
                throw runtime_error("Количество уникальных вершин превышает указанное в заголовке");
            tempNameToIndex[to] = tempNames.size();
            tempNames.push_back(to);
        }

        int u = tempNameToIndex[from];
        int v = tempNameToIndex[to];

        adj[u].push_back({v, weight});
        if (!directed && u != v)
        {
            adj[v].push_back({u, weight});
        }
    }

    vertexNames = tempNames;
    nameToIndex = tempNameToIndex;

    int autoIndex = 0;
    while ((int)vertexNames.size() < vertices)
    {
        string name;
        do {
            name = "v" + to_string(autoIndex++);
        } while (nameToIndex.count(name));
            
        vertexNames.push_back(name);
        nameToIndex[name] = vertexNames.size() - 1;
    }
}

// Конструктор копирования
Graph::Graph(const Graph& other)
{
    adj = other.adj;
    directed = other.directed;
    vertexNames = other.vertexNames;
    nameToIndex= other.nameToIndex;
}

// Проверка вершины
void Graph::checkVertex(int v) const
{
    if (v < 0 || v >= (int)adj.size())
        throw out_of_range("Некорректный индекс вершины");
}

// Добавление вершины
void Graph::addVertex(const string& name)
{
    if (nameToIndex.count(name))
    {
        throw runtime_error("Вершина с таким именем уже существует");
    }
    int index = adj.size();
    adj.push_back({});
    vertexNames.push_back(name);
    nameToIndex[name] = index;
}

// Удаление вершины
void Graph::removeVertex(const string& name)
{
    if (!nameToIndex.count(name))
    {
        throw runtime_error("Вершина не существует");
    }
    int index = nameToIndex[name];
    for (auto& vec : adj)
    {
        vec.erase(
            remove_if(vec.begin(), vec.end(), [index](const Edge& e)
            {
                return e.to == index;
            }),
            vec.end()
        );
    }

    adj.erase(adj.begin() + index);
    vertexNames.erase(vertexNames.begin() + index);
    nameToIndex.erase(name);

    for (auto& vec : adj)
    {
        for (auto& e : vec)
        {
            if (e.to > index) e.to--;
        }
    }

    for (int i=0; i < (int)vertexNames.size(); ++i)
    {
        nameToIndex[vertexNames[i]] = i;
    }
}

void Graph::addEdgeByIndex(int u, int v, double weight)
{
    adj[u].push_back({v, weight});
    if (!directed && u != v)
    {
        adj[v].push_back({u, weight});
    }
}

// Добавление ребра
void Graph::addEdge(const string& from, 
    const string& to, 
    double weight)
{
    if (!nameToIndex.count(from) || !nameToIndex.count(to))
    {
        throw runtime_error("Одна из вершин не существует");
    }
    int u = nameToIndex[from];
    int v = nameToIndex[to];
    
    for(const auto& e : adj[u])
    {
        if (e.to == v)
        {
            throw runtime_error("Ребро уже существует");
        }
    }
    addEdgeByIndex(u, v, weight);     
}

// Удаление ребра
void Graph::removeEdge(const string& from, const string& to)
{
    if (!nameToIndex.count(from) || !nameToIndex.count(to))
    {
        throw runtime_error("Одна из вершин не существует");
    }
    int u = nameToIndex[from];
    int v = nameToIndex[to];

    auto& vec = adj[u];
    auto oldSize = vec.size();
    vec.erase(
        remove_if(
            vec.begin(),
            vec.end(),
            [v](const Edge& e){return e.to==v;}
        ),
        vec.end()
    );

    if (vec.size() == oldSize)
    {
        throw runtime_error("Ребро не существует");
    }
    if (!directed)
    {
        auto& vec2 = adj[v];
        vec2.erase(
            remove_if(
                vec2.begin(),
                vec2.end(),
                [u](const Edge& e){return e.to==u;}
            ),
            vec2.end()
        );
    }
}


// Получение списка ребер
vector<tuple<string, string, double>> Graph::getEdgeList() const
{
    vector<tuple<string, string, double>> edges;
    for(int i=0;i<(int)adj.size();++i)
    {
        for(const auto& e : adj[i])
        {
            if(directed || i <= e.to)
            {
                edges.push_back({
                    vertexNames[i],
                    vertexNames[e.to],
                    e.weight});
            }
                
        }
    }
    return edges;
}

// Ориентир \ неор
bool Graph::isDirected() const
{
    return directed;
}

bool Graph::isWeighted() const
{
    return weighted;
}

// Сохранение в файл
void Graph::saveToFile(const string& filename) const
{
    ofstream file(filename);
    if (!file)
        throw runtime_error("Ошибка записи файла");
    file << adj.size() << " " << directed << endl;

    for(const auto& name : vertexNames)
    {
        file << name << "\n";
    }

    for (int i=0; i<(int)adj.size(); ++i)
    {
        for (const auto& e :adj[i])
        {
            if (directed || i <= e.to)
            {
                file << vertexNames[i] << " " << vertexNames[e.to];
                if (weighted)
                {
                    file << " " << e.weight;
                }
                file << "\n";
            }
        }
    }
}

// Вывод списка смежности
void Graph::print() const
{
    for (int i=0;i<adj.size();i++)
    {
        cout << vertexNames[i] << ": ";
        for (const auto& e : adj[i])
        {
            cout << "(" << vertexNames[e.to];
            if (weighted)
            {
                cout << ", w=" << e.weight;
            }
            cout << ") ";
        }
        cout << "\n";
            
    }
}

//2  Вывести все вершины орграфа, смежные с данной.
void Graph::printAdjacent(const string& name) const
{
    if (!nameToIndex.count(name))
    {
        cout << "Вершина не найдена\n";
        return;
    }

    int v = nameToIndex.at(name);
    cout << "Смежные с " << name << ": ";
    //исходящие
    for (const auto& edge : adj[v])
    {
        cout << vertexNames[edge.to] << " ";
    }
    //входящие
    for (int u = 0; u < adj.size(); u++)
    {
        for (const auto& edge : adj[u])
        {
            if (edge.to == v && u != v)
            {
                cout << vertexNames[u] << " ";
            }
        }
    }
    cout << endl;
}

//3 Вывести все висячие вершины графа (степени 1).
//рассчет входящих степеней
vector<int> Graph::computeInDegrees() const
{
    vector<int> inDeg(adj.size(), 0);
    for (int u=0; u < adj.size(); u++)
    {
        for (const auto& edge : adj[u])
        {
            int v = edge.to;
            inDeg[v]++;
        }
    }
    return inDeg;
}

void Graph::printPedantVertices() const
{
    vector<int> inDeg = computeInDegrees();

    cout << "Висячие вершины: \n";
    for (int i=0; i < adj.size(); i++)
    {
        bool deg = false;
        int outDeg = adj[i].size();
        if (directed) 
        {
            if (inDeg[i] == 1 && outDeg == 0) 
            {
                deg = true;
            }
        }
        else
        {
            if (outDeg == 1) 
            {
                deg = true;
            }
        }
        if (deg) 
        {
            cout << vertexNames[i] << endl;
        }    
    }
}

//dfs
bool Graph::dfsCycle(int v, int parent, vector<int>& color) const
{
    color[v] = 1;
    for (const auto& edge : adj[v])
    {
        int to = edge.to;
        if (!directed){
            if (to == parent) continue;
            if (color[to] == 1) return true;
        } 
        else 
        {
            if (color[to] == 1) return true;
        }
        if (color[to] == 0 && dfsCycle(to, v, color)) return true;
    }
    color[v] = 2;
    return false;
}

bool Graph::isAcyclic() const
{
    vector<int> color(adj.size(), 0);
    for (int i=0; i < adj.size(); i++)
    {
        if (color[i] == 0){
            if (dfsCycle(i, -1, color)) return false;
        }
    }
    return true;
}

//bfs
void Graph::shortestPath(const string& from, const string& to) const
{
    if (!nameToIndex.count(from) || !nameToIndex.count(to))
    {
        cout << "Одна из вершин не найдена\n";
        return;
    }
    int start = nameToIndex.at(from);
    int finish = nameToIndex.at(to);

    vector<int> parent(adj.size(), -1);
    vector<bool> visited(adj.size(), false);

    queue<int>q;
    q.push(start);
    visited[start] = true;

    while (!q.empty())
    {
        int v = q.front();
        q.pop();

        for (const auto& edge : adj[v])
        {
            int u = edge.to;
            if (!visited[u])
            {
                visited[u] = true;
                parent[u] = v;
                q.push(u);
            }
        }
    }
    if (!visited[finish])
    {
        cout << "Путь не существует\n";
        return;
    }
    //восстановление пути
    vector<int> path;
    for (int v= finish; v != -1; v = parent[v])
    {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());

    cout << "Кратчайший путь:\n";
    for (int v : path)
    {
        cout << vertexNames[v] << " ";
    }
    cout << endl;
}

// каркас мин веса
void Graph::kruskalMST() const
{
    int edgesUsed = 0;
    auto edges = getEdgeList();

    //сорт по весу
    sort(edges.begin(), edges.end(), [](const auto& a, const auto& b)
    {
        return get<2>(a) < get<2>(b);
    });
    DSU dsu(adj.size());
    double totalWeight = 0;
    cout << "Ребра минимального остовного дерева:\n";
    for (const auto& [from, to, w] : edges)
    {
        int u = nameToIndex.at(from);
        int v = nameToIndex.at(to);
        if (dsu.find(u) != dsu.find(v))
        {
            dsu.unite(u, v);
            cout << from << " - " << to << " (w=" << w << ")\n";
            totalWeight += w;
            edgesUsed++;
        }
    }
    if (edgesUsed == (int)adj.size() - 1)
    {
        cout << "Получилось дерево\n";
    }
    else
    {
        cout << "Получился лес\n";
    }
    cout << "Общий вес: " << totalWeight << endl;
}

void Graph::findCenterAndRadius() const
{
    const double INF = 1e18;
    int n = adj.size();
    vector<vector<double>> dist(n, vector<double>(n, INF));
    for (int i = 0; i < n; ++i)
    {
        dist[i][i] = 0;
        for (const auto& e : adj[i])
        {
            dist[i][e.to] = min(dist[i][e.to], e.weight);
        }
    }

    for (int k = 0; k < n; ++k)
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                if (dist[i][k] < INF && dist[k][j] < INF)
                {
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                }
        
    vector<double> ecc(n, 0);
    for (int i = 0; i < n; ++i)
    {
        double maxDist = 0;
        for (int j=0; j < n; ++j)
        {
            if (dist[i][j] == INF)
            {
                maxDist = INF;
                break;
            }
            maxDist = max(maxDist, dist[i][j]);
        }
        ecc[i] = maxDist;
    }

    double radius = INF;
    for (double e : ecc)
    {
        radius = min(radius, e);
    }

    if (radius == INF)
    {
        cout << "Граф несвязный, центр не определен\n";
    }
    else
    {
        cout << "Радиус графа: " << radius << "\n";
        cout << "Центр графа: ";
        for (int i=0; i <n; ++i)
        {
            if (ecc[i] == radius)
            {
                cout << vertexNames[i] << " ";
            }
        }
        cout << "\n";
    }  
}

void Graph::shortestPathsFrom(const string& startName,
                            const string& v1Name,
                            const string& v2Name) const
{
    const double INF = numeric_limits<double>::infinity();

    if (!nameToIndex.count(startName) ||
        !nameToIndex.count(v1Name) ||
        !nameToIndex.count(v2Name))
    {
        throw runtime_error("Одна из вершин не существует");
    }

    int start = nameToIndex.at(startName);
    int v1 = nameToIndex.at(v1Name);
    int v2 = nameToIndex.at(v2Name);

    int n = adj.size();

    vector<double> dist(n, INF);
    vector<int> parent(n, -1);

    dist[start] = 0;

    auto edges = getEdgeList();

    for (int i = 0; i < n - 1; ++i)
    {
        for (const auto& [fromName, toName, w] : edges)
        {
            int u = nameToIndex.at(fromName);
            int v = nameToIndex.at(toName);

            if (dist[u] < INF && dist[u] + w < dist[v])
            {
                dist[v] = dist[u] + w;
                parent[v] = u;
            }
        }
    }

    // for (const auto& [fromName, toName, w] : edges)
    // {
    //     int u = nameToIndex.at(fromName);
    //     int v = nameToIndex.at(toName);

    //     if (dist[u] < INF && dist[u] + w < dist[v])
    //     {
    //         cout << "Обнаружен цикл отрицательного веса\n";
    //         return;
    //     }
    // }


    auto printPath = [&](int target)
    {
        if (dist[target] == INF)
        {
            cout << "Нет пути до " << vertexNames[target] << "\n";
            return;
        }

        vector<int> path;
        for (int v = target; v != -1; v = parent[v])
            path.push_back(v);

        reverse(path.begin(), path.end());

        cout << "Путь до " << vertexNames[target] << ": ";
        for (int i = 0; i < (int)path.size(); ++i)
        {
            cout << vertexNames[path[i]];
            if (i + 1 < (int)path.size()) cout << " -> ";
        }

        cout << " (длина = " << dist[target] << ")\n";
    };

    printPath(v1);
    printPath(v2);
}

bool Graph::hasNegativeCycle(int u, int v, const vector<vector<double>>& dist) const
{
    const double INF = numeric_limits<double>::infinity();
    int n = dist.size();
    for (int k=0; k < n; ++k)
    {
        if (dist[k][k] < 0 &&
            dist[u][k] < INF &&
            dist[k][v] < INF)
        {
            return true;
        }
    }
    return false;
}

// флойд-уоршелл
void Graph::printPathsToVertex(const string& targetName) const
{
    if (!nameToIndex.count(targetName))
    {
        cout << "Вершина не существует";
        return;
    }
    int target = nameToIndex.at(targetName);

    const double INF = numeric_limits<double>::infinity();
    int n = adj.size();

    vector<vector<double>> dist(n, vector<double>(n, INF));
    vector<vector<int>> next(n, vector<int>(n, -1));

    for (int i=0; i < n; ++i)
    {
        dist[i][i] = 0;
        next[i][i] = i;
    }

    for (int i=0; i< n; ++i)
    {
        for (const auto& e : adj[i])
        {
            dist[i][e.to] = e.weight;
            next[i][e.to] = e.to;

            if (!directed)
            {
                dist[e.to][i] = e.weight;
                next[e.to][i] = i;
            }
        }
    }

    for (int k=0; k < n; ++k)
    {
        for (int i=0; i < n; ++i)
        {
            for (int j =0; j < n; ++j)
            {
                if (dist[i][k] < INF && dist[k][j] < INF)
                {
                    if (dist[i][j] > dist[i][k] + dist[k][j])
                    {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        next[i][j] = next[i][k];
                    }
                }
            }
        }
    }

    auto buildPath = [&](int start, int end)
    {
        vector <int> path;
        if (next[start][end] == -1)
        {
            return path;
        }
        int cur = start;
        while (cur != end)
        {
            path.push_back(cur);
            cur = next[cur][end];
        }
        path.push_back(end);
        return path;
    };
    cout << "Кратчайшие пути до вершины " << targetName << ":\n";

    for (int i=0; i < n; ++i)
    {
        if (i == target) continue;

        cout << vertexNames[i] << " -> " << targetName << ": ";
        if (hasNegativeCycle(i, target, dist))
        {
            cout << "путь проходит через отрицательный цикл\n";
            continue;
        }

        if (dist[i][target] == INF)
        {
            cout << "нет пути\n";
            continue;
        }

        auto path = buildPath(i, target);

        for (int j=0; j < path.size(); ++j)
        {
            cout << vertexNames[path[j]];
            if (j+1 < path.size()) cout << " -> ";
        }
        cout << " (длина = " << dist[i][target] << ")\n";
        
    }
}

void Graph::maxFlow(const string& sourceName, const string& sinkName) const
{
    if (!nameToIndex.count(sourceName) || !nameToIndex.count(sinkName))
    {
        cout << "Одной или обеих вершин не существует";
        return;
    }

    int n = adj.size();
    int s = nameToIndex.at(sourceName);
    int t = nameToIndex.at(sinkName);

    struct EdgeF
    {
        int to;
        double cap;
        int rev;
    };

    vector<vector<EdgeF>> g(n);

    auto addEdge = [&](int u, int v, double cap)
    {
        g[u].push_back({v, cap, (int)g[v].size()});
        g[v].push_back({v, 0, (int)g[v].size() - 1});
    };

    // строим редуц граф
    for (int u = 0; u < n; ++u)
    {
        for (const auto& e : adj[u])
        {
            addEdge(u, e.to, e.weight);
        }
    }

    vector<int> level(n), ptr(n);

    auto bfs = [&]() -> bool
    {
        fill(level.begin(), level.end(), -1);
        queue<int> q;

        q.push(s);
        level[s] = 0;

        while (!q.empty())
        {
            int v = q.front();
            q.pop();

            for (auto &e : g[v])
            {
                if (e.cap > 0 && level[e.to] == -1)
                {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }

        return level[t] != -1;
    };

    function<double(int, double)> dfs = [&](int v, double pushed)
    {
        if (pushed == 0) return 0.0;
        if (v == t) return pushed;

        for (int &i = ptr[v]; i < (int)g[v].size(); ++i)
        {
            auto &e = g[v][i];
            if (e.cap > 0 && level[e.to] == level[v] + 1)
            {
                double tr = dfs(e.to, min(pushed, e.cap));

                if (tr == 0) continue;

                e.cap -= tr;
                g[e.to][e.rev].cap += tr;

                return tr;
            }
        }
        return 0.0;
    };
    double flow = 0;

    while (bfs())
    {
        fill(ptr.begin(), ptr.end(), 0);

        while (double pushed = dfs(s, numeric_limits<double>::infinity()))
        {
            flow += pushed;
        }
    }
    cout << "Максимальный поток из " << sourceName << " в " << sinkName 
        << " = " << flow << "\n";
}
