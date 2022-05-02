#ifndef MAP_H
#define	MAP_H
#include <iostream>
#include "Constants.h"
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Tinyxml2.h"

//That's the class that stores BOTH grid map data AND start-goal locations.
//getValue reads the input XML and fills the Map object.
//Some naive checks are already included in getValue but still on some corrupted XMLs it may fail,
//so be sure to invoke it passing the correct XML (e.g. with correctly filled map tag, e.g. <map>.

//Map is NOT to be modified during the search. It should be passed as a const pointer.
//Think of it as an "independent" piece of data that is managed by outer (non-search related) proccesses.
//ThetaSearch algorithm should create it own object/structures needed to run the search on that map.

class Map
{
private:
    double  cellSize;
    int**   Grid;
    int height;
    int start_i;
    int start_j;
    int goal_i;
    int goal_j;
    int width;
    int agent_radius_in_cells;

public:
    Map();
    ~Map();

    bool getMap(const char *FileName);
    bool CellIsTraversable (int i, int j) const;
    bool CellOnGrid (int i, int j) const;
    bool CellIsObstacle(int i, int j) const;
    int  getValue(int i, int j) const;
    int getMapHeight() const;
    int getMapWidth() const;
    double getCellSize() const;
    int getStartI() const;
    int getStartJ() const;
    int getGoalI() const;
    int getGoalJ() const;
    bool isLeftUpperCorner(int i, int j);
    bool isRightUpperCorner(int i, int j);
    bool isLeftDownCorner(int i, int j);
    bool isRightDownCorner(int i, int j);
    bool isLeftTop(int i, int j);
    bool isRightTop(int i, int j);
    bool isUpperTop(int i, int j);
    bool isDownTop(int i, int j);
    bool isAloneObstacle(int i, int j);
    bool isLeftAndRightNotObstacle(int i, int j);
    bool isUpAndDownNotObstacle(int i, int j);
    bool isLeftNotObstacle(int i, int j);
    bool isUpNotObstacle(int i, int j);
    bool isRightNotObstacle(int i, int j);
    bool isDownNotObstacle(int i, int j);
    void expandObstacles(int begin_i, int begin_j, int end_i, int end_j);

    void expandObstacles();
};

#endif

