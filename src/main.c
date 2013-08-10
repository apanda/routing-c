#include <primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void usage(char* argv[]) {
    fprintf(stderr, "Usage: %s -n <nodes> [-s seed]\n", argv[0]);
}

int main(int argc, char* argv[]) {
    int c; int vertices = 0;
    unsigned long int seed = 0; bool recreate = false;
    while ((c = getopt(argc, argv, "n:s:t")) != -1) {
        if (c == 'n') {
            printf("Nodes: %s\n", optarg);
            vertices = atoi(optarg);
        } else if (c == 's') {
            recreate = true;
            seed = strtoul(optarg, NULL, 10);
        } else if (c == 't') {
            test_permutations (3, 3);
            return 0;
        }
    }
    if (vertices < 4) {
        fprintf(stderr, "Need at least 4 vertices\n");
        usage(argv);
        return 1;
    }
    InitRng();
    if (recreate) {
        set_rng_seed (seed);
    }

    if (generateAndTestRandomGraph(vertices)) {
        return 0;
    } else {
        return 1;
    }
}
