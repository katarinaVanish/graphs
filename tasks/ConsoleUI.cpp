#include <iostream>
#include "Graph.hpp"

using namespace std;

class ConsoleUI
{
private:
    Graph graph{true, true};
    bool initialized = false;

    void initialize()
    {
        while (!initialized)
        {
            cout << "1. Создать ориентированный взвешенный граф\n";
            cout << "2. Создать ориентированный невзвешенный граф\n";
            cout << "3. Создать неориентированный взвешенный граф\n";
            cout << "4. Создать неориентированный невзвешенный граф\n";
            cout << "5. Загрузить из файла\n";
            cout << "0. Выход\n";
            cout << "Выбор: ";

            int choice;
            cin >> choice;

            try
            {
                switch (choice)
                {
                    case 1:
                        graph = Graph(true, true);
                        initialized = true;
                        break;

                    case 2:
                        graph = Graph(true, false);
                        initialized = true;
                        break;
                    
                    case 3:
                        graph = Graph(false, true);
                        initialized = true;
                        break;

                    case 4:
                        graph = Graph(false, false);
                        initialized = true;
                        break;

                    case 5:
                    {
                        string file;
                        cout << "Имя файла: ";
                        cin >> file;
                        graph = Graph(file);
                        initialized = true;
                        break;
                    }

                    case 0:
                        exit(0);

                    default:
                        cout << "Некорректный выбор\n";
                }
            }
            catch (exception& e)
            {
                cout << "Ошибка: " << e.what() << "\n";
            }
        }
    }

public:

    void run()
    {
        initialize();
        
        while (true)
        {
            cout << endl;
            cout << "1. Добавить вершину\n";
            cout << "2. Добавить ребро\n";
            cout << "3. Удалить вершину\n";
            cout << "4. Удалить ребро\n";
            cout << "5. Показать список смежности\n";
            cout << "6. Сохранить в файл\n";
            cout << "7. Загрузить из файла\n";
            cout << "8. Пересоздать граф\n";
            cout << "9. Показать смежные вершины\n";
            cout << "10. Найти висячие вершины\n";
            cout << "11. Проверить граф на ацикличность\n";
            cout << "12. Найти кратчайший (по числу дуг) путь\n";
            cout << "13. Найти центр графа\n";
            cout << "14. Вывести длины кратчайших путей от u до v1 и v2\n";
            if (!graph.isDirected() && graph.isWeighted()){
                cout << "15. Найти каркас минимального веса\n";
            }

            cout << "0. Выход\n";

            // для вывода варианта с постороением каркаса - проверка на неориент и взвеш
            cout << "Выбор: ";
            int choice;
            cin >> choice;

            try
            {
                switch(choice)
                {
                    case 1:
                    {
                        string name;
                        cout << "Введите название вершины: ";
                        cin >> name;
                        graph.addVertex(name);
                        cout << "Вершина добавлена\n";
                        break;

                    }
                    case 2:
                    {
                        string from,to;
                        cout << "from to: ";
                        cin >> from >> to;

                        if (graph.isWeighted())
                        {
                            double weight;
                            cout << "weight: ";
                            cin >> weight;
                            graph.addEdge(from, to, weight);
                        } else 
                        {
                            graph.addEdge(from, to, 1.0);
                        }
                        cout << "Ребро добавлено\n";
                        
                        break;
                    }

                    case 3:
                    {
                        string name;
                        cout << "vertex: ";
                        cin >> name;
                        graph.removeVertex(name);
                        cout << "Вершина удалена\n";
                        break;
                    }

                    case 4:
                    {
                        string from,to;
                        cout << "from to: ";
                        cin >> from >> to;
                        graph.removeEdge(from,to);
                        cout << "Ребро удалено\n";
                        break;
                    }

                    case 5:
                    {
                        graph.print();
                        break;
                    }
                        
                    case 6:
                    {
                        string file;
                        cout << "filename: ";
                        cin >> file;
                        graph.saveToFile(file);
                        cout << "Сохранено\n";
                        break;
                    }

                    case 7:
                    {
                        string file;
                        cout << "filename: ";
                        cin >> file;
                        graph = Graph(file);
                        cout << "Граф загружен\n";
                        break;
                    }

                    case 8:
                    {
                        initialized = false;
                        initialize();
                        break;
                    }

                    case 9:
                    {
                        string name;
                        cout << "Введите вершину: ";
                        cin >> name;
                        graph.printAdjacent(name);
                        break;
                    }

                    case 10:
                    {
                        graph.printPedantVertices();
                        break;
                    }

                    case 11:
                    {
                        if (graph.isAcyclic()) cout << "Граф ацикличен\n";
                        else cout << "В графе есть цикл\n";
                        break;
                    }

                    case 12:
                    {
                        string from, to;
                        cout << "from to ";
                        cin >> from >> to;
                        graph.shortestPath(from, to);
                        break;
                    }

                    case 13:
                    {
                        graph.findCenterAndRadius();
                        break;
                    }
                    case 14:
                    {
                        string u, v1, v2;
                        cout << "u: ";
                        cin >> u;
                        cout << " v1: ";
                        cin >> v1;
                        cout << " v2: ";
                        cin >> v2;
                        graph.shortestPathsFrom(u, v1, v2);
                        break;
                    }
                    case 15:
                    {
                        if (graph.isDirected() && !graph.isWeighted()){
                            "Некорректный выбор";
                            break;
                        }
                        graph.kruskalMST();
                        break;

                    }
                    

                    case 0:
                        return;
                    default:
                        cout << "Некорректный выбор\n";
                }
            }
            catch(exception& e)
            {
                cout << "Ошибка: " << e.what() << endl;
            }
        }
    }
};
