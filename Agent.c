// Implementation of the Agent ADT

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Agent.h"
#include "Map.h"
#include "PQ.h"

// This struct stores information about an individual agent and can be
// used to store information that the agent needs to remember.
struct agent {
    char *name;
    int startLocation;
    int location;
    int maxStamina; // max stamina
    int stamina;    // current stamina
    int strategy;
    Map map;
    
    // TODO: Add more fields here as needed
    int *visited_map;
    int *previous_map;
    int *all_adj_visited_map;
};

static Move dfsMove(Agent agent, Map m);
static Move clvMove(Agent agent, Map m);
static Move chooseRandomMove(Agent agent, Map m);
static int filterRoads(Agent agent, Road roads[], int numRoads, Road legalRoads[]);

// Helper Functions for DFS
static bool allAdjVisited(Agent agent, Road roads[], int numRoads);
static bool wholeMapVisited(Agent agent, Map m);
static int lowestID(Agent agent, Map m, Road roads[], int numRoads);

/**
 * Creates a new agent
 */
Agent AgentNew(int start, int stamina, int strategy, Map m, char *name) {
    if (start >= MapNumCities(m)) {
        fprintf(stderr, "error: starting city (%d) is invalid\n", start);
        exit(EXIT_FAILURE);
    }
    
    Agent agent = malloc(sizeof(struct agent));
    if (agent == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    agent->startLocation = start;
    agent->location = start;
    agent->maxStamina = stamina;
    agent->stamina = stamina;
    agent->strategy = strategy;
    agent->map = m;
    agent->name = strdup(name);

    // TODO: You may need to add to this
    agent->visited_map = calloc(MapNumCities(m), sizeof(int));
    agent->visited_map[agent->location] = 1;

    agent->previous_map = calloc(MapNumCities(m), sizeof(int));

    agent->all_adj_visited_map = calloc(MapNumCities(m), sizeof(int));
    
    return agent;
}

/**
 * Frees all memory associated with the agent
 * NOTE: You should not free the map because the map is owned by the
 *       main program, and the main program will free it
 */
void AgentFree(Agent agent) {
    // TODO: You may need to update this to free any extra memory you use
    free(agent->name);
    free(agent->visited_map);
    free(agent->previous_map);
    free(agent->all_adj_visited_map);
    free(agent);
}

////////////////////////////////////////////////////////////////////////
// Making moves

/**
 * Calculates the agent's next move
 */
Move AgentGetNextMove(Agent agent, Map m) {
    switch (agent->strategy) {
        case STATIONARY: return (Move){agent->location, 0};
        case RANDOM:     return chooseRandomMove(agent, m);
        case CHEAPEST_LEAST_VISITED: return clvMove(agent, m);
        case DFS: return dfsMove(agent, m);
        // TODO: Implement more strategies here
        default:
            printf("error: strategy not implemented yet\n");
            exit(EXIT_FAILURE);
    }
}

// Function that checks if all adjacent cities are visited
static bool allAdjVisited(Agent agent, Road roads[], int numRoads) {
    for (int i = 0; i < numRoads; i++) {
        Road r = roads[i];
        if (agent->visited_map[r.to] == 0) {
            return false;
        }
    }
    agent->all_adj_visited_map[agent->location] = 1;
    return true;
}

// Function that checks if all cities are visited
static bool wholeMapVisited(Agent agent, Map m) {
    for (int j = 0; j < MapNumCities(m); j++) {
        if (agent->visited_map[j] == 0) {
            return false;
        }
    }
    return true;
}

// Function that returns the lowest ID city that hasnt been visited
static int lowestID(Agent agent, Map m, Road roads[], int numRoads){
    
    int lowest_city = MapNumCities(m);
    for (int k = 0; k < numRoads; k++) {
        Road s = roads[k];
        if ( s.to < lowest_city && agent->visited_map[s.to] == 0) {
            lowest_city = s.to;
        }
    }
    return lowest_city;
}

static Move dfsMove(Agent agent, Map m){

    Road *roads = malloc(MapNumCities(m) * sizeof(Road));
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);

    // Check if all adjacent cities/whole map has been visited
    bool all_adj_visited = allAdjVisited(agent, roads, numRoads);
    bool whole_map_visited = wholeMapVisited(agent, m);

    Move move;
    // Get city with the lowest ID & not visited
    int lowest_city = lowestID(agent, m, roads, numRoads);
    
    // All adjacent visited but whole map not visited
    if (all_adj_visited && !(whole_map_visited)) {
        move = (Move) {agent->previous_map[agent->location], MapContainsRoad(m, agent->location, agent->previous_map[agent->location])};
    }
    
    // All cities visited
    else if (all_adj_visited && whole_map_visited) {
        // Reset visited array
        for (int z = 0; z < MapNumCities(m); z++) {
            agent->visited_map[z] = 0;
        }
        int starting_city = lowestID(agent, m, roads, numRoads);

        agent->previous_map[starting_city] = agent->location;
        move = (Move){starting_city, MapContainsRoad(m, agent->location, starting_city)};
    }

    else {
        agent->previous_map[lowest_city] = agent->location;
        move = (Move){lowest_city, MapContainsRoad(m, agent->location, lowest_city)};
    }    

    // Not enough stamina
    if (agent->stamina < move.staminaCost){
        move = (Move){agent->location, 0};
    }

    //printf("to: %d, staminacost: %d\n", move.to, move.staminaCost);
    free(roads);
    return move;
}

