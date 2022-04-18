#ifndef SEARCH_H
#define SEARCH_H

#include <list>
#include <vector>
#include <limits>
#include <chrono>
#include <unordered_map>
#include <map>
#include <set>
#include <cmath>
#include <fstream>
#include <iostream>
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include "Direction.h"

class Search {
public:
    Search();

    ~Search();

    // Main logic of the program: starts search algorithm.
    SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options);
//    static bool lineOfSight(Node *first, int successor_x, int successor_y, const Map &map,
//                            const EnvironmentOptions &options);
    static bool lineOfSight(Node *first, int successor_x, int successor_y, const Map &map, bool cut_corners);

protected:
    const double kStraightStepLength = 1;

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
    SearchResult sresult;
    // Primary path.
    std::list<Node> lppath;
    // Secondary path.
    std::list<Node> hppath;

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

    static double distance(int first_x, int first_y, int second_x, int second_y);

    void outputResultsToFiles(const Map &map);
};

#endif
