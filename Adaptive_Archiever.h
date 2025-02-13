//
// Created by 2007n on 12.01.2025.
//

#ifndef ADAPTIVE_ARCHIEVER_H
#define ADAPTIVE_ARCHIEVER_H
#include <bits/stdc++.h>
using namespace std;

class Adaptive_Archiever {
    struct Node {
        char symbol;       // Символ (если это листовой узел)
        int weight;        // Частота появления символа
        int index;         // Индекс узла в порядке добавления
        Node* left;        // Левый потомок
        Node* right;       // Правый потомок
        Node* parent;      // Родительский узел
        bool isNYT;        // Флаг узла NYT (Not Yet Transmitted)

        Node(char sym, int w, int idx, bool nyt = false)
            : symbol(sym), weight(w), index(idx), left(nullptr), right(nullptr), parent(nullptr), isNYT(nyt) {}
    };

    Node* root;  // Корень дерева
    Node* nyt;   // NYT-узел
    unordered_map<char, Node*> symbolTable; // Таблица символов
    vector<Node*> nodeList; // Линейный список узлов для отслеживания порядка
    string encodedData;
    int nodeCounter = 0;

    string getCode(Node* node) {
        string code = "";
        while (node->parent) {
            if (node->parent->left == node)
                code = "0" + code;
            else
                code = "1" + code;
            node = node->parent;
        }
        return code;
    }

    void encodeNYT(char c) {
        encodedData += getCode(nyt);
        encodedData += bitset<8>(c).to_string(); // Символ в двоичном виде
    }

    void encodeExistingSymbol(char c) {
        encodedData += getCode(symbolTable[c]);
    }

    void addNewSymbol(char c) {
        // Создаем новые узлы для символа и нового NYT
        Node* newSymbolNode = new Node(c, 1, nodeCounter++);
        Node* newNYT = new Node('\0', 0, nodeCounter++, true);

        // Обновляем текущее NYT-дерево
        nyt->isNYT = false;
        nyt->left = newNYT;
        nyt->right = newSymbolNode;
        newNYT->parent = nyt;
        newSymbolNode->parent = nyt;

        // Добавляем в таблицу символов
        symbolTable[c] = newSymbolNode;
        nyt = newNYT; // Обновляем указатель NYT

        // Добавляем узлы в список
        nodeList.push_back(newSymbolNode);
        nodeList.push_back(newNYT);

        // Обновляем веса дерева
        updateTree(newSymbolNode);
    }

    void updateTree(Node* node) {
        while (node) {
            node->weight++;
            balanceTree(node);
            node = node->parent;
        }
    }

    void balanceTree(Node* node) {
        if (!node->parent) return; // Если корень - ничего не делаем
        Node* largest = findLargestSameWeight(node);
        if (largest && largest != node && largest->parent != node) {
            swapNodes(node, largest);
        }
    }

    Node* findLargestSameWeight(Node* node) {
        Node* result = node;
        for (Node* n : nodeList) {
            if (n->weight == node->weight && n->index > result->index) {
                result = n;
            }
        }
        return result;
    }

    void swapNodes(Node* a, Node* b) {
        if (!a->parent || !b->parent) return;
        swap(a->parent, b->parent);
        swap(a->left, b->left);
        swap(a->right, b->right);
        swap(a->index, b->index);
        if (a->left) a->left->parent = a;
        if (a->right) a->right->parent = a;
        if (b->left) b->left->parent = b;
        if (b->right) b->right->parent = b;
    }

public:
    Adaptive_Archiever() {
        root = nyt = new Node('\0', 0, nodeCounter++, true); // Начальное дерево с одним NYT-узлом
        nodeList.push_back(root);
    }

    void insertSymbol(char c) {
        if (symbolTable.find(c) == symbolTable.end()) {
            cout << "Добавляем новый символ: " << c << endl;
            encodeNYT(c);
            addNewSymbol(c);
        } else {
            cout << "Обновляем существующий символ: " << c << endl;
            encodeExistingSymbol(c);
            updateTree(symbolTable[c]);
        }
    }

    string getEncodedData() const {
        return encodedData;
    }
};




#endif //ADAPTIVE_ARCHIEVER_H
