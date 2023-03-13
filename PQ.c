// Reference: This file is from lab08

// Implementation of a priority queue of edges


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "Map.h"
#include "Agent.h"
#include "PQ.h"

#define DEFAULT_CAPACITY 64

struct PQRep {
	Road *items;
	int   size;
	int   capacity;
};

static void fixUp(PQ pq, int i);
static void fixDown(PQ pq, int i);
static void swap(Road *items, int i, int j);

PQ PQNew(void) {
	PQ pq = malloc(sizeof(*pq));
	if (pq == NULL) {
		fprintf(stderr, "Couldn't allocate PQ!\n");
		exit(EXIT_FAILURE);
	}
	
	pq->items = malloc((DEFAULT_CAPACITY + 1) * sizeof(Road));
	if (pq->items == NULL) {
		fprintf(stderr, "Couldn't allocate PQ!\n");
		exit(EXIT_FAILURE);
	}
	
	pq->size = 0;
	pq->capacity = DEFAULT_CAPACITY;
	return pq;
}

void PQFree(PQ pq) {
	free(pq->items);
	free(pq);
}

void PQInsert(PQ pq, Road e) {
	// If the PQ is full, expand it (i.e., double its capacity)
	if (pq->size == pq->capacity) {
		pq->capacity *= 2;
		pq->items = realloc(pq->items, (pq->capacity + 1) * sizeof(Road));
		if (pq->items == NULL) {
			fprintf(stderr, "Couldn't expand PQ!\n");
			exit(EXIT_FAILURE);
		}
	}
	
	// Add the new item to the end
	pq->size++;
	pq->items[pq->size] = e;
	fixUp(pq, pq->size);
}

static void fixUp(PQ pq, int i) {
	while (i > 1 && pq->items[i].length < pq->items[i / 2].length) {
		swap(pq->items, i, i / 2);
		i = i / 2;
	}
}

Road PQExtract(PQ pq) {
	assert(pq->size > 0);
	
	Road e = pq->items[1];
	pq->items[1] = pq->items[pq->size];
	pq->size--;
	fixDown(pq, 1);
	return e;
}

static void fixDown(PQ pq, int i) {
	Road *es = pq->items;
	while (true) {
		int j = i;
		int l = 2 * i;
		int r = 2 * i + 1;
		if (l <= pq->size && es[l].length < es[j].length) j = l;
		if (r <= pq->size && es[r].length < es[j].length) j = r;
		if (j == i) break;
		swap(pq->items, i, j);
		i = j;
	}
}

bool PQIsEmpty(PQ pq) {
	return (pq->size == 0);
}

void PQShow(PQ pq) {
	printf("#items = %d\n", pq->size);
	printf("Items:");
	for (int i = 1; i <= pq->size; i++) {
		printf(" (v: %d, w: %d, weight: %d)",
		       pq->items[i].to, pq->items[i].from, pq->items[i].length);
	}
	printf("\n");
}

////////////////////////////////////////////////////////////////////////

static void swap(Road *items, int i, int j) {
	Road tmp = items[i];
	items[i] = items[j];
	items[j] = tmp;
}
