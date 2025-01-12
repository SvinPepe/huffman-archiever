#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;

class Archiever {
    const short DEFAULT_VALUE = -1;
    const int CHUNK_SIZE = 8;
    FILE *input = nullptr, *output = nullptr;

    struct Tree {
        ull weight;
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

    priority_queue<Tree, vector<Tree>, Tree> forest;
    vector<Vertex> tree;
    vector<vector<unsigned char> > codes;
    vector<unsigned char> input_chars;
    ull freq[256]{};

    int update_tree_position(unsigned char bit, unsigned long long pos) const {
        if (bit) {
            return tree[pos].right;
        }
        return tree[pos].left;
    }

    void write_freq() {
        unsigned char flag = 0;
        short cnt = 0;

        for (unsigned int i: freq) {
            if (i > UINT16_MAX and flag < 1) {
                flag = 1;
            }
            if (i > UINT32_MAX and flag < 2) {
                flag = 2;
            }
            if (i) {
                cnt++;
            }
        }

        fwrite(&flag, sizeof flag, 1, output);
        fwrite(&cnt, sizeof cnt, 1, output);

        switch (flag) {
            case 0:
                for (int i = 0; i < 256; i++) {
                    if (freq[i]) {
                        unsigned char symbol = i;
                        fwrite(&symbol, sizeof symbol, 1, output);
                        unsigned short tmp = freq[i];
                        fwrite(&tmp, sizeof tmp, 1, output);
                    }
                }
                break;
            case 1:
                for (int i = 0; i < 256; i++) {
                    if (freq[i]) {
                        unsigned char symbol = i;
                        fwrite(&symbol, sizeof symbol, 1, output);
                        unsigned int tmp = freq[i];
                        fwrite(&tmp, sizeof tmp, 1, output);
                    }
                }
                break;
            default:
                for (int i = 0; i < 256; i++) {
                    if (freq[i]) {
                        unsigned char symbol = i;
                        fwrite(&symbol, sizeof symbol, 1, output);
                        fwrite(&freq[i], sizeof freq[i], 1, output);
                    }
                }
        }
    }

    void init_freq(bool isCompressing) {
        if (isCompressing) {
            unsigned char ch;
            fill_n(freq, 256, 0);
            while (fread(&ch, sizeof ch, 1, input)) {
                freq[ch]++;
                input_chars.push_back(ch);
            }
        } else {
            unsigned char flag;
            short cnt;
            fread(&flag, sizeof flag, 1, input);
            fread(&cnt, sizeof cnt, 1, input);
            fill_n(freq, 256, 0);

            switch (flag) {
                case 0:
                    for (int i = 0; i < cnt; i++) {
                        unsigned char symbol;
                        unsigned short freq_buf;
                        fread(&symbol, sizeof symbol, 1, input);
                        fread(&freq_buf, sizeof freq_buf, 1, input);
                        freq[symbol] = freq_buf;
                    }
                    break;
                case 1:
                    for (int i = 0; i < cnt; i++) {
                        unsigned char symbol;
                        unsigned int freq_buf;
                        fread(&symbol, sizeof symbol, 1, input);
                        fread(&freq_buf, sizeof freq_buf, 1, input);
                        freq[symbol] = freq_buf;
                    }
                    break;
                default:
                    for (int i = 0; i < cnt; i++) {
                        unsigned char symbol;
                        ull freq_buf;
                        fread(&symbol, sizeof symbol, 1, input);
                        fread(&freq_buf, sizeof freq_buf, 1, input);
                        freq[symbol] = freq_buf;
                    }
            }
        }
    }

    void build_tree() {
        while (forest.size() > 1) {
            Tree main_min = forest.top();
            forest.pop();
            Tree second_min = forest.top();
            forest.pop();
            tree.push_back({main_min.root, second_min.root, DEFAULT_VALUE});
            tree[main_min.root].parent = tree.size() - 1;
            tree[second_min.root].parent = tree.size() - 1;
            forest.push(Tree{main_min.weight + second_min.weight, static_cast<short>(tree.size() - 1)});
        }
    }

