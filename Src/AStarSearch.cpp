//
// Created by Karim on 24.04.2022.
//

#include "AStarSearch.h"

AStarSearch::AStarSearch() {
    search_result.path_found = false;
    search_result.nodes_created = 0;
    search_result.number_of_steps = 0;
    search_result.primary_path = nullptr;
    search_result.secondary_path = nullptr;
    search_result.time = 0;
    search_result.path_length = 0;
}

AStarSearch::~AStarSearch() {
    for (auto pair: nodes_container) {
        delete pair.second;
    }
}


SearchResult AStarSearch::startSearch(const Map &map, const EnvironmentOptions &options) {
    h_weight = options.h_weight;
    int start_i = map.getStartI();
    int start_j = map.getStartJ();
    int goal_i = map.getGoalI();
    int goal_j = map.getGoalJ();
    int number_of_steps = 0;
    auto start = std::chrono::steady_clock::now();
    // Creates node with start coordinates.
    nodes_container[nodeHash(start_i, start_j, map)] =
            new Node(start_i, start_j, 0,h_weight * calculateHeuristic(start_i, start_j, options, map),
                     nullptr);
    OPEN.insert(nodes_container[nodeHash(start_i, start_j, map)]);
    while (!OPEN.empty()) {
        ++number_of_steps;
        Node *node_to_expand = *OPEN.begin();
        OPEN.erase(OPEN.begin());
        if ((node_to_expand->i == goal_i) && (node_to_expand->j == goal_j)) {
            search_result.path_found = true;
            makePrimaryPath(node_to_expand);
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> executed_time = end - start;
            search_result.time = executed_time.count();
            search_result.path_length = node_to_expand->g;
            break;
        }
        for (auto&[successor_i, successor_j, is_successor_diagonal]: getSuccessors(node_to_expand->i,
                                                                                   node_to_expand->j,
                                                                                   map, options)) {
            checkSuccessorCell(node_to_expand, successor_i, successor_j, is_successor_diagonal, map, options);
        }
        CLOSED[std::make_pair(node_to_expand->i, node_to_expand->j)] = node_to_expand;
    }
    search_result.number_of_steps = number_of_steps;
    search_result.nodes_created = OPEN.size() + CLOSED.size();
    if (search_result.path_found) {
        makeSecondaryPath();
        outputResultsToFiles(map);
        search_result.primary_path = &primary_path;
        search_result.secondary_path = &secondary_path;
    }
    return search_result;
}

