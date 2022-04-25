#include "EnvironmentOptions.h"


EnvironmentOptions::EnvironmentOptions()
{
    metric_type = CN_SP_MT_EUCL;
    allow_squeeze = false;
    allow_diagonal = true;
    cut_corners = false;
    search_type = CN_SP_ST_ASTAR;
    h_weight = 1;
}

EnvironmentOptions::EnvironmentOptions(bool allow_squeeze, bool allow_diagonal, bool cut_corners, int metric_type,
                                       int search_type, double h_weight) {
    this->metric_type = metric_type;
    this->allow_squeeze = allow_squeeze;
    this->allow_diagonal = allow_diagonal;
    this->cut_corners = cut_corners;
    this->search_type = search_type;
    this->h_weight = h_weight;
}
