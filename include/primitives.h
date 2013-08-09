/* Some primitive operations for creating igraph structures of interest
 *
 */
#include <igraph.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <gsl/gsl_rng.h>
#include "khash.h"
#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

KHASH_MAP_INIT_INT(table, int);

void evolve3ConnectedGraph (igraph_t* graph, uint32_t verticesToAdd);
void construct3ConnectedGraph (igraph_t* graph, uint32_t vertices);
khash_t(table)* orderToTable(int order[], int size);
void adjListToAdjMatrix (igraph_adjlist_t* list, igraph_matrix_t* mat, int32_t vertices);
bool testPathExist (igraph_t* graph, igraph_integer_t src, igraph_integer_t dest, int order[], int size);
#endif
