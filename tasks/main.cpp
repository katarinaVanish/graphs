#include "ConsoleUI.cpp"
#include <iostream>

int main()
{
    //1 - неориентир взвеш
    //2 - ориентир взвеш
    //3 - неор невзв
    //4 - ор невз
    ConsoleUI ui;
    ui.run();
    return 0;
}

//  g++ -std=c++17 tasks/main.cpp tasks/Graph.cpp tasks/ConsoleUI.cpp -o tasks/main.exe
//  ./tasks/main.exe
//  chcp 65001