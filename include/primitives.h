/* Some primitive operations for creating igraph structures of interest
 *
 */
#include <igraph/igraph.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_permutation.h>
#include "khash.h"
#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

// Set up hash map named table
KHASH_MAP_INIT_INT(table, int);

// Evolve a 3-connected graph by adding more nodes. This does not actually
// guarantee that only vertivesToAdd vertexes are added. The actual number of
// vertexes can be 1 more than required. 
void evolve3ConnectedGraph (igraph_t* graph, uint32_t verticesToAdd);

// Take an uninitialized igraph_t struct and create a graph.
void construct3ConnectedGraph (igraph_t* graph, uint32_t vertices);

// Convert an adjacency list (natively provided by igraph) to an adjacency
// matrix (easier to use for our purposes).
void adjListToAdjMatrix (const igraph_adjlist_t* list, 
                        igraph_matrix_t* mat, 
                        int32_t vertices);

// Test that a path exists from source to destination using the provided routing
// table
bool testPathExist (const igraph_matrix_t* adj, 
                const int32_t vertices,
                const igraph_integer_t src,
                const igraph_integer_t dest,
                const khash_t(table)* t);

// Check to see whether a particular routing order is 2-resilient
bool test3ConnectedResilience (const igraph_t* graph, 
                                const igraph_integer_t dest, 
                                const gsl_permutation* order, 
                                const int size);
bool generateAndTestRandomGraph (int vertices);
void recreate3ConnectedGraph (igraph_t *graph, const char* commands);
void set_rng_seed (unsigned long int seed);
void InitRng ();
#endif
