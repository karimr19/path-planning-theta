#include "LazyThetaSearch.h"

SearchResult LazyThetaSearch::startSearch(const Map &map, const EnvironmentOptions &options) {
    h_weight = options.h_weight;
    auto start = std::chrono::steady_clock::now();;
    Node node_to_expand;
    node_to_expand.i = map.getStartI();
    node_to_expand.j = map.getStartJ();
    node_to_expand.g = 0;
    node_to_expand.H = calculateHeuristic(node_to_expand.i, node_to_expand.j, options, map);
    node_to_expand.F = h_weight * node_to_expand.H;
    node_to_expand.parent = nullptr;
    insertOpen(node_to_expand);
    while (!stopSearch()) {
        search_result.number_of_steps++;
        node_to_expand = getNodeWithMinF();
        setVertex(&node_to_expand, map, options);
        CLOSE.insert({nodeHash(node_to_expand.i, node_to_expand.j, map), node_to_expand});
        OPEN[node_to_expand.i].pop_front();
        open_size--;
        if (nodeIsFinish(&node_to_expand, map)) {
            search_result.path_found = true;
            break;
        }
        std::list<Node> successors = getSuccessors(node_to_expand, map, options);
        auto it = successors.begin();
        Node* parent = &CLOSE[nodeHash(node_to_expand.i, node_to_expand.j, map)];
        while (it != successors.end()) {
            it->parent = parent;
            it->H = calculateHeuristic(it->i, it->j, options, map);
            if (parent->parent != nullptr) {
                resetParent(&*it, *it->parent, map, options);
            }
            it->F = it->g + h_weight * it->H;
            insertOpen(*it);
            it++;
        }
    }
    search_result.nodes_created = CLOSE.size() + open_size;
    if (search_result.path_found) {
        makePrimaryPath(node_to_expand);
        search_result.path_length = node_to_expand.g;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> executed_time = end - start;
    search_result.time = executed_time.count();
    if (search_result.path_found) {
        makeSecondaryPath();
//        outputResultsToFiles(map);
    }
    search_result.secondary_path = &secondary_path;
    search_result.primary_path = &primary_path;
    return search_result;
}

void LazyThetaSearch::setVertex(Node *node_to_expand, const Map &map, const EnvironmentOptions &options) {
    if (node_to_expand->parent != nullptr) {
        if (!lineOfSight(node_to_expand->parent, node_to_expand, map, options)) {
            node_to_expand->parent = findParent(node_to_expand, map, options);
            node_to_expand->g = node_to_expand->parent->g +
                                ThetaSearch::distance(node_to_expand->parent->i, node_to_expand->parent->j,
                                                      node_to_expand->i, node_to_expand->j);
            node_to_expand->F = node_to_expand->g + node_to_expand->H;
        }
    }
}

Node* LazyThetaSearch::findParent(Node *curNode, const Map &map, const EnvironmentOptions &options) {
    Node *newParent;
    double min_g = 1'000'000'000'000;
    int adjacent_i, adjacent_j;
    for (int i = -1; i <= +1; i++) {
        for (int j = -1; j <= +1; j++) {
            adjacent_i = curNode->i + i;
            adjacent_j = curNode->j + j;
            if ((i != 0 || j != 0) && map.CellOnGrid(adjacent_i, adjacent_j) &&
                (map.CellIsTraversable(adjacent_i, adjacent_j))) {
                if (i != 0 && j != 0) {
                    if (!options.allow_diagonal)
                        continue;
                    else if (!options.cut_corners) {
                        if (map.CellIsObstacle(curNode->i, adjacent_j) ||
                            map.CellIsObstacle(adjacent_i, curNode->j))
                            continue;
                    }
                    else if (!options.allow_squeeze) {
                        if (map.CellIsObstacle(curNode->i, adjacent_j) &&
                            map.CellIsObstacle(adjacent_i, curNode->j))
                            continue;
                    }
                }
                if (CLOSE.find((adjacent_i) * map.getMapWidth() + adjacent_j) != CLOSE.end()) {
                    double new_g = CLOSE[adjacent_i * map.getMapWidth() + adjacent_j].g +
                                   distance(adjacent_i, adjacent_j, curNode->i, curNode->j);
                    if (new_g < min_g || (new_g == min_g && newParent != nullptr &&
                            (CLOSE[adjacent_i * map.getMapWidth() + adjacent_j].F < newParent->F))) {
                        newParent = &CLOSE[adjacent_i * map.getMapWidth() + adjacent_j];
                        min_g = CLOSE[adjacent_i * map.getMapWidth() + adjacent_j].g +
                                distance(adjacent_i, adjacent_j, curNode->i, curNode->j);
                    }
                }
            }
        }
    }
    return newParent;
}

void LazyThetaSearch::resetParent(Node *current, Node parent, const Map &map, const EnvironmentOptions &options) {
    double new_g = parent.parent->g + distance(parent.parent->i, parent.parent->j, current->i, current->j);
    if (new_g < current->g) {
        current->g = new_g;
        current->parent = parent.parent;
    }
}
