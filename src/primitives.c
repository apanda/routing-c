#include "primitives.h"
#include <string.h>
static gsl_rng *r = NULL;
//Init the rng
void InitRng () {
    if (r == NULL) {
        const gsl_rng_type *T;
        unsigned long int seed;
        FILE* random = fopen("/dev/random", "r");
        fread(&seed, 1, sizeof(seed), random);
        fclose(random);
        gsl_rng_env_setup();
        T = gsl_rng_default;
        r = gsl_rng_alloc (T);
        printf("Setting seed to %lu\n", seed);
        gsl_rng_set (r, seed);
    }
}

void set_rng_seed (unsigned long int seed) {
    printf("Setting seed to %lu\n", seed);
    gsl_rng_set (r, seed);
}

// Split one edge and add a new edge connecting the new vertex to a third vertex
static void splitEdge (igraph_t* graph) {
    // # of edges
    int32_t edges = igraph_ecount(graph);
    // # of vertices
    int32_t vertices = igraph_vcount(graph);
    // Choose edge randomly
    int32_t chosen_edge = (int32_t)(gsl_rng_get (r) % (unsigned long int)edges);
    int32_t err;
    igraph_vector_t added_edges;
    // Choose a vertex
    int32_t chosen_vertex = (int32_t)(gsl_rng_get (r) % (unsigned long int)vertices);
    int32_t i = 0;
    int32_t small = 0;
    igraph_integer_t v[3];

    // Find edge endpoints
    igraph_edge(graph, chosen_edge, &v[0], &v[1]);
    v[2] = igraph_vcount(graph);
    // Add a vertex
    err = igraph_add_vertices(graph, 1, NULL);
    // Delete old edge
    igraph_delete_edges(graph, igraph_ess_1(chosen_edge));
    if (v[0] > v[1]) {
        small = 1;
    }
    
    for (i = 0; i < 2; i++) {
        // Skip over v0
        if (chosen_vertex >= v[(small + i) % 2]) {
            chosen_vertex = (chosen_vertex + 1) % vertices;  
        }
        
    }
    
    // Add new edges
    igraph_vector_init (&added_edges, 3 * 2);
    for (i = 0; i < 2; i++) {
        VECTOR(added_edges)[i * 2] = v[i]; VECTOR(added_edges)[(i * 2) + 1] = v[2];
    }
    VECTOR(added_edges)[4] = chosen_vertex; VECTOR(added_edges)[5] = v[2];

    igraph_add_edges (graph, &added_edges, NULL);
    igraph_vector_destroy (&added_edges);
}


// Split two edges by adding vertices, connect the two new vertices
static void addTwoVertices (igraph_t* graph) {
    // # of edges
    int32_t edges = igraph_ecount(graph);
    // # of vertices
    int32_t vertices = igraph_vcount(graph);
    int32_t i = 0;
    igraph_integer_t new_vertices[2] = {vertices, vertices + 1};
    igraph_add_vertices(graph, 2, NULL);
    igraph_vector_t added_edges;
    igraph_vector_t deleted_edges;
    // Choose a vertex
    igraph_integer_t v[2];
    int32_t smaller = 0;
    igraph_vector_init (&deleted_edges, 2);
    // Choose edge randomly
    VECTOR(deleted_edges)[0] = (int32_t)(gsl_rng_get (r) % (unsigned long int)edges);
    VECTOR(deleted_edges)[1] = (int32_t)(gsl_rng_get (r) % (unsigned long int)edges);
    if (VECTOR(deleted_edges)[0] > VECTOR(deleted_edges)[1]) {
        smaller = 1;
    }
    VECTOR(deleted_edges)[smaller] = ((int32_t)(VECTOR(deleted_edges)[1]) + 1) % edges;
    igraph_vector_init (&added_edges, 5 * 2);
    for (i = 0; i < 2; i++) {
        igraph_edge(graph, VECTOR(deleted_edges)[i], &v[0], &v[1]);
        VECTOR(added_edges)[i*4 + 0] = v[0]; VECTOR(added_edges)[i*4 + 1] = new_vertices[i];
        VECTOR(added_edges)[i*4 + 2] = v[1]; VECTOR(added_edges)[i*4 + 3] = new_vertices[i];
    }
    igraph_delete_edges(graph, igraph_ess_vector(&deleted_edges));
    VECTOR(added_edges)[8] = new_vertices[0]; VECTOR(added_edges)[9] = new_vertices[1];
    igraph_add_edges (graph, &added_edges, NULL);
    igraph_vector_destroy (&deleted_edges);
    igraph_vector_destroy (&added_edges);
}

