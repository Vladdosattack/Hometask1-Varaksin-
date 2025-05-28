#include <bits/stdc++.h>
#include <vector>

using namespace std;

struct Product {
    double weight;
    double value;
    int index;
    
    Product() : weight(0.0), value(0.0), index(0) {}
};

bool compare_by_ratio(Product a, Product b) { 
    return (a.value / a.weight) > (b.value / b.weight);
}

bool compare_by_value(Product a, Product b) { 
    return a.value < b.value;
}

int main() {
    // string filename;
    // cin >> filename;
    // ifstream input_file;
    // input_file.open(filename);
    
    (Михаил Владимирович, строчки выше можно для ручного ввода файла использовать)
    ifstream input_file("ks_100_0.txt");
    
    int item_count, max_weight;
    input_file >> item_count >> max_weight;
    cout << "Maximum Weight: " << max_weight << endl;
    
    vector<Product> items;
    vector<Product> sorted_by_value;
    double value, weight;
    
    for (int i = 0; i < item_count; i++) {
        input_file >> value >> weight;
        items.emplace_back();
        sorted_by_value.emplace_back();
        items[i].value = value;
        items[i].weight = weight;
        items[i].index = i;
        sorted_by_value[i].value = value;
        sorted_by_value[i].index = i;
        sorted_by_value[i].weight = weight;
    }
    
    sort(items.begin(), items.end(), compare_by_ratio);
    sort(sorted_by_value.begin(), sorted_by_value.end(), compare_by_value);
    
    vector<int> selected_by_ratio;
    vector<int> selected_by_value;
    double total_weight_ratio = 0;
    double total_weight_value = 0;
    int count_ratio = 0;
    int count_value = 0;
    
    for (int i = 0; i < item_count; i++) {
        if (total_weight_ratio + items[i].weight <= max_weight) {
            selected_by_ratio.push_back(items[i].index);
            total_weight_ratio += items[i].weight;
            count_ratio++;
        }
    }
    
    for (int i = 0; i < item_count; i++) {
        if (total_weight_value + sorted_by_value[i].weight <= max_weight) {
            selected_by_value.push_back(sorted_by_value[i].index);
            total_weight_value += sorted_by_value[i].weight;
            count_value++;
        }
    }
    
    cout << "Selection by Value-to-Weight Ratio" << endl;
    for (int i = 0; i < count_ratio; i++) {
        cout << "Item Number: " << selected_by_ratio[i] + 1 << " , Weight: " << items[selected_by_ratio[i]].weight << " , Value: " << items[selected_by_ratio[i]].value << endl;
    }
    cout << "Total Weight: " << total_weight_ratio << endl;
    
    cout << "Selection by Value" << endl;
    for (int i = 0; i < count_value; i++) {
        cout << "Item Number: " << selected_by_value[i] + 1 << " , Weight: " << sorted_by_value[selected_by_value[i]].weight << " , Value: " << sorted_by_value[selected_by_value[i]].value << endl;
    }
    cout << "Total Weight: " << total_weight_value << endl;
    
    return 0;
}
