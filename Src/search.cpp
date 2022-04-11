#include "search.h"

Search::Search() {
    sresult.pathfound = false;
    sresult.nodescreated = 0;
    sresult.numberofsteps = 0;
    sresult.lppath = nullptr;
    sresult.hppath = nullptr;
    sresult.time = 0;
    sresult.pathlength = 0;
}

Search::~Search() {
    for (auto pair: nodes_container) {
        delete pair.second;
    }
}


SearchResult Search::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
    int start_i = map.getStartI();
    int start_j = map.getStartJ();
    int goal_i = map.getGoalI();
    int goal_j = map.getGoalJ();
    int number_of_steps = 0;
    auto start = std::chrono::steady_clock::now();
    // Creates node with start coordinates.
    nodes_container[nodeHash(start_i, start_j, map)] =
            new Node(start_i, start_j, 0, calculateHeuristic(start_i, start_j, options, map),nullptr);
    OPEN.insert(nodes_container[nodeHash(start_i, start_j, map)]);
    while (!OPEN.empty()) {
        ++number_of_steps;
        Node *node_to_expand = *OPEN.begin();
        OPEN.erase(OPEN.begin());
        if ((node_to_expand->i == goal_i) && (node_to_expand->j == goal_j)) {
            sresult.pathfound = true;
            makePrimaryPath(node_to_expand);
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> executed_time = end - start;
            sresult.time = executed_time.count();
            sresult.pathlength = node_to_expand->g;
            break;
        }
        for (auto&[successor_i, successor_j, is_successor_diagonal]: getSuccessors(node_to_expand->i,
                                                                                   node_to_expand->j,
                                                                                   map, options)) {
            checkSuccessorCell(node_to_expand, successor_i, successor_j, is_successor_diagonal, map, options);
        }
        CLOSED[std::make_pair(node_to_expand->i, node_to_expand->j)] = node_to_expand;
    }
    sresult.numberofsteps = number_of_steps;
    sresult.nodescreated = OPEN.size() + CLOSED.size();
    if (sresult.pathfound) {
        makeSecondaryPath();
        sresult.hppath = &hppath;
        sresult.lppath = &lppath;
    }
    return sresult;
}

std::vector<std::tuple<int, int, bool>> Search::getSuccessors(int cell_i, int cell_j, const Map &map,
                                                              const EnvironmentOptions &options) {
    std::vector<std::tuple<int, int, bool>> successors = std::vector<std::tuple<int, int, bool>>();
    int adjacent_cell_i, adjacent_cell_j;
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            if ((i != 0) || (j != 0)) {
                adjacent_cell_i = cell_i + i;
                adjacent_cell_j = cell_j + j;
                if (map.getValue(adjacent_cell_i, adjacent_cell_j) == 0) {
                    // Checks if it is diagonal step.
                    if ((i + j) % 2 == 0) {
                        if (options.allowdiagonal) {
                            if (map.getValue(cell_i + i, cell_j) && map.getValue(cell_i, cell_j + j)) {
                                if (options.allowsqueeze) {
                                    successors.emplace_back(adjacent_cell_i, adjacent_cell_j, true);
                                }
                            } else if (map.getValue(cell_i + i, cell_j) || map.getValue(cell_i, cell_j + j)) {
                                if (options.cutcorners) {
                                    successors.emplace_back(adjacent_cell_i, adjacent_cell_j, true);
                                }
                            } else {
                                successors.emplace_back(adjacent_cell_i, adjacent_cell_j, true);
                            }
                        }
                    } else {
                        successors.emplace_back(adjacent_cell_i, adjacent_cell_j, false);
                    }
                }
            }
        }
    }
    return successors;
}

void Search::makePrimaryPath(Node *curNode) {
    lppath = std::list<Node>();
    lppath.push_front(*curNode);
    while (curNode->parent) {
        curNode = curNode->parent;
        lppath.push_front(*curNode);
    }
}

void Search::makeSecondaryPath() {
    hppath = std::list<Node>();
    hppath.push_back(lppath.front());
    auto lppath_front = lppath.begin();
    auto previousNode = lppath_front;
    std::advance(lppath_front, 1);
    if (lppath_front != lppath.end()) {
        Direction lastDirection;
        if ((previousNode->i - lppath_front->i) * (previousNode->j - lppath_front->j) > 0) {
            lastDirection = MAINDIAGONAL;
        } else if ((previousNode->i - lppath_front->i) * (previousNode->j - lppath_front->j) < 0) {
            lastDirection = SIDEDIAGONAL;
        } else if (previousNode->i != lppath_front->i) {
            lastDirection = VERTICAL;
        } else {
            lastDirection = HORIZONTAL;
        }
        previousNode = lppath_front;
        std::advance(lppath_front, 1);
        if (lppath_front != lppath.end()) {
            checkDirection(lppath_front, previousNode, lastDirection);
            previousNode = lppath_front;
            std::advance(lppath_front, 1);
            for (auto it = lppath_front; it != lppath.end(); ++it) {
                checkDirection(it, previousNode, lastDirection);
                previousNode = it;
            }
            hppath.push_back(lppath.back());
        } else {
            hppath.push_back(*previousNode);
        }
    }
}

