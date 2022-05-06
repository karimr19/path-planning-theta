#include "ThetaSearch.h"

ThetaSearch::ThetaSearch() {
    h_weight = 1;
    open_size = 0;
    search_result.path_found = false;
    search_result.number_of_steps = 0;
}

ThetaSearch::~ThetaSearch() = default;

bool ThetaSearch::stopSearch() const{
    return open_size == 0;
}

SearchResult ThetaSearch::startSearch(const Map &map, const EnvironmentOptions &options) {
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
            if (!is_astar) {
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
//        outputResultsToFile(map);
    }
    search_result.secondary_path = &secondary_path;
    search_result.primary_path = &primary_path;
    return search_result;
}

Node ThetaSearch::getNodeWithMinF() {
    Node min_node;
    double min_f = std::numeric_limits<double>::infinity();
    for (auto &nodes_list : OPEN) {
        if ((!nodes_list.second.empty()) && (nodes_list.second.begin()->F <= min_f)) {
            min_f = nodes_list.second.begin()->F;
            min_node = *nodes_list.second.begin();
        }
    }
    return min_node;
}

std::list<Node> ThetaSearch::getSuccessors(Node node_to_expand, const Map &map, const EnvironmentOptions &options) {
    Node newNode;
    int cell_i = node_to_expand.i;
    int cell_j = node_to_expand.j;
    std::list<Node> successors;
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
                            if (map.getValue(cell_i + i, cell_j) != 0 && map.getValue(cell_i, cell_j + j) != 0) {
                                if (!options.allow_squeeze) {
                                    continue;
                                }
                            } else if (map.getValue(cell_i + i, cell_j) != 0 || map.getValue(cell_i, cell_j + j) != 0) {
                                if (!options.cut_corners) {
                                    continue;
                                }
                            }
                        } else {
                            continue;
                        }
                    }
                    if (CLOSE.find(nodeHash(adjacent_cell_i, adjacent_cell_j, map)) == CLOSE.end()) {
                        newNode.i = adjacent_cell_i;
                        newNode.j = adjacent_cell_j;
                        if (i == 0 || j == 0) {
                            newNode.g = node_to_expand.g + 1;
                        } else {
                            newNode.g = node_to_expand.g + std::sqrt(2);
                        }
                        successors.push_front(newNode);
                    }
                }
            }
        }
    }
    return successors;
}

void ThetaSearch::makePrimaryPath(Node last_node) {
    Node current = last_node;
    while (current.parent) {
        primary_path.push_front(current);
        current = *current.parent;
    }
    primary_path.push_front(current);
}

void ThetaSearch::makeSecondaryPath() {
    auto iter = primary_path.begin();
    int current_i, current_j, next_i, next_j, d_i, d_j;
    secondary_path.push_back(*iter);
    while (iter != --primary_path.end()) {
        current_i = iter->i;
        current_j = iter->j;
        ++iter;
        next_i = iter->i;
        next_j = iter->j;
        d_i = next_i - current_i;
        d_j = next_j - current_j;
        ++iter;
        if ((iter->i - next_i) != d_i || (iter->j - next_j) != d_j) {
            --iter;
            secondary_path.push_back(*iter);
        } else {
            --iter;
        }
    }
}

void ThetaSearch::insertOpen(Node new_node) {
    if (OPEN[new_node.i].empty()) {
        OPEN[new_node.i].push_back(new_node);
        open_size++;
        return;
    }
    auto insert_position = OPEN[new_node.i].end();
    bool position_found = false;
    double epsilon = 0.0001;
    for (auto it = OPEN[new_node.i].begin(); it != OPEN[new_node.i].end(); ++it) {
        if (!position_found && (it->F >= new_node.F)) {
            if (it->F == new_node.F) {
                if (new_node.g >= it->g) {
                    insert_position = it;
                    position_found = true;
                }
            } else {
                insert_position = it;
                position_found = true;
            }
        }
        if (it->j == new_node.j) {
            if (new_node.F >= it->F)
                return;
            else {
                if (insert_position == it) {
                    it->F = new_node.F;
                    it->g = new_node.g;
                    it->parent = new_node.parent;
                    return;
                }
                OPEN[new_node.i].erase(it);
                open_size--;
                break;
            }
        }
    }
    open_size++;
    OPEN[new_node.i].insert(insert_position, new_node);
}

