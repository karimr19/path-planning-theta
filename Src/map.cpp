#include "Map.h"


Map::Map() {
    height = -1;
    width = -1;
    start_i = -1;
    start_j = -1;
    goal_i = -1;
    goal_j = -1;
    Grid = nullptr;
    cellSize = 1;
}

Map::~Map() {
    if (Grid) {
        for (int i = 0; i < height; ++i)
            delete[] Grid[i];
        delete[] Grid;
    }
}

bool Map::CellIsTraversable(int i, int j) const {
    return (Grid[i][j] == CN_GC_NOOBS);
}

bool Map::CellIsObstacle(int i, int j) const {
    return (Grid[i][j] != CN_GC_NOOBS);
}

bool Map::CellOnGrid(int i, int j) const {
    return (i < height && i >= 0 && j < width && j >= 0);
}

bool Map::getMap(const char *FileName) {
    int rowiter = 0, grid_i = 0, grid_j = 0;

    tinyxml2::XMLElement *root = 0, *map = 0, *element = 0, *mapnode;

    std::string value;
    std::stringstream stream;

    bool hasGridMem = false, hasGrid = false, hasHeight = false, hasWidth = false, hasSTX = false, hasSTY = false,
            hasFINX = false, hasFINY = false, hasCellSize = false, hasRadius = false;

    tinyxml2::XMLDocument doc;

    // Load XML File
    if (doc.LoadFile(FileName) != tinyxml2::XMLError::XML_SUCCESS) {
        std::cout << "Error opening XML file!" << std::endl;
        return false;
    }
    // Get ROOT element
    root = doc.FirstChildElement(CNS_TAG_ROOT);
    if (!root) {
        std::cout << "Error! No '" << CNS_TAG_ROOT << "' tag found in XML file!" << std::endl;
        return false;
    }

    // Get MAP element
    map = root->FirstChildElement(CNS_TAG_MAP);
    if (!map) {
        std::cout << "Error! No '" << CNS_TAG_MAP << "' tag found in XML file!" << std::endl;
        return false;
    }

    for (mapnode = map->FirstChildElement(); mapnode; mapnode = mapnode->NextSiblingElement()) {
        element = mapnode->ToElement();
        value = mapnode->Value();
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);

        stream.str("");
        stream.clear();

        if (value != CNS_TAG_GRID) {
            stream << element->GetText();
        }


        if (!hasGridMem && hasHeight && hasWidth) {
            Grid = new int *[height];
            for (int i = 0; i < height; ++i)
                Grid[i] = new int[width];
            hasGridMem = true;
        }

        if (value == CNS_TAG_HEIGHT) {
            if (hasHeight) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_HEIGHT << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_HEIGHT << "' =" << height << "will be used."
                          << std::endl;
            } else {
                if (!((stream >> height) && (height > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_HEIGHT
                              << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_HEIGHT << "' tag should be an integer >=0" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_HEIGHT
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasHeight = true;
            }
        } else if (value == CNS_TAG_WIDTH) {
            if (hasWidth) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_WIDTH << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_WIDTH << "' =" << width << "will be used." << std::endl;
            } else {
                if (!((stream >> width) && (width > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_WIDTH
                              << "' tag encountered (or could not convert to integer)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_WIDTH << "' tag should be an integer AND >0" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_WIDTH
                              << "' tag will be encountered later..." << std::endl;

                } else
                    hasWidth = true;
            }
        } else if (value == CNS_TAG_RADIUS) {
            if (!hasWidth) {
                std::cout << "Error! '" << CNS_TAG_RADIUS << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag."
                          << std::endl;
                return false;
            }
            if (!hasHeight) {
                std::cout << "Error! '" << CNS_TAG_RADIUS << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag."
                          << std::endl;
                return false;
            }
            if (hasRadius) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_RADIUS << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_RADIUS << "' =" << agent_radius_in_cells
                          << "will be used."
                          << std::endl;
            } else if (!(stream >> agent_radius_in_cells && agent_radius_in_cells >= 0 &&
                         agent_radius_in_cells < std::min(width, height) / 2)) {
                std::cout << "Warning! Invalid value of '" << CNS_TAG_RADIUS
                          << "' tag encountered (or could not convert to integer)" << std::endl;
                std::cout << "Value of '" << CNS_TAG_RADIUS << "' tag should be an integer AND >=0 AND < min('"
                          << CNS_TAG_WIDTH << "', '" << CNS_TAG_HEIGHT << "') / 2 value, which is "
                          << std::min(width, height) / 2 << std::endl;
                std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_RADIUS
                          << "' tag will be encountered later..." << std::endl;
            } else {
                hasRadius = true;
            }
        } else if (value == CNS_TAG_CELLSIZE) {
            if (hasCellSize) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_CELLSIZE << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_CELLSIZE << "' =" << cellSize << "will be used."
                          << std::endl;
            } else {
                if (!((stream >> cellSize) && (cellSize > 0))) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_CELLSIZE
                              << "' tag encountered (or could not convert to double)." << std::endl;
                    std::cout << "Value of '" << CNS_TAG_CELLSIZE
                              << "' tag should be double AND >0. By default it is defined to '1'" << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_CELLSIZE
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasCellSize = true;
            }
        } else if (value == CNS_TAG_STX) {
            if (!hasWidth) {
                std::cout << "Error! '" << CNS_TAG_STX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag."
                          << std::endl;
                return false;
            }

            if (hasSTX) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STX << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STX << "' =" << start_j << "will be used." << std::endl;
            } else {
                if (!(stream >> start_j && start_j >= 0 && start_j < width)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_STX
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_STX << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_WIDTH << "' value, which is " << width << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_STX
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasSTX = true;
            }
        } else if (value == CNS_TAG_STY) {
            if (!hasHeight) {
                std::cout << "Error! '" << CNS_TAG_STY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag."
                          << std::endl;
                return false;
            }

            if (hasSTY) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_STY << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_STY << "' =" << start_i << "will be used." << std::endl;
            } else {
                if (!(stream >> start_i && start_i >= 0 && start_i < height)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_STY
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_STY << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_HEIGHT << "' value, which is " << height << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_STY
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasSTY = true;
            }
        } else if (value == CNS_TAG_FINX) {
            if (!hasWidth) {
                std::cout << "Error! '" << CNS_TAG_FINX << "' tag encountered before '" << CNS_TAG_WIDTH << "' tag."
                          << std::endl;
                return false;
            }

            if (hasFINX) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINX << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINX << "' =" << goal_j << "will be used." << std::endl;
            } else {
                if (!(stream >> goal_j && goal_j >= 0 && goal_j < width)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_FINX
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_FINX << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_WIDTH << "' value, which is " << width << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_FINX
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasFINX = true;
            }
        } else if (value == CNS_TAG_FINY) {
            if (!hasHeight) {
                std::cout << "Error! '" << CNS_TAG_FINY << "' tag encountered before '" << CNS_TAG_HEIGHT << "' tag."
                          << std::endl;
                return false;
            }

            if (hasFINY) {
                std::cout << "Warning! Duplicate '" << CNS_TAG_FINY << "' encountered." << std::endl;
                std::cout << "Only first value of '" << CNS_TAG_FINY << "' =" << goal_i << "will be used." << std::endl;
            } else {
                if (!(stream >> goal_i && goal_i >= 0 && goal_i < height)) {
                    std::cout << "Warning! Invalid value of '" << CNS_TAG_FINY
                              << "' tag encountered (or could not convert to integer)" << std::endl;
                    std::cout << "Value of '" << CNS_TAG_FINY << "' tag should be an integer AND >=0 AND < '"
                              << CNS_TAG_HEIGHT << "' value, which is " << height << std::endl;
                    std::cout << "Continue reading XML and hope correct value of '" << CNS_TAG_FINY
                              << "' tag will be encountered later..." << std::endl;
                } else
                    hasFINY = true;
            }
        } else if (value == CNS_TAG_GRID) {
            hasGrid = true;
            if (!(hasHeight && hasWidth)) {
                std::cout << "Error! No '" << CNS_TAG_WIDTH << "' tag or '" << CNS_TAG_HEIGHT << "' tag before '"
                          << CNS_TAG_GRID << "'tag encountered!" << std::endl;
                return false;
            }
            element = mapnode->FirstChildElement();
            while (grid_i < height) {
                if (!element) {
                    std::cout << "Error! Not enough '" << CNS_TAG_ROW << "' tags inside '" << CNS_TAG_GRID << "' tag."
                              << std::endl;
                    std::cout << "Number of '" << CNS_TAG_ROW
                              << "' tags should be equal (or greater) than the value of '" << CNS_TAG_HEIGHT
                              << "' tag which is " << height << std::endl;
                    return false;
                }
                std::string str = element->GetText();
                std::vector<std::string> elems;
                std::stringstream ss(str);
                std::string item;
                while (std::getline(ss, item, ' '))
                    elems.push_back(item);
                rowiter = grid_j = 0;
                int val;
                if (elems.size() > 0)
                    for (grid_j = 0; grid_j < width; ++grid_j) {
                        if (grid_j == elems.size())
                            break;
                        stream.str("");
                        stream.clear();
                        stream << elems[grid_j];
                        stream >> val;
                        Grid[grid_i][grid_j] = val;
                    }

                if (grid_j != width) {
                    std::cout << "Invalid value on " << CNS_TAG_GRID << " in the " << grid_i + 1 << " " << CNS_TAG_ROW
                              << std::endl;
                    return false;
                }
                ++grid_i;

                element = element->NextSiblingElement();
            }
        }
    }
    //some additional checks
    if (!hasGrid) {
        std::cout << "Error! There is no tag 'grid' in xml-file!\n";
        return false;
    }
    if (!(hasFINX && hasFINY && hasSTX && hasSTY))
        return false;

    if (Grid[start_i][start_j] != CN_GC_NOOBS) {
        std::cout << "Error! Start cell is not traversable (cell's value is" << Grid[start_i][start_j] << ")!"
                  << std::endl;
        return false;
    }

    if (Grid[goal_i][goal_j] != CN_GC_NOOBS) {
        std::cout << "Error! Goal cell is not traversable (cell's value is" << Grid[goal_i][goal_j] << ")!"
                  << std::endl;
        return false;
    }
    if (!hasRadius) {
        std::cout << "Warning! No '" << CNS_TAG_RADIUS << "' tag found." << std::endl;
        std::cout << "Value of '" << CNS_TAG_RADIUS << "' was defined to 0." << std::endl;
        agent_radius_in_cells = 0;
    }
    if (agent_radius_in_cells > 0) {
        expandObstacles();
    }
    if (Grid[start_i][start_j] != CN_GC_NOOBS) {
        std::cout << "Error! Start cell is not traversable (cell's value is" << Grid[start_i][start_j] << ")!"
                  << std::endl;
        return false;
    }

    if (Grid[goal_i][goal_j] != CN_GC_NOOBS) {
        std::cout << "Error! Goal cell is not traversable (cell's value is" << Grid[goal_i][goal_j] << ")!"
                  << std::endl;
        return false;
    }
    return true;
}