void Search::checkDirection(const std::list<Node>::iterator &currentNode, const std::list<Node>::iterator &previousNode,
                            Direction &lastDirection) {
    if ((previousNode->i - currentNode->i) * (previousNode->j - currentNode->j) > 0) {
        if (lastDirection != MAINDIAGONAL) {
            lastDirection = MAINDIAGONAL;
            hppath.push_back(*previousNode);
        }
    } else if ((previousNode->i - currentNode->i) * (previousNode->j - currentNode->j) < 0) {
        if (lastDirection != SIDEDIAGONAL) {
            lastDirection = SIDEDIAGONAL;
            hppath.push_back(*previousNode);
        }
    } else if (previousNode->i != currentNode->i) {
        if (lastDirection != VERTICAL) {
            lastDirection = VERTICAL;
            hppath.push_back(*previousNode);
        }
    } else {
        if (lastDirection != HORIZONTAL) {
            lastDirection = HORIZONTAL;
            hppath.push_back(*previousNode);
        }
    }
}

void Search::checkSuccessorCell(Node *node_to_expand, int successor_i, int successor_j, bool is_diagonal,
                                const Map &map, const EnvironmentOptions &options) {
    uint64_t successor_hash = nodeHash(successor_i, successor_j, map);
    double stepLength = kStraightStepLength;
//    if (is_diagonal) {
//        stepLength = CN_SQRT_TWO;
//    }
    if (node_to_expand->parent && lineOfSight(node_to_expand->parent, successor_j, successor_i, map, options)) {
        if (!nodes_container[successor_hash]) {
            nodes_container[successor_hash] = new Node(successor_i, successor_j, node_to_expand->parent->g +
                                                                                 distance(node_to_expand->parent->j, node_to_expand->parent->i, successor_j, successor_i),
                                                       calculateHeuristic(successor_i, successor_j, options, map), node_to_expand->parent);
            OPEN.insert(nodes_container[successor_hash]);
        } else {
            // If it is not in CLOSED, it is in OPEN.
            if (!CLOSED.contains(std::make_pair(successor_i, successor_j))) {
                Node *successor = nodes_container[successor_hash];
                double parent_successor_distance = distance(node_to_expand->parent->j, node_to_expand->parent->i,
                                                            successor->j, successor->i);
                if (node_to_expand->parent->g + parent_successor_distance < successor->g) {
                    OPEN.erase(successor);
                    successor->g = node_to_expand->parent->g + parent_successor_distance;
                    successor->F = successor->g + successor->H;
                    successor->parent = node_to_expand->parent;
                    OPEN.insert(successor);
                }
            }
        }
    } else {
        if (!nodes_container[successor_hash]) {
            nodes_container[successor_hash] = new Node(successor_i, successor_j, node_to_expand->g +
                                                                                 distance(node_to_expand->j, node_to_expand->i, successor_j, successor_i),
                                                       calculateHeuristic(successor_i, successor_j, options, map), node_to_expand);
            OPEN.insert(nodes_container[successor_hash]);
        } else {
            // If it is not in CLOSED, it is in OPEN.
            if (!CLOSED.contains(std::make_pair(successor_i, successor_j))) {
                Node *successor = nodes_container[successor_hash];
                double node_to_expand_successor_distance = distance(node_to_expand->j, node_to_expand->i,
                                                                    successor_j, successor_i);
                if (node_to_expand->g + node_to_expand_successor_distance < successor->g) {
                    OPEN.erase(successor);
                    successor->g = node_to_expand->g + node_to_expand_successor_distance;
                    successor->F = successor->g + successor->H;
                    OPEN.insert(successor);
                }
            }
        }
    }
}

uint64_t Search::nodeHash(int i, int j, const Map &map) {
    return static_cast<uint64_t>(i) * map.getMapWidth() + j;
}

double Search::calculateHeuristic(int current_i, int current_j, const EnvironmentOptions &options, const Map &map) {
    int metric_type = options.metrictype;
    int dx = std::abs(map.getGoalI() - current_i);
    int dy = std::abs(map.getGoalJ() - current_j);
    if (metric_type == CN_SP_MT_DIAG) {
        return (CN_SQRT_TWO * std::min(dx, dy) + std::abs(dx - dy)) * options.hweight;
    } else if (metric_type == CN_SP_MT_MANH) {
        return (dx + dy) * options.hweight;
    } else if (metric_type == CN_SP_MT_EUCL) {
        return (std::sqrt(std::pow(dx, 2) + std::pow(dy, 2))) * options.hweight;
    } else if (metric_type == CN_SP_MT_CHEB) {
        return std::max(dx, dy) * options.hweight;
    }
    return 0;
}