// Add a new edge between two vertexes. This might result in a vertex connecting two already connected
// vertexes.
inline static void addNewEdge (igraph_t* graph) {
    // # of edges
    // # of vertices
    int32_t vertices = igraph_vcount(graph);
    int32_t i = 0;
    igraph_integer_t verticesToConnect[2];
    igraph_vector_t added_edges; 
    verticesToConnect[0] = (int32_t)(gsl_rng_get (r) % (unsigned long int)vertices);
    verticesToConnect[1] = (int32_t)(gsl_rng_get (r) % (unsigned long int)vertices);
    if (verticesToConnect[0] == verticesToConnect[1]) {
        verticesToConnect[1] = (verticesToConnect[1] + 1) % vertices;
    }
    igraph_vector_init(&added_edges, 2);
    for (i = 0; i < 2; i++) {
        VECTOR(added_edges)[i] = verticesToConnect[i];
    }
    igraph_add_edges (graph, &added_edges, NULL);
    igraph_vector_destroy (&added_edges);
}

// Generate random graph using BG-operations
void evolve3ConnectedGraph (igraph_t* graph, uint32_t verticesToAdd) {
    uint32_t added = 0;
    while (added < verticesToAdd) {
        int32_t operation = (int32_t)(gsl_rng_get(r) % 3);
        printf("%d", operation);
        switch (operation) {
            case 0:
                splitEdge (graph);
                added += 1;
                break;
            case 1:
                addTwoVertices (graph);
                added += 2;
                break;
            case 2:
                addNewEdge (graph);
                break;
        };
    }
}

void construct3ConnectedGraph (igraph_t* graph, uint32_t vertices) {
    printf("k");
    // Start with K4
    int err = igraph_full (graph,
                 4, // Vertices
                 false, // Undirected
                 false); // No self loops
    if (err != 0) {
        printf ("Failed to construct graph\n");
        return;
    }
    if (vertices > 4) {
        evolve3ConnectedGraph (graph, vertices - 4);
    }
    printf("\n");
}

void recreate3ConnectedGraph (igraph_t *graph, const char* commands) {
    int clen = strlen(commands);
    int err;
    int32_t operation;
    for (int i = 0; i < clen; i++) {
        switch (commands[i]) {
            case 'k':
                err = igraph_full (graph,
                                    4,
                                    false, 
                                    false);
                if (err != 0) {
                    printf ("Failed to construct graph\n");
                    return;
                }
                break;
            case '0':
                operation = (int32_t)(gsl_rng_get(r) % 3);
                splitEdge (graph);
                break;
            case '1':
                operation = (int32_t)(gsl_rng_get(r) % 3);
                addTwoVertices (graph);
                break;
            case '2':
                operation = (int32_t)(gsl_rng_get(r) % 3);
                addNewEdge (graph);
                break;
        }
    }
}

static khash_t(table)* orderToTable(const gsl_permutation* order, const int size) {
    khash_t(table)* t = kh_init(table);
    int ret = 0;
    for (int i = 0; i < size; i++) {
        int ith = gsl_permutation_get(order, i);
        int nextth = gsl_permutation_get(order, (i + 1) % size);
        khiter_t k = kh_put(table, t, ith, &ret);
        assert(ret);
        kh_value(t, k) = nextth;
    }
    return t;
}



void adjListToAdjMatrix (const igraph_adjlist_t* list,
           igraph_matrix_t* mat,
           int32_t vertices) {
    igraph_matrix_null(mat);
    int i = 0;
    for (i = 0; i < vertices; i++) {
        igraph_vector_t* nbrs = igraph_adjlist_get(list, i);
        long int size = igraph_vector_size(nbrs);
        int j = 0;
        for (j = 0; j < size; j++) {
            int other_vertex = VECTOR(*nbrs)[j];
            igraph_matrix_set(mat, i, other_vertex, MATRIX(*mat, i, other_vertex) + 1);
        }
    }
}

