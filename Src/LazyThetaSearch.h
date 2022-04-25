//
// Created by Karim on 18.04.2022.
//

#ifndef KARIM_RAKHMATULLIN_ASEARCH_LAZYTHETASEARCH_H
#define KARIM_RAKHMATULLIN_ASEARCH_LAZYTHETASEARCH_H

#include <vector>
#include <limits>
#include <chrono>
#include <map>
#include <set>
#include <iostream>
#include "ThetaSearch.h"


class LazyThetaSearch : public ThetaSearch {
public:
    LazyThetaSearch() : ThetaSearch() {
    }

    // Main logic of the program: starts search algorithm.
    SearchResult startSearch(const Map &Map, const EnvironmentOptions &options) override;

private:
    Node* findParent(Node *curNode, const Map &map, const EnvironmentOptions &options);
    static void setVertex(Node* node_to_expand, const Map &map, const EnvironmentOptions &options);
};


#endif //KARIM_RAKHMATULLIN_ASEARCH_LAZYTHETASEARCH_H