    void init_tree() {
        for (auto i: freq) {
            forest.push(Tree{i, static_cast<short>(forest.size())});
            tree.push_back({DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE});
        }
        build_tree();
    }

    void dfs(int vertex, vector<unsigned char> &calmer) {
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

    void init_codes() {
        codes.resize(tree.size(), {});
        vector<unsigned char> calmer = {};
        dfs(tree.size() - 1, calmer);
    }

    void init(bool isCompressing) {
        init_freq(isCompressing);
        init_tree();
        init_codes();
        if (isCompressing) {
            write_freq();
        }
    }

    void print_tree() {
        cout << "FOREST:\n";
        for (auto forestToPrint(forest); !forestToPrint.empty(); forestToPrint.pop()) {
            cout << forestToPrint.top().weight << " " << forestToPrint.top().root << '\n';
        }
        cout << "TREE:\n";
        for (auto i: tree) {
            cout << i.left << " " << i.right << " " << i.parent << '\n';
        }
    }

public:
    void compress_file(const char *input_filename = "input.txt", const char *output_filename = "output.txt") {
        input = fopen(input_filename, "rb");
        output = fopen(output_filename, "wb");

        tree.clear();
        codes.clear();
        while (!forest.empty()) forest.pop();

        init(true);
        vector<unsigned char> output_chars;
        unsigned char buf = 0;
        unsigned char buf_size = 0;

        for (auto symbol: input_chars) {
            for (auto bit: codes[symbol]) {
                buf_size++;
                buf <<= 1;
                buf |= bit;
                if (buf_size == CHUNK_SIZE) {
                    output_chars.push_back(buf);
                    buf = 0;
                    buf_size = 0;
                }
            }
        }

        fwrite(&buf_size, sizeof buf_size, 1, output);
        fwrite(&output_chars[0], sizeof output_chars[0], output_chars.size(), output);
        fwrite(&buf, sizeof buf, 1, output);

        fclose(input);
        fclose(output);
    }

    void decompress_file(const char *input_filename = "input.txt", const char *output_filename = "output.txt") {
        input = fopen(input_filename, "rb");
        output = fopen(output_filename, "wb");

        tree.clear();
        codes.clear();
        while (!forest.empty()) forest.pop();

        init(false);
        unsigned char last_byte_size;
        fread(&last_byte_size, sizeof last_byte_size, 1, input);

        auto pos = tree.size() - 1;
        unsigned char buf_ch;
        fread(&buf_ch, sizeof buf_ch, 1, input);
        for (;;) {
            const auto ch = buf_ch;
            if (const auto state = fread(&buf_ch, sizeof buf_ch, 1, input); !state) {
                for (int i = last_byte_size - 1; i >= 0; i--) {
                    pos = update_tree_position((ch >> i) & 1, pos);
                    if (pos < (tree.size() + 1) / 2) {
                        unsigned char symbol = pos;
                        fwrite(&symbol, sizeof symbol, 1, output);
                        pos = tree.size() - 1;
                    }
                }
                break;
            }
            for (int i = CHUNK_SIZE - 1; i >= 0; i--) {
                pos = update_tree_position((ch >> i) & 1, pos);
                if (pos < (tree.size() + 1) / 2) {
                    unsigned char symbol = pos;
                    fwrite(&symbol, sizeof symbol, 1, output);
                    pos = tree.size() - 1;
                }
            }
        }
        fclose(input);
        fclose(output);
    }
};

// int main() {
//     Archiever archiever;
//
//     archiever.compress_file("input.txt", "output.txt");
//     archiever.decompress_file("output.txt", "decompressed.txt");
//     return 0;
// }

int32_t main(int argc, char *argv[]) {
    Archiever archiever;
    if (!strcmp("encode", argv[1])) {
        archiever.compress_file(argv[3], argv[2]);
    } else {
        archiever.decompress_file(argv[2], argv[3]);
    }
    return 0;
}

//https://sch9.ru/gate/benchmarker/results_v2/03d4bc8a-1f17-441a-9669-aa975987913c.txt
