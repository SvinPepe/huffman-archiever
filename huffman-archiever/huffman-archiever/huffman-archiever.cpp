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
	const int CHUNK_SIZE = 8;
	const int TREE_SIZE = 511;
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
	vector<char> input_buf;
	void init_tree() {
		unsigned char ch;
		uint64_t freq[256];
		for (int i = 0; i < 256; i++) {
			freq[i] = 0;
		}
		while (fscanf(input, "%c", &ch) != -1) {
			freq[ch]++;
			input_buf.push_back(ch);
		}
		for (int i = 0; i < 256; i++) {
			forest.push(Tree{ freq[i], (int)forest.size() });
			tree.push_back({ DEFAULT_VALUE, DEFAULT_VALUE, DEFAULT_VALUE });
		}
	}

	void read_tree() {
		for (int i = 0; i < TREE_SIZE; i++) {
			int left, right, parent;
			/*fscanf(input, "%d", &left);
			fscanf(input, "%d", &right);
			fscanf(input, "%d", &parent);*/
			fscanf(input, "%d %d %d ", &left, &right, &parent);
			tree.push_back({ left, right, parent });
		}
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

	int update_tree_position(char bit, int pos) {
		if (bit) {
			return tree[pos].right;
		}
		else {
			return tree[pos].left;
		}
	}


	void init() {
		init_tree();
		build_tree();
		init_codes();
	}

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

public:
	

	void compress_file(const char* input_filename = "input.txt", const char* output_filename = "output.txt") {
		char ch;
		input = fopen(input_filename, "rb");
		output = fopen(output_filename, "wb");

		tree.clear();
		codes.clear();
		input_buf.clear();
		while (!forest.empty()) forest.pop();

		init();
		for (auto i : tree) {
			fprintf(output, "%d %d %d ", i.left, i.right, i.parent);
			/*fprintf(output, "%d", i.left);
			fprintf(output, "%d", i.right);
			fprintf(output, "%d", i.parent);*/
		}		
		
		vector<char> output_chars;
		char buf = 0;
		int buf_size = 0;

		for (auto i : input_buf) {
			for (auto bit : codes[i]) {
				buf_size++;
				buf = (buf << 1) + bit;
				if (buf_size == CHUNK_SIZE) {
					output_chars.push_back(buf);
					buf = 0;
					buf_size = 0;
				}
			}
		}

		fprintf(output, "%c", char(buf_size));
		for (auto i : output_chars) {
			fprintf(output, "%c", i);
		}
		fprintf(output, "%c", buf);

		fclose(input);
		fclose(output);
	}

	void decompress_file(const char* input_filename = "input.txt", const char* output_filename = "output.txt") {
		char ch;
		input = fopen(input_filename, "rb");
		output = fopen(output_filename, "wb");

		tree.clear();
		codes.clear();
		input_buf.clear();
		while (!forest.empty()) forest.pop();

		read_tree();
		char last_byte_size;
		fscanf(input, "%c", &last_byte_size);
		init_codes();
		
		int pos = tree.size() - 1;
		while (fscanf(input, "%c", &ch) != -1) {			
			input_buf.push_back(ch);
		}
		for (int input_index = 0; input_index < input_buf.size(); input_index++) {
			ch = input_buf[input_index];

			if (input_index == input_buf.size() - 1) {
				for (int i = last_byte_size - 1; i >= 0; i--) {
					pos = update_tree_position((ch >> i) & 1, pos);
					if (pos < (tree.size() + 1) / 2) {
						fprintf(output, "%c", char(pos));
						pos = tree.size() - 1;
					}
				}
				continue;
			}
			for (int i = CHUNK_SIZE - 1; i >= 0; i--) {
				pos = update_tree_position((ch >> i) & 1, pos);
				if (pos < (tree.size() + 1) / 2) {
					fprintf(output, "%c", char(pos));
					pos = tree.size() - 1;
				}
			}
		}

	}
};

int main() {

	Archiever archiever;
	archiever.compress_file();
	archiever.decompress_file("output.txt", "decompressed.txt");
	return 0;
}