void Map::expandObstacles() {
    std::cout << "Map obstacles expanding began!\n";
    int begin_i, begin_j, end_i, end_j;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (getValue(i, j) == 1) {
                if (isAloneObstacle(i, j)) {
                    begin_i = std::max(0, i - agent_radius_in_cells);
                    begin_j = std::max(0, j - agent_radius_in_cells);
                    end_i = std::min(height - 1, i + agent_radius_in_cells);
                    end_j = std::min(width - 1, j + agent_radius_in_cells);
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isUpperTop(i, j)) {
                    begin_i = std::max(0, i - agent_radius_in_cells);
                    begin_j = std::max(0, j - agent_radius_in_cells);
                    end_i = i;
                    end_j = std::min(width - 1, j + agent_radius_in_cells);
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isDownTop(i, j)) {
                    begin_i = i;
                    begin_j = std::max(0, j - agent_radius_in_cells);
                    end_i = std::min(height - 1, i + agent_radius_in_cells);
                    end_j = std::min(width - 1, j + agent_radius_in_cells);
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isLeftTop(i, j)) {
                    begin_i = std::max(0, i - agent_radius_in_cells);
                    begin_j = std::max(0, j - agent_radius_in_cells);
                    end_i = std::min(height - 1, i + agent_radius_in_cells);
                    end_j = j;
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isRightTop(i, j)) {
                    begin_i = std::max(0, i - agent_radius_in_cells);
                    begin_j = j;
                    end_i = std::min(height - 1, i + agent_radius_in_cells);
                    end_j = std::min(width - 1, j + agent_radius_in_cells);
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isLeftUpperCorner(i, j)) {
                    begin_i = std::max(0, i - agent_radius_in_cells);
                    begin_j = std::max(0, j - agent_radius_in_cells);
                    end_i = i;
                    end_j = j;
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isRightUpperCorner(i, j)) {
                    begin_i = std::max(0, i - agent_radius_in_cells);
                    begin_j = j;
                    end_i = i;
                    end_j = std::min(width - 1, j + agent_radius_in_cells);
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isLeftDownCorner(i, j)) {
                    begin_i = i;
                    begin_j = std::max(0, j - agent_radius_in_cells);
                    end_i = std::min(height - 1, i + agent_radius_in_cells);
                    end_j = j;
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isRightDownCorner(i, j)) {
                    begin_i = i;
                    begin_j = j;
                    end_i = std::min(height - 1, i + agent_radius_in_cells);
                    end_j = std::min(width - 1, j + agent_radius_in_cells);
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isLeftAndRightNotObstacle(i, j)) {
                    begin_i = i;
                    begin_j = std::max(0, j - agent_radius_in_cells);
                    end_i = i;
                    end_j = std::min(width - 1, j + agent_radius_in_cells);
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isUpAndDownNotObstacle(i, j)) {
                    begin_i = std::max(0, i - agent_radius_in_cells);
                    begin_j = j;
                    end_i = std::min(height - 1, i + agent_radius_in_cells);
                    end_j = j;
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isLeftNotObstacle(i, j)) {
                    begin_i = i;
                    begin_j = std::max(0, j - agent_radius_in_cells);
                    end_i = i;
                    end_j = j;
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isRightNotObstacle(i, j)) {
                    begin_i = i;
                    begin_j = j;
                    end_i = i;
                    end_j = std::min(width - 1, j + agent_radius_in_cells);
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isUpNotObstacle(i, j)) {
                    begin_i = std::max(0, i - agent_radius_in_cells);
                    begin_j = j;
                    end_i = i;
                    end_j = j;
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                } else if (isDownNotObstacle(i, j)) {
                    begin_i = i;
                    begin_j = j;
                    end_i = std::min(height - 1, i + agent_radius_in_cells);
                    end_j = j;
                    expandObstacles(begin_i, begin_j, end_i, end_j);
                }
            }
        }
    }
    std::cout << "Map obstacles expanding finished!\n";
}


