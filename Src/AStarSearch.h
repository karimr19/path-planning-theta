//
// Created by Karim on 24.04.2022.
//

#ifndef KARIM_RAKHMATULLIN_ASEARCH_ASTARSEARCH_H
#define KARIM_RAKHMATULLIN_ASEARCH_ASTARSEARCH_H

#include <set>
#include <map>
#include <unordered_map>
#include <cmath>
#include <fstream>
#include <chrono>
#include "SearchResult.h"
#include "EnvironmentOptions.h"
#include "Map.h"

// Enum to make primary path.
enum Direction {
    NONE,
    HORIZONTAL,
    VERTICAL,
    MAINDIAGONAL,
    SIDEDIAGONAL
};

class AStarSearch {
public:
    AStarSearch();

    ~AStarSearch();

    // Main logic of the program: starts search algorithm.
    SearchResult startSearch(const Map &Map, const EnvironmentOptions &options);

protected:
    const double kStraightStepLength = 1;

    void outputResultsToFiles(const Map &map);
    // Contains pointers on nodes to expand.
    std::set<Node*, NodesComparator> OPEN;
    // Contains pointers on closed nodes that or not to reopen (because heuristic is monotone).
    std::map<std::pair<int, int>, Node*> CLOSED;
    // Map to easily get node with input i and j.
    std::unordered_map<uint64_t, Node*> nodes_container;

    // Calculates heuristic of node with given coords.
    static double calculateHeuristic(int current_i, int current_j, const EnvironmentOptions &options, const Map &map);

    // Returns vector of tuples: (successor_i, successor_j, is_diagonal)
    static std::vector<std::tuple<int, int, bool>> getSuccessors(int i, int j, const Map &map,
                                                                 const EnvironmentOptions &options);

    // Result of the search.
    SearchResult search_result;

    double h_weight;

    // Primary path.
    std::list<Node> primary_path;
    // Secondary path.
    std::list<Node> secondary_path;

    // Checks if successor's node exists and updates it if it has greater g_value, else creates new node.
    void checkSuccessorCell(Node *node_to_expand, int successor_i, int successor_j, bool is_diagonal,
                            const Map &map, const EnvironmentOptions &options);

    // Makes primary path from last node.
    void makePrimaryPath(Node *curNode);

    // Makes secondary path from primary path.
    void makeSecondaryPath();

    // Counts hash of the node using i and j.
    static uint64_t nodeHash(int i, int j, const Map &map);

    // Adds node to secondary path depending on direction.
    void checkDirection(const std::list<Node>::iterator &currentNode, const std::list<Node>::iterator &previousNode,
                        Direction &lastDirection);
};

#endif
