//
// Created by Kliment Serafimov on 1/29/20.
//

#ifndef COMPACTPOSET_NODETEMPLATE_H
#define COMPACTPOSET_NODETEMPLATE_H

#include <string>
#include <vector>
#include <map>
#include <assert.h>

using namespace std;

enum VisitedType {vis_bits, vis_type, vis_origin, vis_leafs};

//int global_vis_ids[2];

map<pair<VisitedType, int>, int>* get_global_vis_ids();

void memset_visited(VisitedType type, int num_subtree_markers);

void memset_visited(VisitedType type);

enum NameType {id_name, str_name};

class Name
{
    NameType name_type;
    int id;
    string name;

public:
    Name(string _name, int _id)
    {
        name_type = id_name;
        name = _name;
        id = _id;
    }

    string to_string()
    {
        if(name_type == id_name)
        {
            return name + "["+std::to_string(id)+"]";
        }
        else if(name_type == str_name)
        {
            return name;
        }
        else
        {
            assert(false);
        }
    }

    Name(string _name)
    {
        name_type = str_name;
        name = _name;
    }
};

//#define NUM_VISITED_ARRAYS 7

class Visitable
{
public:
    map<pair<VisitedType, int>, int> vis_id;

    Visitable()
    {
    }

    void visit(VisitedType type)
    {
        visit(type, 0);
    }

    void visit(VisitedType type, int num_subtree_markers)
    {
        pair<VisitedType, int> idx = make_pair(type, num_subtree_markers);
        vis_id[idx] = get_global_vis_ids()->at(idx);
    }

    bool visited(VisitedType type)
    {
        return visited(type, 0);
    }

    bool visited(VisitedType type, int num_subtree_markers)
    {
        pair<VisitedType, int> idx = make_pair(type, num_subtree_markers);
//        cout << "visited("<<std::to_string(type) << ")=" << (vis_id[type])<<  " ==  " << local_global_vis_ids[type] << endl;
        if(vis_id.find(idx) == vis_id.end())
        {
            return false;
        }
        return vis_id[idx] == get_global_vis_ids()->at(idx);
    }
};

static int global_node_id = 0;

class NodeTemplate: public Visitable
{
public:
    vector<NodeTemplate*> parents;
    vector<Name> names;

    int node_id = -1;

    void init(NodeTemplate* _parent, Name name)
    {
        parents.push_back(_parent);
        names.push_back(name);
        node_id = global_node_id++;
    }

    string to_string()
    {
        string parent_str;
        for(int i = 0;i<parents.size();i++)
        {
            if(i!=0)
            {
                parent_str+=", ";
            }
            if(parents[i] != NULL)
            {
                parent_str += std::to_string(parents[i]->node_id);
            }
            else
            {
                parent_str += "_";
            }
        }
        return "["+std::to_string(node_id)+": id"+"; "+parent_str+": parent_ids"+"]";
    }

    string root_to_node_path__to__string();
};



#endif //COMPACTPOSET_NODETEMPLATE_H
