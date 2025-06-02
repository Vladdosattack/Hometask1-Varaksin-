#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <algorithm>
#include <iomanip>
#include <future>
#include <functional>

using namespace std;


struct Item {
    double cost;
    double weight;
    double cost_efficiency;
    
    Item(double c, double w) : cost(c), weight(w), cost_efficiency(0.0) {}
};

class ThreadPool {
public:
    ThreadPool(size_t threads) : stop(false) {
        for(size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while(true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { 
                            return stop || !tasks.empty(); 
                        });
                        
                        if(stop && tasks.empty()) return;
                        
                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> future<typename result_of<F(Args...)>::type> {
        using return_type = typename result_of<F(Args...)>::type;
        
        auto task = make_shared<packaged_task<return_type()>>(
            bind(forward<F>(f), forward<Args>(args)...)
        );
        
        future<return_type> res = task->get_future();
        {
            unique_lock<mutex> lock(queue_mutex);
            if(stop) throw runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace([task](){ (*task)(); });
        }
        condition.notify_one();
        return res;
    }
    
    ~ThreadPool() {
        {
            unique_lock<mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for(thread &worker: workers)
            worker.join();
    }

private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex queue_mutex;
    condition_variable condition;
    bool stop;
};

void computeEfficiency(vector<Item>& items, size_t start, size_t end) {
    for(size_t i = start; i < end; ++i) {
        items[i].cost_efficiency = items[i].cost / items[i].weight;
    }
}

double solveKnapsackProblem(double capacity, vector<Item>& items) {
    const size_t num_threads = thread::hardware_concurrency();
    ThreadPool pool(num_threads);
    
    const size_t chunk_size = items.size() / num_threads;
    vector<future<void>> futures;
    
    for(size_t i = 0; i < num_threads; ++i) {
        size_t start = i * chunk_size;
        size_t end = (i == num_threads - 1) ? items.size() : start + chunk_size;
        futures.emplace_back(
            pool.enqueue(computeEfficiency, ref(items), start, end)
        );
    }
    
    for(auto &f : futures) f.wait();
    
    sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.cost_efficiency > b.cost_efficiency;
    });
    
    double total_value = 0.0;
    
    for(const auto& item : items) {
        if(capacity <= 0) break;
        
        if(item.weight <= capacity) {
            capacity -= item.weight;
            total_value += item.cost;
        } else {
            total_value += item.cost_efficiency * capacity;
            capacity = 0;
        }
    }
    
    return total_value;
}

int main() {
    // string filename;
    // cout << "Enter a filename: ";
    // cin >> filename;
    
    // ifstream input_file(filename);
    // if(!input_file.is_open()) {
    //     cerr << "Error. File is not found" << filename << endl;
    //     return 1;
    // }
    
    //(Михаил Владимирович, строчки выше можно для ручного ввода файла использовать)

    string filename = "ks_100_0.txt";
    ifstream input_file(filename);
    int item_count;
    double max_capacity;
    input_file >> item_count >> max_capacity;
    
    vector<Item> items;
    items.reserve(item_count);
    
    for(int i = 0; i < item_count; ++i) {
        double cost, weight;
        input_file >> cost >> weight;
        items.emplace_back(cost, weight);
    }
    
    double max_value = solveKnapsackProblem(max_capacity, items);
    
    cout << fixed << setprecision(2);
    cout << "Max_value in the Knapsack: " << max_value << endl;
    
    return 0;
}