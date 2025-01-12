#include <bits/stdc++.h>
#include "Archiever.h"
using namespace std;
typedef long long ll;
typedef unsigned long long ull;



int main() {
    Archiever archiever;

    archiever.compress_file("input.txt", "output.txt");
    archiever.decompress_file("output.txt", "decompressed.txt");
    return 0;
}

// int32_t main(int argc, char *argv[]) {
//     Archiever archiever;
//     if (!strcmp("encode", argv[1])) {
//         archiever.compress_file(argv[3], argv[2]);
//     } else {
//         archiever.decompress_file(argv[2], argv[3]);
//     }
//     return 0;
// }

//https://sch9.ru/gate/benchmarker/results_v2/03d4bc8a-1f17-441a-9669-aa975987913c.txt
