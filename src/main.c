#include <primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void usage(char* argv[]) {
    fprintf(stderr, "Usage: %s [-n <nodes>] [-f filename] [-s seed]\n", argv[0]);
}

int main(int argc, char* argv[]) {
    int c; int vertices = 0;
    unsigned long int seed = 0; bool recreate = false;
    bool fileProvided = false; char* file;
    while ((c = getopt(argc, argv, "n:s:tf:")) != -1) {
        if (c == 'n') {
            printf("Nodes: %s\n", optarg);
            vertices = atoi(optarg);
        } else if (c == 's') {
            recreate = true;
            seed = strtoul(optarg, NULL, 10);
        } else if (c == 't') {
            test_permutations (3, 3);
            return 0;
        } else if (c == 'f') {
            fileProvided = true;
            file = optarg;
        }
    }
    int retval = 0;
    if (!fileProvided) {
        if (vertices < 4) {
            fprintf(stderr, "Need at least 4 vertices\n");
            usage(argv);
            return 1;
        }
        if (generateAndTestRandomGraph(vertices)) {
            retval = 0;
        } else {
            retval = 1;
        }
    } else {
        if (generateAndTestSavedGraph (file)) {
            retval = 1;
        } else {
            retval = 0;
        }
    }
    return retval;
}
