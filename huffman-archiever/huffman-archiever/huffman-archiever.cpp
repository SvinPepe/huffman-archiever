#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <bitset>
#include <queue>
using namespace std;



class Archiever {
private:
    const int DEFAULT_VALUE = -1;
    FILE* input, * output;

    struct Tree {
        uint64_t weight;
        int root;
        bool operator() (Tree a, Tree b)
        {
            return a.weight > b.weight;
        }
    };

    struct Vertex {
        int left;
        int right;
        int parent;
    };

    priority_queue<Tree, vector<Tree>, Tree> forest;
    vector<Vertex> tree;
    vector<vector<char>> codes;
    void init_tree() {
        unsigned char ch;
        uint64_t freq[256];
        for (int i = 0; i < 256; i++) {
            freq[i] = 0;
        }
        while (fscanf(input, "%c", &ch) != -1) {
            freq[ch]++;
        }
        for (int i = 0; i < 256; i++) {
            forest.push(Tree{ freq[i], (int)forest.size() });
            tree.push_back({ DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE });
        }
        fclose(input);
    }

    void build_tree() {
        while (forest.size() > 1) {
            Tree main_min = forest.top(); forest.pop();
            Tree second_min = forest.top(); forest.pop();

            tree.push_back({ main_min.root, second_min.root, DEFAULT_VALUE });
            tree[main_min.root].parent = tree.size() - 1;
            tree[second_min.root].parent = tree.size() - 1;

            forest.push(Tree{ main_min.weight + second_min.weight, int(tree.size() - 1) });
        }
    }

    void init_codes() {
        codes.resize(tree.size(), {});
        dfs(tree.size() - 1, {});
    }

    void dfs(int vertex, vector<char> calmer) {
        calmer.push_back(false);
        if (tree[vertex].left != DEFAULT_VALUE) {
            dfs(tree[vertex].left, calmer);
        }
        calmer.pop_back();

        calmer.push_back(true);
        if (tree[vertex].right != DEFAULT_VALUE) {
            dfs(tree[vertex].right, calmer);
        }
        calmer.pop_back();
        codes[vertex] = calmer;
    }

    void init() {
        init_tree();
        build_tree();
        init_codes();
    }


    

public:
    void print_tree() {
        cout << "FOREST:\n";
        auto forestToPrint(forest);
        for (; !forestToPrint.empty(); forestToPrint.pop()) {
            cout << forestToPrint.top().weight << " " << forestToPrint.top().root << '\n';
        }
        cout << "TREE:\n";
        for (auto i : tree) {
            cout << i.left << " " << i.right << " " << i.parent << '\n';
        }
    }

    void compress_file(const char* input_filename = "input.txt", const char* output_filename = "output.txt") {  
        char ch;
        input = fopen(input_filename, "rb");
        output = fopen(output_filename, "wb");

        init();

        for (auto i : tree) {
            fprintf(output, "%d %d %d ", i.left, i.right, i.parent);
        }
        
        while (fscanf(input, "%c", &ch) != -1) {
            for (auto bit : codes[ch]) {
                fprintf(output, "%d", bit);
            }
        }
    }
};

int main() {

    Archiever archiever;
    archiever.compress_file();
    

    return 0;
}