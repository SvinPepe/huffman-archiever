#include <bits/stdc++.h>
#ifndef ARCHIEVER_H
#define ARCHIEVER_H


class Archiever {
    const short DEFAULT_VALUE = -1;
    const int CHUNK_SIZE = 8;
    FILE *input = nullptr, *output = nullptr;

    struct Tree {
        unsigned long long weight;
        short root;

        bool operator()(const Tree a, const Tree b) const {
            return a.weight > b.weight;
        }
    };

    struct Vertex {
        short left;
        short right;
        short parent;
    };

    std::priority_queue<Tree, std::vector<Tree>, Tree> forest;
    std::vector<Vertex> tree;
    std::vector<std::vector<unsigned char> > codes;
    std::vector<unsigned char> input_chars;
    unsigned long long freq[256]{};

    void write_freq();
    void init_freq(bool isCompressing);
    void build_tree();
    void init_tree();
    void dfs(int vertex, std::vector<unsigned char> &calmer);
    void init(bool isCompressing);

public:
    void compress_file(const char *input_filename = "input.txt", const char *output_filename = "output.txt");
    void decompress_file(const char *input_filename = "input.txt", const char *output_filename = "output.txt");
};


#endif //ARCHIEVER_H
