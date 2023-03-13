// Implementation of the Map ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Map.h"

struct map { 
    // TODO
    int num_cities;
    int num_roads;
    int **road_map;
    char **names;
};

// Helper Functions
static int validVertex(Map m, int city);
static char *myStrdup(char *s);

/**
 * Creates a new map with the given number of cities
 * Assumes that numCities is positive
 */
Map MapNew(int numCities) {
    // TODO
    assert(numCities > 0);

    Map m = malloc(sizeof(struct map));

    m->num_cities = numCities;
    m->num_roads = 0;

    m->road_map = malloc(numCities * sizeof(int *));
    for (int i = 0; i < numCities; i++) {
        m->road_map[i] = calloc(numCities, sizeof(int));
    }

    m->names = calloc(m->num_cities, sizeof(char*));

    return m;
}

/**
 * Frees all memory associated with the given map
 */
void MapFree(Map m) {
    // TODO

    // 1 - Free road_map
    for (int i = 0; i < m->num_cities; i++) {
        free(m->road_map[i]);
    }
    free(m->road_map);

    // 2 - Free names array
    for (int i = 0; i < m->num_cities; i++) {
        free(m->names[i]);
    }
    free(m->names);

    free(m);
}

/**
 * Returns the number of cities on the given map
 */
int MapNumCities(Map m) {
    // TODO
    return m->num_cities;
}

/**
 * Returns the number of roads on the given map
 */
int MapNumRoads(Map m) {
    // TODO
    return m->num_roads;
}

/**
 * Inserts a road between two cities with the given length
 * Does nothing if there is already a road between the two cities
 * Assumes that the cities are valid and are not the same
 * Assumes that the length of the road is positive
 */
void MapInsertRoad(Map m, int city1, int city2, int length) {
    // TODO
    assert(validVertex(m, city1));
    assert(validVertex(m, city2));
    assert(city1 != city2);
    assert(length > 0.0);

    if (m->road_map[city1][city2] == 0) {
        m->road_map[city1][city2] = length;
        m->road_map[city2][city1] = length;
        m->num_roads ++;
    }
    return;
}

/**
 * Sets the name of the given city
 */
void MapSetName(Map m, int city, char *name) {
    // TODO
    m->names[city] = myStrdup(name);
}

/**
 * Returns the name of the given city
 */
char *MapGetName(Map m, int city) {
    // TODO
    return m->names[city];
}

/**
 * Checks if there is a road between the two given cities
 * Returns the length of the road if there is a road, and 0 otherwise
 */
int MapContainsRoad(Map m, int city1, int city2) {
    // TODO
    assert(validVertex(m, city1));
    assert(validVertex(m, city2));
    
    return m->road_map[city1][city2];
}

/**
 * Returns the number of roads connected to the given city and stores
 * them in the given roads array. The `from` field should be equal to
 * `city` for all the roads in the array.
 * Assumes that the roads array is large enough to store all the roads
 */
int MapGetRoadsFrom(Map m, int city, Road roads[]) {
    // TODO
    int road_count = 0;

    for (int i = 0; i < m->num_cities; i ++) {
        if (m->road_map[city][i] > 0) {
            roads[road_count].from = city;
            roads[road_count].to = i;
            roads[road_count].length = m->road_map[city][i];
            road_count ++;
        }
    }
    return road_count;
}

/**
 * Displays the map
 * !!! DO NOT EDIT THIS FUNCTION !!!
 * This function will work once the other functions are working
 */
void MapShow(Map m) {
    printf("Number of cities: %d\n", MapNumCities(m));
    printf("Number of roads: %d\n", MapNumRoads(m));
    
    Road *roads = malloc(MapNumRoads(m) * sizeof(Road));
    if (roads == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);    
    }
    
    for (int i = 0; i < MapNumCities(m); i++) {
        printf("[%d] %s has roads to:", i, MapGetName(m, i));
        int numRoads = MapGetRoadsFrom(m, i, roads);
        for (int j = 0; j < numRoads; j++) {
            if (j > 0) {
                printf(",");
            }
            printf(" [%d] %s (%d)", roads[j].to, MapGetName(m, roads[j].to),
                   roads[j].length);
        }
        printf("\n");
    }
    
    free(roads);
}

// Reference: This function is from lab07 graph.c
static int validVertex(Map m, int city) {
    return city >= 0 && city < m->num_cities;
}

// Reference: This function is from lab05 Fb.c
static char *myStrdup(char *s) {
    char *copy = malloc((strlen(s) + 1) * sizeof(char));
    if (copy == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    return strcpy(copy, s);
}