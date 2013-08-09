#include <primitives.h>
#include <stdio.h>
void testK4Routing () {
    igraph_t graph;
    construct3ConnectedGraph (&graph, 4);
    int order[3] = {1, 2, 3};
    testPathExist (&graph, 2, 0, order, 3);
}
int main(int argc, char* argv[]) {
    printf("Testing K4 routing\n");
    testK4Routing();
    return 1;
}
