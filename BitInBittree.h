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
class BittreeTypeNode;
class BitInBittree;

class TreeNode: public NodeTemplate
{
public:

    BittreeTypeNode* bittree_type_node = NULL;
    BittreeInputOutputType* bittree_io_type = NULL;
    BittreeTaskType* bittree_task_type = NULL;
    BitInBittree* bit_in_bittree = NULL;

    TreeNode(NodeTemplate* _parent, Name name, BittreeTypeNode* _subclass_instance)
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
