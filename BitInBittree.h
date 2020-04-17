//
// Created by Kliment Serafimov on 1/11/20.
//

#ifndef COMPACTPOSET_BITINBITTREE_H
#define COMPACTPOSET_BITINBITTREE_H

#include <string>
#include <vector>
#include <iostream>
#include "NodeTemplate.h"

using namespace std;

class BittreeTaskType;
class BittreeInputOutputType;
class BittreeNode;
class BitInBittree;
class BittreeTaskDecomposition;

class TreeNode: public NodeTemplate
{
public:

    BittreeNode* bittree_type_node = NULL;
    BittreeInputOutputType* bittree_io_type = NULL;
    BittreeTaskType* bittree_task_type = NULL;
    BitInBittree* bit_in_bittree = NULL;
    BittreeTaskDecomposition* bittree_task_decomposition = NULL;

    vector<TreeNode*> copies;
    TreeNode* copied_from;

    TreeNode(NodeTemplate* _parent, Name name, BittreeNode* _subclass_instance)
    {
        init(_parent, name);
        bittree_type_node = _subclass_instance;
    }

    TreeNode(NodeTemplate* _parent, Name name, BittreeTaskType* _subclass_instance)
    {
        init(_parent, name);
        bittree_task_type = _subclass_instance;
    }

    TreeNode(NodeTemplate* _parent, Name name, BitInBittree* _subclass_instance)
    {
        init(_parent, name);
        bit_in_bittree = _subclass_instance;
    }

    TreeNode(NodeTemplate* _parent, Name name, BittreeInputOutputType* _subclass_instance)
    {
        init(_parent, name);
        bittree_io_type = _subclass_instance;
    }

    TreeNode(NodeTemplate* _parent, Name name, BittreeTaskDecomposition* _subclass_instance)
    {
        init(_parent, name);
        bittree_task_decomposition = _subclass_instance;
    }

    void append_bits(vector<BitInBittree*>& bits);
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
};

#endif //COMPACTPOSET_BITINBITTREE_H
