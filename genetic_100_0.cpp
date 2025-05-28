#include <bits/stdc++.h>

using namespace std;

void initialize_parents(vector<int>& individual, int index) { 
    int length = individual.size();
    for (int i = 0; i < length; i++) {
        individual[i] = (i == index - 1) ? 1 : 0;
    }
}

int calculate_cost(const vector<int>& individual, const vector<int>& values) { 
    int total_cost = 0;
    int length = individual.size();
    for (int i = 0; i < length; i++) {
        if (individual[i] == 1) {
            total_cost += values[i];
        }
    }
    return total_cost;
}

int calculate_weight(const vector<int>& individual, const vector<int>& weights) { 
    int total_weight = 0;
    int length = individual.size();
    for (int i = 0; i < length; i++) {
        if (individual[i] == 1) {
            total_weight += weights[i];
        }
    }
    return total_weight;
}

bool is_valid_selection(const vector<int>& individual, int threshold, const vector<int>& values) { 
    return calculate_cost(individual, values) >= threshold;
}

vector<int> perform_two_point_crossover(const vector<int>& parent1, const vector<int>& parent2) { 
    int length = parent1.size();
    vector<int> offspring(length);
    int point1 = rand() % length; 
    int point2 = rand() % length; 
    if (point1 > point2) swap(point1, point2); 
    for (int i = 0; i < length; i++) {
        offspring[i] = (i < point1 || i >= point2) ? parent1[i] : parent2[i];
    }
    return offspring;
}

vector<vector<int>> create_new_population(const vector<vector<int>>& parents, const vector<vector<int>>& offspring, int target_size, const vector<int>& weights, int max_weight, const vector<int>& values) {
    vector<vector<int>> combined_population;
    combined_population.insert(combined_population.end(), parents.begin(), parents.end());
    combined_population.insert(combined_population.end(), offspring.begin(), offspring.end());
    
    vector<vector<int>> valid_population; 
    for (const auto& individual : combined_population) {
        if (calculate_weight(individual, weights) <= max_weight) {
            valid_population.push_back(individual);
        }
    }
    
    sort(valid_population.begin(), valid_population.end(), [&](const vector<int>& a, const vector<int>& b) { 
        return calculate_cost(a, values) > calculate_cost(b, values);
    });
    
    while (valid_population.size() < target_size) { 
        if (!offspring.empty()) { 
            int random_index = rand() % offspring.size(); 
            valid_population.push_back(offspring[random_index]); 
        } else if (!parents.empty()) { 
            int random_index = rand() % parents.size(); 
            valid_population.push_back(parents[random_index]); 
        }
    }
    
    if (valid_population.size() > target_size) { 
        valid_population.resize(target_size);
    }
    
    return valid_population;
}

vector<int> run_genetic_algorithm(const vector<int>& weights, const vector<int>& values, int max_weight, int population_size, int generations) {
    srand(time(0)); 
    int item_count = weights.size();
    vector<vector<int>> population(population_size, vector<int>(item_count, 0)); 
    for (int i = 0; i < population_size; i++) {
        initialize_parents(population[i], (i % item_count) + 1); 
    }
    
    for (int gen = 0; gen < generations; gen++) {
        vector<int> costs(population_size);
        vector<int> weights_pop(population_size);
        for (int i = 0; i < population_size; i++) {
            costs[i] = calculate_cost(population[i], values);
            weights_pop[i] = calculate_weight(population[i], weights);
        }
        
        int total_cost = accumulate(costs.begin(), costs.end(), 0);
        int average_cost = total_cost / population_size;
        
        vector<vector<int>> selected_parents;
        for (int i = 0; i < population_size; i++) {
            if (weights_pop[i] <= max_weight && is_valid_selection(population[i], average_cost, values)) {
                selected_parents.push_back(population[i]);
            }
        }
        
        while (selected_parents.size() < 2) {
            vector<int> random_individual(item_count, 0);
            int random_index = rand() % item_count;
            random_individual[random_index] = 1;
            selected_parents.push_back(random_individual);
        }
        
        vector<vector<int>> offspring;
        for (size_t i = 0; i < selected_parents.size() / 2; i++) {
            int index1 = rand() % selected_parents.size();
            int index2 = rand() % selected_parents.size();
            vector<int> parent1 = selected_parents[index1];
            vector<int> parent2 = selected_parents[index2];
            offspring.push_back(perform_two_point_crossover(parent1, parent2));
            offspring.push_back(perform_two_point_crossover(parent2, parent1));
        }
        
        population = create_new_population(selected_parents, offspring, population_size, weights, max_weight, values);
    }
    
    return population.empty() ? vector<int>(item_count, 0) : population[0];
}

int main() {
    ifstream input_file("ks_100_0.txt");
    int item_count, max_weight;
    input_file >> item_count >> max_weight;
    vector<int> weights(item_count);
    vector<int> values(item_count);
    for (int i = 0; i < item_count; i++) {
        input_file >> values[i] >> weights[i];
    }
    
    int population_size = item_count;
    int generations = 100;
    vector<int> best_solution = run_genetic_algorithm(weights, values, max_weight, population_size, generations);
    
    cout << "Optimal solution: ";
    for (int i = 0; i < best_solution.size(); i++) {
        cout << best_solution[i] << " ";
    }
    cout << endl;
    cout << "Total Cost: " << calculate_cost(best_solution, values) << endl;
    cout << "Total Weight: " << calculate_weight(best_solution, weights) << endl;
    
    return 0;
}
