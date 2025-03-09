#include <iostream>
#include <vector>
#include <queue> 
#include <algorithm> 
#include <fstream> 

using namespace std;

struct Item
{
    int cost, mass; // Стоимость и масса элемента
    double ratio; // Соотношение цена/масса для упрощенной сортировки
};

class TreeNode
{
public:
    int index, totalCost, totalMass; // Индекс элемента, стоимость и масса текущего узла
    double maxProfit; // Максимальная прибыль, которую можно достичь из этого узла

    TreeNode(int idx, int cost, int mass, double profit)
    {
        index = idx;
        totalCost = cost;
        totalMass = mass;
        maxProfit = profit;
    }

    TreeNode() {}; 
};

// Функция для вычисления потенциальной прибыли от узла
double calculateProfit(int idx, int currentCost, int currentMass, int totalItems, int weightLimit, vector<Item> &elements)
{
    // Если текущая масса превышает предел, прибыль должна быть нулевая
    if (currentMass >= weightLimit) return 0.0;

    // Начинаем с прибыли от текущего узла
    double estimatedProfit = currentCost;

    // Переходим к следующему элементу
    int i = idx + 1;
    int totalMass = currentMass;

    // Пока суммарная масса не превышает лимит и есть еще элементы
    while (totalMass + elements[i].mass <= weightLimit && i < totalItems)
    {
        estimatedProfit += elements[i].cost;
        totalMass += elements[i].mass;
        i++;
    }

    // Если все еще есть элементы, которые не были добавлены, оценим прибыль с использованием эвристики
    if (i < totalItems)
    {
        double heuristicValue = elements[i].ratio;
        double remainingProfit = (weightLimit - totalMass) * heuristicValue;
        estimatedProfit += remainingProfit;
    }

    return estimatedProfit;
}

vector<int> branchAndBound(int totalItems, int weightLimit, vector<Item> &elements)
{
    // Лямбда-функция для сортировки элементов по соотношению цена/масса
    bool (*compareItems)(const Item &a, const Item &b) = [](const Item &a, const Item &b) { return a.ratio > b.ratio; };
    sort(elements.begin(), elements.end(), compareItems);

    // Функция для сравнения узлов по максимальной прибыли
    bool (*compareNodes)(const TreeNode &a, const TreeNode &b) = [](const TreeNode &a, const TreeNode &b) { return a.maxProfit < b.maxProfit; };
    priority_queue<TreeNode, vector<TreeNode>, decltype(compareNodes)> priorityQueue(compareNodes);

    // Начальные условия: ничего не добавлено
    int idx = -1, currentCost = 0, currentMass = 0;
    double maxProfitEstimate = calculateProfit(idx, currentCost, currentMass, totalItems, weightLimit, elements);
    TreeNode currentNode(idx, currentCost, currentMass, maxProfitEstimate);
    priorityQueue.push(currentNode);

    int highestProfit = 0;
    int totalMassUsed = 0;

    TreeNode nextNode;
    while (!priorityQueue.empty())
    {
        // Извлекаем узел с максимальной прибыльностью
        currentNode = priorityQueue.top();
        priorityQueue.pop();

        if (currentNode.index + 1 >= totalItems) break;

        // Обновляем узел, добавляя следующий элемент
        nextNode.index = currentNode.index + 1;
        nextNode.totalCost = currentNode.totalCost + elements[nextNode.index].cost;
        nextNode.totalMass = currentNode.totalMass + elements[nextNode.index].mass;

        // Проверяем, если новый узел допустим
        if (nextNode.totalCost > highestProfit && nextNode.totalMass <= weightLimit)
        {
            totalMassUsed = nextNode.totalMass;
            highestProfit = nextNode.totalCost;
        }

        // Оцениваем прибыль для нового узла
        nextNode.maxProfit = calculateProfit(nextNode.index, nextNode.totalCost, nextNode.totalMass, totalItems, weightLimit, elements);
        if (nextNode.maxProfit > highestProfit)
        {
            priorityQueue.push(nextNode);
        }

        // Убедимся, что узел без нового элемента тоже проверен
        nextNode.totalMass = currentNode.totalMass;
        nextNode.totalCost = currentNode.totalCost;
        nextNode.maxProfit = calculateProfit(nextNode.index, nextNode.totalCost, nextNode.totalMass, totalItems, weightLimit, elements);

        if (nextNode.maxProfit > highestProfit)
            priorityQueue.push(nextNode);
    }

    return {highestProfit, totalMassUsed};
}

int main()
{
    ifstream file("ks_10000_0");

    if (!file)
    {
        cerr << "Ошибка: Не удалось открыть файл" << endl;
        return -1;
    }

    int totalItems, weightLimit;
    file >> totalItems >> weightLimit;

    vector<Item> elements(totalItems);
    int itemIndex = 0, cost, mass;
    for (itemIndex; itemIndex < totalItems; ++itemIndex)
    {
        file >> cost >> mass;
        elements[itemIndex].cost = cost;
        elements[itemIndex].mass = mass;
        elements[itemIndex].ratio = (double)cost / mass;
    }

    file.close();

    vector<int> result = branchAndBound(totalItems, weightLimit, elements);

    int finalProfit = result[0];
    int usedWeight = result[1];
    cout << "Прибыль: " << finalProfit << endl;
    cout << "Общий вес: " << usedWeight << endl;

    return 0;
}