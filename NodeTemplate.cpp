//
// Created by Kliment Serafimov on 1/29/20.
//

#include "NodeTemplate.h"

int global_vis_ids[NUM_VISITED_ARRAYS] = {0, 0, 0, 0, 0, 0, 0};

int* get_global_vis_ids()
{
    return global_vis_ids;
}

void memset_visited(VisitedType type)
{
    assert(type == vis_bits);
    global_vis_ids[type]++;
}

void memset_visited(VisitedType type, int num_subtree_markers)
{
    assert(type == vis_type);
    global_vis_ids[type+num_subtree_markers]++;
}
string NodeTemplate::root_to_node_path__to__string()
{
    string ret;
    assert(parents.size() == names.size());
    for(int i = 0;i<parents.size();i++)
    {
        if(i>=1)
        {
            ret += " | ";
        }
        ret += names[i].to_string();
        if(parents[i]!=NULL)
        {
            ret += " <- {" + parents[i]->root_to_node_path__to__string() + "}";
        }
        else
        {
            ret += " <- end";
        }
    }
    return ret;
}