std::vector<std::tuple<int, int, bool>> AStarSearch::getSuccessors(int cell_i, int cell_j, const Map &map,
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
                        if (options.allow_diagonal) {
                            if (map.getValue(cell_i + i, cell_j) && map.getValue(cell_i, cell_j + j)) {
                                if (options.allow_squeeze) {
                                    successors.emplace_back(adjacent_cell_i, adjacent_cell_j, true);
                                }
                            } else if (map.getValue(cell_i + i, cell_j) || map.getValue(cell_i, cell_j + j)) {
                                if (options.cut_corners) {
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

void AStarSearch::makePrimaryPath(Node *curNode) {
    primary_path = std::list<Node>();
    primary_path.push_front(*curNode);
    while (curNode->parent) {
        curNode = curNode->parent;
        primary_path.push_front(*curNode);
    }
}

void AStarSearch::makeSecondaryPath() {
    secondary_path = std::list<Node>();
    secondary_path.push_back(primary_path.front());
    auto lppath_front = primary_path.begin();
    auto previousNode = lppath_front;
    std::advance(lppath_front, 1);
    if (lppath_front != primary_path.end()) {
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
        if (lppath_front != primary_path.end()) {
            checkDirection(lppath_front, previousNode, lastDirection);
            previousNode = lppath_front;
            std::advance(lppath_front, 1);
            for (auto it = lppath_front; it != primary_path.end(); ++it) {
                checkDirection(it, previousNode, lastDirection);
                previousNode = it;
            }
            secondary_path.push_back(primary_path.back());
        } else {
            secondary_path.push_back(*previousNode);
        }
    }
}

void AStarSearch::checkDirection(const std::list<Node>::iterator &currentNode, const std::list<Node>::iterator &previousNode,
                            Direction &lastDirection) {
    if ((previousNode->i - currentNode->i) * (previousNode->j - currentNode->j) > 0) {
        if (lastDirection != MAINDIAGONAL) {
            lastDirection = MAINDIAGONAL;
            secondary_path.push_back(*previousNode);
        }
    } else if ((previousNode->i - currentNode->i) * (previousNode->j - currentNode->j) < 0) {
        if (lastDirection != SIDEDIAGONAL) {
            lastDirection = SIDEDIAGONAL;
            secondary_path.push_back(*previousNode);
        }
    } else if (previousNode->i != currentNode->i) {
        if (lastDirection != VERTICAL) {
            lastDirection = VERTICAL;
            secondary_path.push_back(*previousNode);
        }
    } else {
        if (lastDirection != HORIZONTAL) {
            lastDirection = HORIZONTAL;
            secondary_path.push_back(*previousNode);
        }
    }
}

void AStarSearch::checkSuccessorCell(Node *node_to_expand, int successor_i, int successor_j, bool is_diagonal,
                                const Map &map, const EnvironmentOptions &options) {
    uint64_t successor_hash = nodeHash(successor_i, successor_j, map);
    double stepLength = kStraightStepLength;
    if (is_diagonal) {
        stepLength = CN_SQRT_TWO;
    }
    if (!nodes_container[successor_hash]) {
        nodes_container[successor_hash] = new Node(successor_i, successor_j, node_to_expand->g + stepLength,
                                                   h_weight * calculateHeuristic(successor_i, successor_j, options, map),
                                                   node_to_expand);
        OPEN.insert(nodes_container[successor_hash]);
    } else {
        // If it is not in CLOSED, it is in OPEN.
        if (!CLOSED.contains(std::make_pair(successor_i, successor_j))) {
            Node *successor = nodes_container[successor_hash];
            if (node_to_expand->g + stepLength < successor->g) {
                OPEN.erase(successor);
                successor->g = node_to_expand->g + stepLength;
                successor->F = successor->g + successor->H;
                OPEN.insert(successor);
            }
        }
    }
}

uint64_t AStarSearch::nodeHash(int i, int j, const Map &map) {
    return static_cast<uint64_t>(i) * map.getMapWidth() + j;
}

double AStarSearch::calculateHeuristic(int current_i, int current_j, const EnvironmentOptions &options,
                                       const Map &map) {
    int metric_type = options.metric_type;
    int dx = std::abs(map.getGoalI() - current_i);
    int dy = std::abs(map.getGoalJ() - current_j);
    if (metric_type == CN_SP_MT_DIAG) {
        return (CN_SQRT_TWO * std::min(dx, dy) + std::abs(dx - dy));
    } else if (metric_type == CN_SP_MT_MANH) {
        return dx + dy;
    } else if (metric_type == CN_SP_MT_EUCL) {
        return (std::sqrt(std::pow(dx, 2) + std::pow(dy, 2)));
    } else if (metric_type == CN_SP_MT_CHEB) {
        return std::max(dx, dy);
    }
    return 0;
}

void AStarSearch::outputResultsToFiles(const Map &map) {// Вывод для проверки работы
    std::ofstream output_stream("map.txt");
    int min_i = 248;
    int min_j = 145;
    int max_i = 285;
    int max_j = 210;
    output_stream << max_i - min_i + 1 << " " << max_j - min_j + 1 << '\n';
    for (int i = min_i; i <= max_i; ++i) {
        for (int j = min_j; j <= max_j; ++j) {
            output_stream << map.getValue(i, j) << " ";
        }
        output_stream << map.getValue(i, map.getMapWidth() - 1) << '\n';
    }
    output_stream << secondary_path.size() << '\n';
    for (auto node : secondary_path) {
        output_stream << node.j - min_j << " " << node.i - min_i << '\n';
    }
    output_stream.close();
}
