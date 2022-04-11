#ifndef NODE_H
#define NODE_H

//That's the data structure for storing a single search node.
//You MUST store all the intermediate computations occuring during the search
//incapsulated to Nodes (so NO separate arrays of g-values etc.)

#include <tuple>

struct Node {
    int i, j; //grid cell coordinates
    double F, g, H; //f-, g- and h-values of the search node
    Node *parent; //backpointer to the predecessor node
    // (e.g. the node which g-value was used to set the g-velue of the current node)

    Node(int i, int j, double g, double H, Node *parent) : i(i), j(j), g(g), H(H), parent(parent) {
        F = this->g + this->H;
    }
};

// Struct to compare nodes in the set.
struct NodesComparator {
    bool operator()(const Node *lhs, const Node *rhs) const {
        return std::tie(lhs->F, lhs->i, lhs->j) < std::tie(rhs->F, rhs->i, rhs->j);
    }
};

#endif
