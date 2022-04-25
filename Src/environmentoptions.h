#ifndef ENVIRONMENTOPTIONS_H
#define ENVIRONMENTOPTIONS_H
#include "Constants.h"

class EnvironmentOptions
{
public:
    EnvironmentOptions(bool allow_squeeze, bool allow_diagonal, bool cut_corners, int metric_type, int search_type,
                       double h_weight);
    EnvironmentOptions();
    int     metric_type;     //Can be chosen Euclidean, Manhattan, Chebyshev and Diagonal distance
    bool    allow_squeeze;   //Option that allows to move throught "bottleneck"
    bool    allow_diagonal;  //Option that allows to make diagonal moves
    bool    cut_corners;     //Option that allows to make diagonal moves, when one adjacent cell is untraversable
    int     search_type;
    double  h_weight;        // Weight of heuristic when calculating f-value.
};

#endif // ENVIRONMENTOPTIONS_H