int Map::getValue(int i, int j) const {
    if (i < 0 || i >= height)
        return -1;

    if (j < 0 || j >= width)
        return -1;

    return Grid[i][j];
}

int Map::getMapHeight() const {
    return height;
}

int Map::getMapWidth() const {
    return width;
}

double Map::getCellSize() const {
    return cellSize;
}

int Map::getStartI() const {
    return start_i;
}

int Map::getStartJ() const {
    return start_j;
}

int Map::getGoalI() const {
    return goal_i;
}

int Map::getGoalJ() const {
    return goal_j;
}

bool Map::isLeftUpperCorner(int i, int j) {
    return i > 0 && j > 0 && Grid[i - 1][j] == 0 && Grid[i][j - 1] == 0;
}

bool Map::isRightUpperCorner(int i, int j) {
    return i > 0 && j < width - 1 && Grid[i - 1][j] == 0 && Grid[i][j + 1] == 0;
}

bool Map::isLeftTop(int i, int j) {
    return i > 0 && j > 0 && i < height - 1 && Grid[i - 1][j] == 0 && Grid[i][j - 1] == 0 &&
           Grid[i + 1][j] == 0;
}

bool Map::isRightTop(int i, int j) {
    return i > 0 && j < width - 1 && i < height - 1 && Grid[i - 1][j] == 0 &&
           Grid[i + 1][j] == 0 && Grid[i][j + 1] == 0;
}

