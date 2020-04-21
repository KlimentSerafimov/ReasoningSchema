//
// Created by kliment on 4/20/20.
//

#ifndef COMPACTPOSET_LABELEDGRAPH_H
#define COMPACTPOSET_LABELEDGRAPH_H

#include <string>
#include <vector>
#include <map>

#include "BittreeTaskType.h"

using namespace std;

class Node
{
    map<string, int> edge_label_to_node_id;
};

class LabeledGraph
{
    map<string, int> node_label_to_node_id;
    vector<Node> nodes;

    LabeledGraph(BittreeTaskType* bittree_task_type)
    {

    }
};


#endif //COMPACTPOSET_LABELEDGRAPH_H
