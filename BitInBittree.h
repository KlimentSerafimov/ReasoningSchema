//
// Created by Kliment Serafimov on 1/11/20.
//

#ifndef COMPACTPOSET_BITINBITTREE_H
#define COMPACTPOSET_BITINBITTREE_H

#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include "NodeTemplate.h"
#include <fstream>

using namespace std;

class BittreeTaskType;
class BittreeInputOutputType;
class BittreeNode;
class BitInBittree;
class BittreeTaskDecomposition;

class TreeNode: public Visitable
{

    vector<TreeNode*> parents;
    vector<Name> names;

    TreeNode* parent = nullptr;
    Name name;

public:

    BittreeNode* bittree_type_node = nullptr;
    BittreeInputOutputType* bittree_io_type = nullptr;
    BittreeTaskType* bittree_task_type = nullptr;
    BitInBittree* bit_in_bittree = nullptr;
    BittreeTaskDecomposition* bittree_task_decomposition = nullptr;


    vector<TreeNode*> copies;
    TreeNode* copied_from = nullptr;

    void init(TreeNode* _parent, Name _name)
    {
        parent = _parent;
        name = std::move(_name);
        parents.push_back(parent);
        names.push_back(name);
    }

    TreeNode() = default;

    TreeNode(TreeNode* _parent, Name name, BittreeNode* _subclass_instance)
    {
        init(_parent, std::move(name));
        bittree_type_node = _subclass_instance;
    }

    TreeNode(TreeNode* _parent, Name name, BittreeTaskType* _subclass_instance)
    {
        init(_parent, std::move(name));
        bittree_task_type = _subclass_instance;
    }

    TreeNode(TreeNode* _parent, Name name, BitInBittree* _subclass_instance)
    {
        init(_parent, std::move(name));
        bit_in_bittree = _subclass_instance;
    }

    TreeNode(TreeNode* _parent, Name name, BittreeInputOutputType* _subclass_instance)
    {
        init(_parent, std::move(name));
        bittree_io_type = _subclass_instance;
    }

    TreeNode(TreeNode* _parent, Name name, BittreeTaskDecomposition* _subclass_instance)
    {
        init(_parent, std::move(name));
        bittree_task_decomposition = _subclass_instance;
    }

    void append_bits(vector<BitInBittree*>& bits);

    string to_string()
    {
        string ret;
        if(bittree_type_node != nullptr)
        {
            ret += "bittree_type_node";
        }
        else if(bittree_io_type != nullptr)
        {
            ret += "bittree_io_type";
        }
        else if(bittree_task_type != nullptr)
        {
            ret += "bittree_task_type";
        }
        else if(bit_in_bittree != nullptr)
        {
            ret += "bit_in_bittree";
        }
        else if(bittree_task_decomposition != nullptr)
        {
            ret += "bittree_task_decomposition";
        }
        else
        {
            assert(false);
        }
        ret += "(name="+name.to_string()+")";
        return ret;
    }

    TreeNode* get_last_parent()
    {
        return parents.back();
    }

    Name get_last_name()
    {
        return names.back();
    }

    void push_back_parent(TreeNode* new_parent)
    {
        parents.push_back(new_parent);
    }
    void push_back_name(Name new_name)
    {
        names.push_back(new_name);
    }

    bool has_unique_name()
    {
        return names.size() == 1;
    }

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

    BitInBittree(BittreeNode *parent, Name name, BitInBittreeType _bit_type);

    BitInBittree(BittreeNode *parent, Name name, BitInBittreeType _bit_type, BitInBittree *_copied_from);

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

    string to_string__one_line();

    string slim_bit_to_string(int tab);
};

#endif //COMPACTPOSET_BITINBITTREE_H