bool Map::isAloneObstacle(int i, int j) {
    return i > 0 && j > 0 && i < height - 1 && j < width - 1 && Grid[i - 1][j] == 0 &&
           Grid[i + 1][j] == 0 && Grid[i][j - 1] == 0 && Grid[i][j + 1] == 0;
}

bool Map::isUpperTop(int i, int j) {
    return i > 0 && j > 0 && j < width - 1 && Grid[i - 1][j] == 0 && Grid[i][j - 1] == 0 &&
           Grid[i][j + 1] == 0;
}

bool Map::isDownTop(int i, int j) {
    return j > 0 && j < width - 1 && i < height - 1 && Grid[i][j - 1] == 0 &&
           Grid[i + 1][j] == 0 && Grid[i][j + 1] == 0;
}

bool Map::isLeftDownCorner(int i, int j) {
    return j > 0 && i < height - 1 && Grid[i + 1][j] == 0 && Grid[i][j - 1] == 0;
}

bool Map::isRightDownCorner(int i, int j) {
    return j < width - 1 && i < height - 1 && Grid[i + 1][j] == 0 && Grid[i][j + 1] == 0;
}

void Map::expandObstacles(int begin_i, int begin_j, int end_i, int end_j) {
    for (int i = begin_i; i <= end_i; ++i) {
        for (int j = begin_j; j <= end_j; ++j) {
            if (Grid[i][j] == 0) {
                Grid[i][j] = 2;
            }
        }
    }
}

bool Map::isLeftAndRightNotObstacle(int i, int j) {
    return j > 0 && j < width - 1 && Grid[i][j - 1] == 0 && Grid[i][j + 1] == 0;
}

bool Map::isUpAndDownNotObstacle(int i, int j) {
    return i > 0 && i < height - 1 && Grid[i - 1][j] == 0 && Grid[i + 1][j] == 0;
}

bool Map::isLeftNotObstacle(int i, int j) {
    return j > 0 && Grid[i][j - 1] == 0;
}

bool Map::isUpNotObstacle(int i, int j) {
    return i > 0 && Grid[i - 1][j] == 0;
}

bool Map::isRightNotObstacle(int i, int j) {
    return j < width - 1 && Grid[i][j + 1] == 0;
}

bool Map::isDownNotObstacle(int i, int j) {
    return i < height - 1 && Grid[i + 1][j] == 0;
}