bool ThetaSearch::lineOfSight(Node *first, Node *second, const Map &map, const EnvironmentOptions &options) {
    int d_i = std::abs(first->i - second->i);
    int d_j = std::abs(first->j - second->j);
    int step_i = 1;
    if (first->i > second->i) {
        step_i = -1;
    }
    int step_j = 1;
    if (first->j > second->j) {
        step_j = -1;
    }
    int error = 0;
    int current_i = first->i;
    int current_j = first->j;
    if (d_i == 0) {
        while (current_j != second->j) {
            if (map.cellIsObstacle(current_i, current_j)) {
                return false;
            }
            current_j += step_j;
        }
        return true;
    } else if (d_j == 0) {
        while (current_i != second->i) {
            if (map.cellIsObstacle(current_i, current_j)) {
                return false;
            }
            current_i += step_i;
        }
        return true;
    }
    if (options.cut_corners) {
        if (d_i > d_j) {
            while (current_i != second->i) {
                if (map.cellIsObstacle(current_i, current_j)) {
                    return false;
                }
                error += d_j;
                if ((error << 1) > d_i) {
                    if (((((error << 1) - d_j) < d_i) && map.cellIsObstacle(current_i + step_i, current_j)) ||
                        ((((error << 1) - d_j) > d_i) && map.cellIsObstacle(current_i, current_j + step_j))) {
                        return false;
                    }
                    current_j += step_j;
                    error -= d_i;
                }
                current_i += step_i;
            }
        } else {
            while (current_j != second->j) {
                if (map.cellIsObstacle(current_i, current_j)) {
                    return false;
                }
                error += d_i;
                if ((error << 1) > d_j) {
                    if (((((error << 1) - d_i) < d_j) && map.cellIsObstacle(current_i, current_j + step_j)) ||
                        (((error << 1) - d_i) > d_j && map.cellIsObstacle(current_i + step_i, current_j))) {
                        return false;
                    }
                    current_i += step_i;
                    error -= d_j;
                }
                current_j += step_j;
            }
        }
    } else {
        int sep_value = d_i * d_i + d_j * d_j;
        if (d_i > d_j) {
            while (current_i != second->i) {
                if (map.cellIsObstacle(current_i, current_j) || map.cellIsObstacle(current_i, current_j + step_j)) {
                    return false;
                }
                error += d_j;
                if (error >= d_i) {
                    if ((((error << 1) - d_i - d_j) * ((error << 1) - d_i - d_j) < sep_value) &&
                        (map.cellIsObstacle(current_i + step_i, current_j))) {
                        return false;
                    }
                    if (((3 * d_i - ((error << 1) - d_j)) * (3 * d_i - ((error << 1) - d_j)) < sep_value) &&
                        (map.cellIsObstacle(current_i, current_j + 2 * step_j))) {
                        return false;
                    }
                    current_j += step_j;
                    error -= d_i;
                }
                current_i += step_i;
            }
            if (map.cellIsObstacle(current_i, current_j)) {
                return false;
            }
        } else {
            while (current_j != second->j) {
                if (map.cellIsObstacle(current_i, current_j) || map.cellIsObstacle(current_i + step_i, current_j)) {
                    return false;
                }
                error += d_i;
                if (error >= d_j) {
                    if ((((error << 1) - d_i - d_j) * ((error << 1) - d_i - d_j) < sep_value) &&
                        (map.cellIsObstacle(current_i, current_j + step_j))) {
                        return false;
                    }
                    if (((3 * d_j - ((error << 1) - d_i)) * (3 * d_j - ((error << 1) - d_i)) < sep_value) &&
                            map.cellIsObstacle(current_i + 2 * step_i, current_j)) {
                        return false;
                    }
                    current_i += step_i;
                    error -= d_j;
                }
                current_j += step_j;
            }
            if (map.cellIsObstacle(current_i, current_j)) {
                return false;
            }
        }
    }
    return true;
}

void ThetaSearch::resetParent(Node *current, Node parent, const Map &map, const EnvironmentOptions &options) {
    if (!((parent.parent == nullptr) || (*current == *parent.parent))) {
        // Considering path 2.
        if (lineOfSight(parent.parent, current, map, options)) {
            current->g = parent.parent->g + distance(parent.parent->i, parent.parent->j, current->i, current->j);
            current->parent = parent.parent;
        }
    }
}

double ThetaSearch::distance(int i1, int j1, int i2, int j2) {
    return std::sqrt(std::pow(i1 - i2, 2) + std::pow(j1 - j2, 2));
}

double ThetaSearch::calculateHeuristic(int current_i, int current_j, const EnvironmentOptions &options, const Map &map) {
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

bool ThetaSearch::nodeIsFinish(Node *node, const Map &map) {
    return node->i == map.getGoalI() && node->j == map.getGoalJ();
}

uint64_t ThetaSearch::nodeHash(int i, int j, const Map &map) {
    return i * map.getMapWidth() + static_cast<uint64_t>(j);
}

void ThetaSearch::setIsAstar(bool astar) {
    this->is_astar = astar;
}