bool raytrace(Node* first, int successor_x, int successor_y, const Map &map) {
    int dx = std::abs(successor_x - first->j);
    int dy = abs(successor_y - first->i);
    int x = first->j;
    int y = first->i;
    int n = 1 + dx + dy;
    int x_inc = (successor_x > first->j) ? 1 : -1;
    int y_inc = (successor_y > first->i) ? 1 : -1;
    int error = dx - dy;
    dx *= 2;
    dy *= 2;

    for (; n > 0; --n)
    {
        if (error > 0) {
            x += x_inc;
            error -= dy;
        } else {
            y += y_inc;
            error += dx;
        }
    }
}

bool Search::lineOfSight(Node *first, int successor_x, int successor_y, const Map &map,
                         const EnvironmentOptions &options) {
    int x_start = first->j;
    int y_start = first->i;
    int difference_x = successor_x - x_start;
    int difference_y = successor_y - y_start;
    int step_y, step_x;
    int f = 0;
    if (difference_y < 0) {
        difference_y = -difference_y;
        step_y = -1;
    } else {
        step_y = 1;
    }

    if (difference_x < 0) {
        difference_x = -difference_x;
        step_x = -1;
    } else {
        step_x = 1;
    }
    bool x_changed;
    bool y_changed;
    if (difference_x >= difference_y) {
        while (x_start != successor_x) {
            y_changed = false;
            f += difference_y;
            if (f >= difference_x) {
                if (!map.CellIsTraversable(y_start + (step_y - 1) / 2, x_start + (step_x - 1) / 2)) {
                    return false;
                }
                y_start += step_y;
                y_changed = true;
                f -= difference_x;
            }
            if ((f != 0) && !map.CellIsTraversable(y_start + (step_y - 1) / 2, x_start + (step_x - 1) / 2)) {
                return false;
            }
            if ((difference_y == 0) && !map.CellIsTraversable(y_start, x_start + (step_x - 1) / 2) &&
                !map.CellIsTraversable(y_start - 1, x_start + (step_x - 1) / 2)) {
                return false;
            }
            x_start += step_x;
            if (y_changed) {
                if (!options.allowsqueeze) {
                    if ((!map.CellIsTraversable(y_start - 1, x_start) &&
                         !map.CellIsTraversable(y_start, x_start - 1)) ||
                            (!map.CellIsTraversable(y_start - 1, x_start) &&
                             !map.CellIsTraversable(y_start, x_start - 1))) {
                        return false;
                    }
                }
                if (!options.cutcorners) {
                    if (!map.CellIsTraversable(y_start - 1, x_start) ||
                        !map.CellIsTraversable(y_start - 1, x_start - 1) ||
                        !map.CellIsTraversable(y_start, x_start - 1) || !map.CellIsTraversable(y_start, x_start)) {
                        return false;
                    }
                }
            }
        }
    } else {
        while (y_start != successor_y) {
            x_changed = false;
            f += difference_x;
            if (f >= difference_y) {
                if (!map.CellIsTraversable(y_start + (step_y - 1) / 2, x_start + (step_x - 1) / 2)) {
                    return false;
                }
                x_start += step_x;
                x_changed = true;
                f -= difference_y;
            }
            if ((f != 0) && !map.CellIsTraversable(y_start + (step_y - 1) / 2, x_start + (step_x - 1) / 2)) {
                return false;
            }
            if ((difference_y == 0) && !map.CellIsTraversable(y_start + (step_y - 1) / 2, x_start) &&
                !map.CellIsTraversable(y_start + (step_y - 1) / 2, x_start - 1)) {
                return false;
            }
            y_start += step_y;
            if (x_changed) {
                if (!options.allowsqueeze) {
                    if ((!map.CellIsTraversable(y_start - 1, x_start) &&
                         !map.CellIsTraversable(y_start, x_start - 1)) ||
                        (!map.CellIsTraversable(y_start - 1, x_start) &&
                         !map.CellIsTraversable(y_start, x_start - 1))) {
                        return false;
                    }
                }
                if (!options.cutcorners) {
                    if (!map.CellIsTraversable(y_start - 1, x_start) ||
                        !map.CellIsTraversable(y_start - 1, x_start - 1) ||
                        !map.CellIsTraversable(y_start, x_start - 1) || !map.CellIsTraversable(y_start, x_start)) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

double Search::distance(int first_x, int first_y, int second_x, int second_y) {
    return std::sqrt(std::pow(first_x - second_x, 2) + std::pow(first_y - second_y, 2));
}
