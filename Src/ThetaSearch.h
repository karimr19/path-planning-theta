#ifndef THETA_SEARCH_H
#define THETA_SEARCH_H

#include <list>
#include <unordered_map>
#include <cmath>
#include <fstream>
#include <limits>
#include <chrono>
#include "SearchResult.h"
#include "EnvironmentOptions.h"
#include "Map.h"

class ThetaSearch
{
public:
    ThetaSearch();
    ~ThetaSearch();

    virtual SearchResult startSearch(const Map &Map, const EnvironmentOptions &options);

    static bool lineOfSight(Node *first, Node* second, const Map &map, const EnvironmentOptions &options);
    static double distance(int i1, int j1, int i2, int j2);
    void setIsAstar(bool astar);

protected:
    void insertOpen(Node new_node);
    std::list<Node> getSuccessors(Node node_to_expand, const Map &map, const EnvironmentOptions &options);
    void makePrimaryPath(Node last_node);//Makes path using back pointers
    void makeSecondaryPath();//Makes another type of path(sections or points)
    virtual void resetParent(Node *current, Node parent, const Map &map, const EnvironmentOptions &options);
    bool stopSearch() const;
    Node getNodeWithMinF();

    SearchResult search_result;
    std::list<Node> primary_path, secondary_path;
    std::unordered_map<uint64_t,Node> CLOSE;
    std::unordered_map<int, std::list<Node>> OPEN;
    bool is_astar;
    int open_size;
    double h_weight; //weight of h-value

    static uint64_t nodeHash(int i, int j, const Map &map);
    static bool nodeIsFinish(Node* node, const Map &map);

    static double calculateHeuristic(int current_i, int current_j, const EnvironmentOptions &options, const Map &map);
};
#endif