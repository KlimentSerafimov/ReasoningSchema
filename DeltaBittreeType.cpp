//
// Created by Kliment Serafimov on 1/11/20.
//

#include "DeltaBittreeType.h"
#include "BittreeTaskType.h"
#include <vector>

DeltaBittreeType::DeltaBittreeType(NodeType delta_node_type)
{
    node_type = leaf_node;
    children = new vector<DeltaBittreeType*>();
    delta = new BittreeTypeNode(delta_node_type);
}
