//
// Created by Kliment Serafimov on 1/29/20.
//

#include "NodeTemplate.h"

map<pair<VisitedType, int>, int> global_vis_ids;

map<pair<VisitedType, int>, int>* get_global_vis_ids()
{
    return &global_vis_ids;
}

void memset_visited(VisitedType type)
{
    memset_visited(type, 0);
}

void memset_visited(VisitedType type, int int_idx)
{
    pair<VisitedType, int> idx = make_pair(type, int_idx);
    if(global_vis_ids.find(idx) == global_vis_ids.end())
    {
        global_vis_ids[idx] = 0;
    }
    else
    {
        global_vis_ids[idx]++;
    }
}

//string NodeTemplate::root_to_node_path__to__string()
//{
//    string ret;
//    assert(parents.size() == names.size());
//    for(int i = 0;i<parents.size();i++)
//    {
//        if(i>=1)
//        {
//            ret += " | ";
//        }
//        ret += names[i].to_string();
//        if(parents[i]!=nullptr)
//        {
//            ret += " <- {" + parents[i]->root_to_node_path__to__string() + "}";
//        }
//        else
//        {
//            ret += " <- end";
//        }
//    }
//    return ret;
//}
