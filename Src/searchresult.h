#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H
#include <list>
#include "Node.h"

//That's the output structure for the search algorithms.
//The task of the pathfinder is to fill it in a right way.

//Practically, after SearchResult is filled by the path finding algorithm,
//one may invoke XMLogger methods like writeToLogSummary etc.
//(passing appropriate fields of SearchResult as parameters)
//and these methods will flush the data to the XML.

//primary_path is a path that is a sequence of nodes, corresponding to adjacent cells on the grid.

//secondary_path is a path that is a sequence of pivotal nodes that may not be adjacent on the grid.
//It's like a path composed of the sections.

//You need to fill BOTH sorts of path (so both primary_path and secondary_path will be flushed to output log).

// A*/Dijkstra naturally generates primary_path. One might post-process it by skipping the nodes at which
//the movement direction does not change to form sections of secondary_path.

struct SearchResult
{
        bool path_found;
        double path_length; //if path not found, then path_length=0
        const std::list<Node>* primary_path; //path as the sequence of adjacent nodes (see above)
                                       //This is a pointer to the list of nodes that is actually created and hadled by ThetaSearch class,
                                       //so no need to re-create them, delete them etc. It's just a trick to save some memory
        const std::list<Node>* secondary_path; //path as the sequence of non-adjacent nodes: "sections" (see above)
                                        //This is a pointer to the list of nodes that is actually created and hadled by ThetaSearch class,
                                        //so no need to re-create them, delete them etc. It's just a trick to save some memory
        uint64_t nodes_created; //|OPEN| + |CLOSE| = total number of nodes saved in memory during search process.
        uint64_t number_of_steps; //number of iterations made by algorithm to find a solution
        double time; //runtime of the search algorithm (expanding nodes + reconstructing the path)
        SearchResult()
        {
            path_found = false;
            path_length = 0;
            primary_path = nullptr;
            secondary_path = nullptr;
            nodes_created = 0;
            number_of_steps = 0;
            time = 0;
        }

};

#endif
