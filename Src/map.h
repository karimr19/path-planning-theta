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
};

#endif

