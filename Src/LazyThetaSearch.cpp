//
// Created by Karim on 18.04.2022.
//

#include "LazyThetaSearch.h"

LazyThetaSearch::LazyThetaSearch() {
    sresult.pathfound = false;
    sresult.nodescreated = 0;
    sresult.numberofsteps = 0;
    sresult.lppath = nullptr;
    sresult.hppath = nullptr;
    sresult.time = 0;
    sresult.pathlength = 0;
}

LazyThetaSearch::~LazyThetaSearch() {
    for (auto pair: nodes_container) {
        delete pair.second;
    }
}


SearchResult LazyThetaSearch::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options) {
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
    Node* last_node = nullptr;
    while (!OPEN.empty()) {
        ++number_of_steps;
        Node *node_to_expand = *OPEN.begin();
        setVertex(node_to_expand, map, options);
        OPEN.erase(OPEN.begin());
        if ((node_to_expand->i == goal_i) && (node_to_expand->j == goal_j)) {
            sresult.pathfound = true;
            last_node = node_to_expand;
            break;
        }
        for (auto&[successor_i, successor_j, is_successor_diagonal]: getSuccessors(node_to_expand->i,
                                                                                   node_to_expand->j,
                                                                                   map, options)) {
            checkSuccessorCell(node_to_expand, successor_i, successor_j, is_successor_diagonal, map, options);
        }
        CLOSED[std::make_pair(node_to_expand->i, node_to_expand->j)] = node_to_expand;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> executed_time = end - start;
    sresult.time = executed_time.count();
    sresult.numberofsteps = number_of_steps;
    sresult.nodescreated = OPEN.size() + CLOSED.size();
    if (sresult.pathfound) {
        makePrimaryPath(last_node);
        sresult.pathlength = last_node->g;
        makeSecondaryPath();
        sresult.hppath = &hppath;
        sresult.lppath = &lppath;
        outputResultsToFiles(map);
    }
    return sresult;
}

std::vector<std::tuple<int, int, bool>> LazyThetaSearch::getSuccessors(int cell_i, int cell_j, const Map &map,
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

void LazyThetaSearch::makePrimaryPath(Node *curNode) {
    lppath = std::list<Node>();
    lppath.push_front(*curNode);
    while (curNode->parent) {
        curNode = curNode->parent;
        lppath.push_front(*curNode);
    }
}

void LazyThetaSearch::makeSecondaryPath() {
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

void LazyThetaSearch::checkDirection(const std::list<Node>::iterator &currentNode, const std::list<Node>::iterator &previousNode,
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

void LazyThetaSearch::checkSuccessorCell(Node *node_to_expand, int successor_i, int successor_j, bool is_diagonal,
                                const Map &map, const EnvironmentOptions &options) {
    uint64_t successor_hash = nodeHash(successor_i, successor_j, map);
    double stepLength = kStraightStepLength;
    if (node_to_expand->parent) {
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

uint64_t LazyThetaSearch::nodeHash(int i, int j, const Map &map) {
    return static_cast<uint64_t>(i) * map.getMapWidth() + j;
}

double LazyThetaSearch::calculateHeuristic(int current_i, int current_j, const EnvironmentOptions &options, const Map &map) {
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

bool LazyThetaSearch::lineOfSight(Node *first, int successor_x, int successor_y, const Map &map, bool cut_corners) {
    int d_x = std::abs(first->j - successor_x);
    int d_y = std::abs(first->i - successor_y);
    int step_x = (first->j < successor_x ? 1 : -1);
    int step_y = (first->i < successor_y ? 1 : -1);
    int error = 0;
    int current_y = first->i;
    int current_x = first->j;
    if(d_x == 0) {
        while (current_x != successor_x) {
            if(map.CellIsObstacle(current_y, current_x)) {
                return false;
            }
            current_x += step_x;
        }
        return true;
    } else if (d_y == 0) {
        while (current_y != successor_y) {
            if(map.CellIsObstacle(current_y, current_x)) {
                return false;
            }
            current_y += step_y;
        }
        return true;
    }
    if(cut_corners) {
        if (d_y > d_x) {
            while (current_y != successor_y) {
                if (map.CellIsObstacle(current_y, current_x)) {
                    return false;
                }
                error += d_x;
                if ((error << 1) > d_y) {
                    if(((error << 1) - d_x) < d_y && map.CellIsObstacle(current_y + step_y, current_x)) {
                        return false;
                    } else if (((error << 1) - d_x) > d_y && map.CellIsObstacle(current_y, current_x + step_x)) {
                        return false;
                    }
                    current_x += step_x;
                    error -= d_y;
                }
                current_y += step_y;
            }
        } else {
            while (current_x != successor_x) {
                if (map.CellIsObstacle(current_y, current_x)) {
                    return false;
                }
                error += d_y;
                if ((error << 1) > d_x) {
                    if(((error << 1) - d_y) < d_x && map.CellIsObstacle(current_y, current_x + step_x)) {
                        return false;
                    } else if(((error << 1) - d_y) > d_x && map.CellIsObstacle(current_y + step_y, current_x)) {
                        return false;
                    }
                    current_y += step_y;
                    error -= d_x;
                }
                current_x += step_x;
            }
        }

    } else {
        int sep_value = d_x * d_x + d_y * d_y;
        if (d_y > d_x) {
            while (current_y != successor_y) {
                if (map.CellIsObstacle(current_y, current_x)) {
                    return false;
                }
                if (map.CellIsObstacle(current_y, current_x + step_x)) {
                    return false;
                }
                error += d_x;
                if (error >= d_y) {
                    if(((error << 1) - d_y - d_x) * ((error << 1) - d_x - d_y) < sep_value) {
                        if(map.CellIsObstacle(current_y + step_y, current_x)) {
                            return false;
                        }
                    }
                    if((3 * d_y - ((error << 1) - d_x)) * (3 * d_y - ((error << 1) - d_x)) < sep_value) {
                        if(map.CellIsObstacle(current_y, current_x + 2 * step_x)) {
                            return false;
                        }
                    }
                    current_x += step_x;
                    error -= d_y;
                }
                current_y += step_y;
            }
            if(map.CellIsObstacle(current_y, current_x)) {
                return false;
            }
        } else {
            while (current_x != successor_x) {
                if(map.CellIsObstacle(current_y, current_x)) {
                    return false;
                }
                if(map.CellIsObstacle(current_y + step_y, current_x)) {
                    return false;
                }
                error += d_y;
                if(error >= d_x) {
                    if(((error << 1) - d_x - d_y) * ((error << 1) - d_x - d_y) < sep_value) {
                        if(map.CellIsObstacle(current_y, current_x + step_x)) {
                            return false;
                        }
                    }
                    if((3 * d_x - ((error << 1) - d_y)) * (3 * d_x - ((error << 1) - d_y)) < sep_value) {
                        if(map.CellIsObstacle(current_y + 2 * step_y, current_x)) {
                            return false;
                        }
                    }
                    current_y += step_y;
                    error -= d_x;
                }
                current_x += step_x;
            }
            if(map.CellIsObstacle(current_y, current_x))
                return false;
        }
    }
    return true;
}

double LazyThetaSearch::distance(int first_x, int first_y, int second_x, int second_y) {
    return std::sqrt(std::pow(first_x - second_x, 2) + std::pow(first_y - second_y, 2));
}

void LazyThetaSearch::outputResultsToFiles(const Map &map) {// Вывод для проверки работы
    std::ofstream output_stream("map.txt");
    output_stream << map.getMapHeight() << " " << map.getMapWidth() << '\n';
    for (int i = 0; i < map.getMapHeight(); ++i) {
        for (int j = 0; j < map.getMapWidth() - 1; ++j) {
            output_stream << map.getValue(i, j) << " ";
        }
        output_stream << map.getValue(i, map.getMapWidth() - 1) << '\n';
    }
    output_stream << lppath.size() << '\n';
    for (auto node : lppath) {

        output_stream << node.j << " " << node.i << '\n';
    }
    output_stream.close();
}

void LazyThetaSearch::setVertex(Node *node_to_expand, const Map &map, const EnvironmentOptions &options) {
    if (node_to_expand->parent) {
        if (!lineOfSight(node_to_expand->parent, node_to_expand->j, node_to_expand->i, map, options.cutcorners)) {
            Node* potential_parent = nullptr;
            double min_g = 1'000'000'000'000;
            double distance;
            double epsilon = 0.0001;
            for (auto&[successor_i, successor_j, is_successor_diagonal]: getSuccessors(node_to_expand->i,
                                                                                       node_to_expand->j,
                                                                                       map, options)) {
                if (CLOSED.contains(std::make_pair(successor_i, successor_j))) {
                    potential_parent = CLOSED[std::make_pair(successor_i, successor_j)];
                    distance = LazyThetaSearch::distance(successor_j, successor_i, node_to_expand->j, node_to_expand->i);
                    if (distance + potential_parent->g - min_g < -epsilon) {
                        min_g = distance + potential_parent->g;
                    } else {
                        potential_parent = nullptr;
                    }
                }
            }
            if (potential_parent) {
                node_to_expand->parent = potential_parent;
                node_to_expand->g = potential_parent->g + LazyThetaSearch::distance(node_to_expand->i, node_to_expand->j,
                                                                   potential_parent->i, potential_parent->j);
                node_to_expand->F = node_to_expand->g + node_to_expand->H;
            }
        }
    }
}

