//
// Created by 2007n on 12.01.2025.
//

#include "Archiever.h"

void Archiever::write_freq() {
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

void Archiever::init_freq(bool isCompressing) {
    if (isCompressing) {
        unsigned char ch;
        std::fill_n(freq, 256, 0);
        while (fread(&ch, sizeof ch, 1, input)) {
            freq[ch]++;
            input_chars.push_back(ch);
        }
    } else {
        unsigned char flag;
        short cnt;
        fread(&flag, sizeof flag, 1, input);
        fread(&cnt, sizeof cnt, 1, input);
        std::fill_n(freq, 256, 0);

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
                    unsigned long long freq_buf;
                    fread(&symbol, sizeof symbol, 1, input);
                    fread(&freq_buf, sizeof freq_buf, 1, input);
                    freq[symbol] = freq_buf;
                }
        }
    }
}

void Archiever::build_tree() {
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

void Archiever::init_tree() {
    for (auto i: freq) {
        forest.push(Tree{i, static_cast<short>(forest.size())});
        tree.push_back({DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE});
    }
    build_tree();
}

void Archiever::dfs(int vertex, std::vector<unsigned char> &code_buffer) {
    code_buffer.push_back(false);
    if (tree[vertex].left != DEFAULT_VALUE) {
        dfs(tree[vertex].left, code_buffer);
    }
    code_buffer.pop_back();
    code_buffer.push_back(true);
    if (tree[vertex].right != DEFAULT_VALUE) {
        dfs(tree[vertex].right, code_buffer);
    }
    code_buffer.pop_back();
    codes[vertex] = code_buffer;
}


void Archiever::init(bool isCompressing) {
    init_freq(isCompressing);
    init_tree();
    codes.resize(tree.size(), {});
    std::vector<unsigned char> code_buffer = {};
    dfs(tree.size() - 1, code_buffer);
    if (isCompressing) {
        write_freq();
    }
}

void Archiever::compress_file(const char *input_filename, const char *output_filename) {
    input = fopen(input_filename, "rb");
    output = fopen(output_filename, "wb");

    tree.clear();
    codes.clear();
    while (!forest.empty()) forest.pop();

    init(true);
    std::vector<unsigned char> output_chars;
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

void Archiever::decompress_file(const char *input_filename, const char *output_filename) {
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
        if (const auto state = fread(&buf_ch, sizeof buf_ch, 1, input); !state) { // last byte
            for (int i = last_byte_size - 1; i >= 0; i--) {
                if (ch >> i & 1) pos = tree[pos].right;
                else pos = tree[pos].left;

                if (pos < (tree.size() + 1) / 2) {
                    unsigned char symbol = pos;
                    fwrite(&symbol, sizeof symbol, 1, output);
                    pos = tree.size() - 1;
                }
            }
            break;
        }
        for (int i = CHUNK_SIZE - 1; i >= 0; i--) {
            if (ch >> i & 1) pos = tree[pos].right;
            else pos = tree[pos].left;

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