static void graphAdjMatrix (const igraph_t *graph, igraph_matrix_t* mat) {
    int32_t vertices = igraph_vcount(graph);
    //
    // Adjacency matrix
    //  1. Get an adjacency list
    igraph_adjlist_t adj_list;
    igraph_adjlist_init(graph, &adj_list, IGRAPH_ALL);
    //  2. Create matrix
    igraph_matrix_init(mat, vertices, vertices);
    //  3. Translate
    adjListToAdjMatrix(&adj_list, mat, vertices);
    igraph_adjlist_destroy(&adj_list);
}

bool testPathExist (const igraph_matrix_t* adj, 
                const int32_t vertices,
                const igraph_integer_t src,
                const igraph_integer_t dest,
                khash_t(table)* t[]) {
    igraph_matrix_t visited;
    // Hashtable for routing

    // Matrix for tracking what has been visited
    igraph_matrix_init(&visited, vertices, vertices);
    igraph_matrix_null(&visited);

    // Start from src
    igraph_integer_t current = src;
    // Assume packet came in on src
    igraph_integer_t link = src;
    ///printf("\n\nStarting src = %d link = %d dest = %d\n", src, link, dest);
    while (current != dest) {
        // Check to see if we have link to destination
        if (MATRIX(*adj, current, dest) > 0) {
            link = current;
            current = dest;
            igraph_matrix_set(&visited, current, dest, 1);
            //printf("Taking direct link to dest\n");
        } else {
            // If we are entering a loop abort
            if (MATRIX(visited, current, link) == 1) {
                igraph_matrix_destroy(&visited);
                //printf("Loop detected\n");
                return false;
            }
            
            // Update loop detection information
            igraph_matrix_set(&visited, current, link, 1);
            khint_t bucket = kh_get(table, t[current], link);            
            if (bucket == kh_end(t[current])) {
                //printf ("current: %d, link: %d, src: %d, dest: %d\n", current, link, src, dest);
            }
            assert(bucket != kh_end(t));
            int next = kh_value(t[current], bucket);
            if (MATRIX(*adj, current, next) > 0) {
                // If a link exists...
                link = current;
                current = next;
                //printf("Going to node %d link = %d\n", current, link);
            } else {
                // No link exists...
                link = next;
                //printf("Reflecting node %d link = %d\n", current, link);
            }
        }
    }
    igraph_matrix_destroy(&visited);
    return true;
}

static inline void RemoveEdge(igraph_matrix_t* mat, int32_t v0, int32_t v1) {
    igraph_matrix_set(mat, v0, v1, MATRIX(*mat, v0, v1) - 1);
    igraph_matrix_set(mat, v1, v0, MATRIX(*mat, v1, v0) - 1);
}

static inline void AddEdge(igraph_matrix_t* mat, int32_t v0, int32_t v1) {
    igraph_matrix_set(mat, v0, v1, MATRIX(*mat, v0, v1) + 1);
    igraph_matrix_set(mat, v1, v0, MATRIX(*mat, v1, v0) + 1);
}

