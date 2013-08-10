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

KHASH_MAP_INIT_INT(table, int);

void evolve3ConnectedGraph (igraph_t* graph, uint32_t verticesToAdd);
void construct3ConnectedGraph (igraph_t* graph, uint32_t vertices);
void adjListToAdjMatrix (igraph_adjlist_t* list, igraph_matrix_t* mat, int32_t vertices);
bool testPathExist (igraph_matrix_t* adj, 
                int32_t vertices,
                igraph_integer_t src,
                igraph_integer_t dest,
                khash_t(table)* t);
bool test3ConnectedResilience (const igraph_t* graph, 
                                igraph_integer_t dest, 
                                gsl_permutation* order, 
                                int size);
void generateAndTestRandomGraph (int vertices);
void recreate3ConnectedGraph (igraph_t *graph, char* commands);
void set_rng_seed (unsigned long int seed);
#endif
