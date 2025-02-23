#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

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
        for (size_t i = 1; i < path.size() - 2; ++i) {
            for (size_t j = i + 1; j < path.size(); ++j) {
                performTwoOptSwap(path, i, j);
                if (routeLength(path, vertices) < routeLength(path, vertices)) {
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
                    performThreeOptSwap(path, a, b, c);
                    if (routeLength(path, vertices) < routeLength(path, vertices)) {
                        improved = true;
                    } else {
                        performThreeOptSwap(path, a, b, c);
                    }
                }
            }
        }
    }
}

int main() {
    int n;
    cin >> n;
    vector<Vertex> points(n);
    for (int i = 0; i < n; ++i) {
        cin >> points[i].x >> points[i].y;
    }
    vector<int> tour(n);
    for (int i = 0; i < n; ++i) {
        tour[i] = i;
    }
    optimizeTwoOpt(tour, points);
    optimizeThreeOpt(tour, points);
    cout << routeLength(tour, points) << endl;
    for (int i : tour) {
        cout << i << " ";
    }
    cout << endl;
    return 0;
}