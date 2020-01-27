//
// Created by Kliment Serafimov on 1/11/20.
//

#ifndef COMPACTPOSET_BITINBITTREE_H
#define COMPACTPOSET_BITINBITTREE_H

#include <string>
#include <vector>
#include <iostream>

#define NUM_VISITED_ARRAYS 7

using namespace std;

enum VisitedType {vis_bits, vis_type};

//int global_vis_ids[2];

int* get_global_vis_ids();

void memset_visited(VisitedType type, int rec_depth);

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

class Visitable
{
public:
    int vis_id[NUM_VISITED_ARRAYS];

    Visitable()
    {
        for(int i = 0;i<NUM_VISITED_ARRAYS;i++)
        {
            vis_id[i] = -1;
        }
    }

    void visit(VisitedType type)
    {
        assert(type == vis_bits);
        vis_id[type] = get_global_vis_ids()[type];
    }

    void visit(VisitedType type, int rec_depth)
    {
        assert(type == vis_type);
        vis_id[type] = get_global_vis_ids()[type+rec_depth];
    }

    bool visited(VisitedType type)
    {
        assert(type == vis_bits);
//        cout << "visited("<<std::to_string(type) << ")=" << (vis_id[type])<<  " ==  " << local_global_vis_ids[type] << endl;
        return vis_id[type] == get_global_vis_ids()[type];
    }

    bool visited(VisitedType type, int rec_depth)
    {
//        cout << "visited("<<std::to_string(type) << ")=" << (vis_id[type])<<  " ==  " << local_global_vis_ids[type] << endl;
        return vis_id[type] == get_global_vis_ids()[type+rec_depth];
    }
};

class BittreeTypeNode;
class BittreeTaskType;
class BitInBittree;

static int global_node_id = 0;

class TreeNode: public Visitable
{
public:

    vector<TreeNode*> parents;
    vector<Name> names;
    BittreeTypeNode* bittree_type_node = NULL;
    BittreeTaskType* bittree_task_type = NULL;
    BitInBittree* bit_in_bittree = NULL;

    int node_id = -1;

    TreeNode(TreeNode* _parent, Name name, BittreeTypeNode* _subclass_instance)
    {
        init(_parent, name);
        bittree_type_node = _subclass_instance;
    }

    TreeNode(TreeNode* _parent, Name name, BittreeTaskType* _subclass_instance)
    {
        init(_parent, name);
        bittree_task_type = _subclass_instance;
    }

    TreeNode(TreeNode* _parent, Name name, BitInBittree* _subclass_instance)
    {
        init(_parent, name);
        bit_in_bittree = _subclass_instance;
    }

    void init(TreeNode* _parent, Name name)
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

    void append_bits(vector<BitInBittree*>& bits);

    string root_to_node_path__to__string();
};

static int global_bit_id = 0;


enum BitInBittreeType {new_machine_bit, shared_machine_bit, shared_blanko_bit, new_blanko_bit};

class BitInBittree: public TreeNode
{
public:
    BitInBittreeType bit_type;

    int bit_id;
    bool is_bit_set = false;
    int bit_val;

    BitInBittree* copied_from;

    BitInBittree(BittreeTypeNode *parent, Name name, BitInBittreeType _bit_type);

    BitInBittree(BittreeTypeNode *parent, Name name, BitInBittreeType _bit_type, BitInBittree *_copied_from);

    string to_string();

    void append_bits(vector<BitInBittree*>& bits);

    void set(int _val)
    {
        is_bit_set = true;
        bit_val = _val;
    }

    void flip()
    {
        assert(is_bit_set);
        assert(bit_val == 0 || bit_val == 1);
        bit_val = 1-bit_val;
    }
};

#endif //COMPACTPOSET_BITINBITTREE_H
