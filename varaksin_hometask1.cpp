#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ctime>

using namespace std;

struct Vertex {
    double x, y;
    Vertex(double x = 0.0, double y = 0.0) : x(x), y(y) {}
};

double euclideanDistance(const Vertex& a, const Vertex& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

double routeLength(const vector<int>& path, const vector<Vertex>& vertices) {
    double total = 0.0;
    for (size_t i = 1; i < path.size(); ++i) {
        total += euclideanDistance(vertices[path[i - 1]], vertices[path[i]]);
    }
    total += euclideanDistance(vertices[path.back()], vertices[path.front()]);
    return total;
}

void performTwoOptSwap(vector<int>& path, int start, int end) {
    while (start < end) {
        swap(path[start++], path[end--]);
    }
}

void optimizeTwoOpt(vector<int>& path, const vector<Vertex>& vertices) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 1; i < path.size() - 1; ++i) {
            for (size_t j = i + 1; j < path.size(); ++j) {
                performTwoOptSwap(path, i, j);
                double newLength = routeLength(path, vertices);
                if (newLength < routeLength(path, vertices)) {
                    improved = true;
                } else {
                    performTwoOptSwap(path, i, j);
                }
            }
        }
    }
}

void performThreeOptSwap(vector<int>& path, int a, int b, int c) {
    reverse(path.begin() + a, path.begin() + b + 1);
    reverse(path.begin() + b + 1, path.begin() + c + 1);
}

void optimizeThreeOpt(vector<int>& path, const vector<Vertex>& vertices) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (int a = 1; a < path.size() - 2; ++a) {
            for (int b = a + 1; b < path.size() - 1; ++b) {
                for (int c = b + 1; c < path.size(); ++c) {
                    vector<int> originalPath = path; // сохранить оригинальный путь
                    performThreeOptSwap(path, a, b, c);
                    double newLength = routeLength(path, vertices);
                    if (newLength < routeLength(originalPath, vertices)) { // используем оригинальный путь для сравнения
                        improved = true;
                    } else {
                        path = originalPath; // восстановить оригинальный путь
                    }
                }
            }
        }
    }
}

int main() {
    int n;
    cout << "Введите количество вершин: ";
    cin >> n;
    vector<Vertex> points(n);
    
    cout << "Введите координаты вершин (x, y):" << endl;
    for (int i = 0; i < n; ++i) {
        cin >> points[i].x >> points[i].y;
    }
    
    vector<int> tour(n);
    for (int i = 0; i < n; ++i) {
        tour[i] = i;
    }

    clock_t startTime, endTime;
    double timeTaken;

    // Оптимизация 2-opt
    startTime = clock();
    optimizeTwoOpt(tour, points);
    endTime = clock();
    timeTaken = double(endTime - startTime) / CLOCKS_PER_SEC;
    double length2Opt = routeLength(tour, points);

    // Сохранить порядок обхода после 2-opt
    vector<int> tourAfter2Opt = tour;

    // Оптимизация 3-opt
    startTime = clock();
    optimizeThreeOpt(tour, points);
    endTime = clock();
    double timeTaken3Opt = double(endTime - startTime) / CLOCKS_PER_SEC; // исправлено для многоразового использования времени
    double length3Opt = routeLength(tour, points);

    // Вывод результата
    cout << "-------------------------------------------------" << endl;
    cout << "| Количество Вершин | Время 2-opt | Время 3-opt |" << endl;
    cout << "-------------------------------------------------" << endl;
    cout << "|       " << n << "           |   " << timeTaken << "   |   " << timeTaken3Opt << "   |" << endl;
    cout << "-------------------------------------------------" << endl;
    
    cout << "Длина маршрута после 2-opt: " << length2Opt << endl;
    cout << "Длина маршрута после 3-opt: " << length3Opt << endl;

    cout << "Порядок обхода после 2-opt: ";
    for (int i : tourAfter2Opt) {
        cout << i << " ";
    }
    cout << endl;

    cout << "Порядок обхода после 3-opt: ";
    for (int i : tour) {
        cout << i << " ";
    }
    cout << endl;

    return 0;
}
