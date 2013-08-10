#include <primitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char* argv[]) {
    int c; int vertices = 0;
    while ((c = getopt(argc, argv, "n:")) != -1) {
        if (c == 'n') {
            printf("Nodes: %s\n", optarg);
            vertices = atoi(optarg);
        }
    }
    if (vertices < 4) {
        fprintf(stderr, "Need at least 4 vertices\n");
        fprintf(stderr, "Usage: %s -n <nodes>\n", argv[0]);
        return 0;
    }
    generateAndTestRandomGraph(vertices);
    return 1;
}
