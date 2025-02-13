#include <bits/stdc++.h>
#include "Archiever.h"
using namespace std;


int32_t main(int argc, char *argv[]) {
    Archiever archiever;
    if (!strcmp("encode", argv[1])) {
        archiever.compress_file(argv[3], argv[2]);
    } else {
        archiever.decompress_file(argv[2], argv[3]);
    }
    return 0;
}