static Move clvMove(Agent agent, Map m){
    Road *roads = malloc(MapNumCities(m) * sizeof(Road));
    Road *legalRoads = malloc(MapNumCities(m) * sizeof(Road));

    // Get all roads to adjacent cities
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);

    // Filter out roads that the agent does not have enough stamina for
    int numLegalRoads = filterRoads(agent, roads, numRoads, legalRoads);

    Move move;
    if (numLegalRoads > 0) {
        // best_city is the least visited city

        Road best_city = legalRoads[0];
        for (int a = 1; a < numLegalRoads; a++) {
            Road r = legalRoads[a];
            if (agent->visited_map[r.to] < agent->visited_map[best_city.to]) {
                best_city = r;
            }
            if (agent->visited_map[r.to] == agent->visited_map[best_city.to] && r.length < best_city.length) {
                best_city = r;
            }
            if (agent->visited_map[r.to] == agent->visited_map[best_city.to] && r.length == best_city.length && r.to < best_city.to) {
                best_city = r;
            }
        }

        // update move 
        move = (Move){best_city.to, best_city.length};

    } else {
        // The agent must stay in the same location
        move = (Move){agent->location, 0};
    }
    
    free(legalRoads);
    free(roads);
    return move;    
}

static Move chooseRandomMove(Agent agent, Map m) {
    Road *roads = malloc(MapNumCities(m) * sizeof(Road));
    Road *legalRoads = malloc(MapNumCities(m) * sizeof(Road));

    // Get all roads to adjacent cities
    int numRoads = MapGetRoadsFrom(m, agent->location, roads);

    // Filter out roads that the agent does not have enough stamina for
    int numLegalRoads = filterRoads(agent, roads, numRoads, legalRoads);

    Move move;
    if (numLegalRoads > 0) {
        // Sort the roads using insertion sort
        for (int i = 1; i < numLegalRoads; i++) {
            Road r = legalRoads[i];
            int j = i;
            while (j > 0 && r.to < legalRoads[j - 1].to) {
                legalRoads[j] = legalRoads[j - 1];
                j--;
            }
            legalRoads[j] = r;
        }
        
        // nextMove is randomly chosen from the legal roads
        int k = rand() % numLegalRoads;
        move = (Move){legalRoads[k].to, legalRoads[k].length};
    } else {
        // The agent must stay in the same location
        move = (Move){agent->location, 0};
    }
    
    free(legalRoads);
    free(roads);
    return move;
}

// Takes an array with all the possible roads and puts the ones the agent
// has enough stamina for into the legalRoads array
// Returns the number of legal roads
static int filterRoads(Agent agent, Road roads[], int numRoads,
                       Road legalRoads[]) {
    int numLegalRoads = 0;
    for (int i = 0; i < numRoads; i++) {
        if (roads[i].length <= agent->stamina) {
            legalRoads[numLegalRoads++] = roads[i];
        }
    }
    return numLegalRoads;
}

/**
 * Executes a given move by updating the agent's internal state
 */
void AgentMakeNextMove(Agent agent, Move move) {
    // Increasing the locations on the visited array by 1
    agent->visited_map[move.to] ++;

    if (move.to == agent->location) {
        agent->stamina = agent->maxStamina;
        return;
    }

    if (agent->all_adj_visited_map[agent->location] == 0) {
        agent->previous_map[move.to] = agent->location;
    }

    agent->stamina -= move.staminaCost;
    agent->location = move.to;    
    // TODO: You may need to add to this to handle different strategies
}

////////////////////////////////////////////////////////////////////////
// Gets information about the agent
// NOTE: It is expected that these functions do not need to be modified

/**
 * Gets the name of the agent
 */
char *AgentName(Agent agent) {
    return agent->name;
}

/**
 * Gets the current location of the agent
 */
int AgentLocation(Agent agent) {
    return agent->location;
}

/**
 * Gets the amount of stamina the agent currently has
 */
int AgentStamina(Agent agent) {
    return agent->stamina;
}

////////////////////////////////////////////////////////////////////////
// Learning information

/**
 * Tells the agent where the thief is
 */
void AgentGainInfo(Agent agent, int thiefLocation) {
    // TODO: Stage 3
}

////////////////////////////////////////////////////////////////////////
// Displaying state

/**
 * Prints information about the agent (for debugging purposes)
 */
void AgentShow(Agent agent) {
    // TODO: You can implement this function however you want
    //       You can leave this function blank if you want
}

////////////////////////////////////////////////////////////////////////