bool test3ConnectedResilience (const igraph_t* graph, 
                                const igraph_integer_t dest, 
                                gsl_permutation** order, 
                                const int size) {
    int32_t vertices = igraph_vcount(graph);
    igraph_matrix_t adjMatrix;
    graphAdjMatrix (graph, &adjMatrix);
    int32_t edges = igraph_ecount(graph);
    int32_t edge[2];
    khash_t(table)** t = malloc(sizeof(khash_t(table)*) * size);
    for (int i = 0; i < size; i++) {
        t[i] = orderToTable(order[i], size);
    }
    igraph_real_t mincut = 0.0;
    igraph_mincut_value(graph, &mincut, NULL);
    assert(mincut >= 3.0);
    bool ret = true;
    for (edge[0] = 0; edge[0] < edges; edge[0]++) {
        igraph_integer_t v0[2];
        // Get edges
        igraph_edge(graph, edge[0], &v0[0], &v0[1]);
        // Remove edge
        RemoveEdge(&adjMatrix, v0[0], v0[1]);
        for (edge[1] = edge[0] + 1; edge[1] < edges; edge[1]++) {
            igraph_integer_t v1[2];
            // We assume things are still connected (the graph is 2 connected)
            // hence omitting connected check
            igraph_edge(graph, edge[1], &v1[0], &v1[1]);
            RemoveEdge(&adjMatrix, v1[0], v1[1]);
            //printf ("Edges removed:%d-%d  %d-%d\n", v0[0], v0[1], v1[0], v1[1]);
            for (int src = 0; src < vertices; src++) {
                if (src == dest) {
                    continue;
                }
                if(!testPathExist(&adjMatrix, vertices, src, dest, t)) {
                    //printf("%d-%d %d-%d ", v0[0], v0[1], v1[0], v1[1]);
                    ret = false;
                    goto cleanup;
                }
            }
            AddEdge(&adjMatrix, v1[0], v1[1]);
        }
        AddEdge(&adjMatrix, v0[0], v0[1]);
    }
cleanup:
    igraph_matrix_destroy(&adjMatrix);
    for (int i = 0; i < size; i++) {
        kh_destroy(table, t[i]); 
    }
    free(t);
    return ret;
}

static int next_permutation (gsl_permutation** permutations, int length) {
    for (int i = length - 1; i >= 0; i--) {
        if (gsl_permutation_next (permutations[i]) == GSL_SUCCESS) {
            return GSL_SUCCESS;
        } else {
            gsl_permutation_init(permutations[i]);
        }
    }
    return GSL_FAILURE;
}

static void init_permutations (gsl_permutation*** permutations, int length, int psize) {
    *permutations = malloc(sizeof(gsl_permutation*) * length);
    for (int i = 0; i < length; i++) {
        (*permutations)[i] = gsl_permutation_alloc(psize);
        gsl_permutation_init((*permutations)[i]);
    }
}

static void destroy_permutations (gsl_permutation** permutations, int length) {
    for (int i = 0; i < length; i++) {
        gsl_permutation_free (permutations[i]);
    }
    free(permutations);
}

static void print_permutations (gsl_permutation** permutations, int length) {
    for (int i = 0; i < length; i++) {
        printf ("| ");
        gsl_permutation_fprintf(stdout, permutations[i], "%u ");
        printf("| ");
    }
}

void test_permutations (int length, int psize) {
    gsl_permutation** permutations;
    init_permutations(&permutations, length, psize);
    do {
        print_permutations(permutations, length);
        printf("\n");
    } while (next_permutation (permutations, length) == GSL_SUCCESS);
}

static bool testGraph (igraph_t* graph) {
    int32_t vertices = igraph_vcount(graph);
    // User vertices - 1 (the highest vertex as dest)
    igraph_integer_t dest = vertices - 1;
    gsl_permutation **porder;
    init_permutations (&porder, (vertices - 1), (vertices - 1));
    bool any_success = false;
    int success_count = 0;
    do {
        print_permutations(porder, (vertices - 1));
        if (test3ConnectedResilience (graph, dest, porder, (vertices - 1))) {
            // Permute order here somehow
            any_success |= true;
            printf ("Success\n");
            success_count++;
        } else {
            printf ("Failure\n");
        }
    } while (next_permutation (porder, (vertices - 1)) == GSL_SUCCESS);
    if (!any_success) {
        printf("Everything failed, writing graph out for analysis to a.gml\n");
        FILE* out = fopen("a.gml", "w");
        igraph_write_graph_gml(graph, out, NULL, NULL);
        fflush(out);
        fclose(out);
    }
    printf("Success count is %d\n", success_count);
    destroy_permutations(porder, (vertices - 1));
    return any_success;
}

bool generateAndTestRandomGraph (int vertices) {
    igraph_t graph;
    construct3ConnectedGraph (&graph, vertices);
    return testGraph (&graph);
}

