//
// Created by Kliment Serafimov on 1/11/20.
//

#include "BitInBittree.h"
#include "BittreeTaskType.h"


BitInBittree::BitInBittree(BittreeNode *parent, Name name, BitInBittreeType _bit_type) : TreeNode(parent, name, this)
{
    bit_type = _bit_type;
    if(bit_type == new_machine_bit || bit_type == shared_machine_bit)
    {
        bit_id = global_bit_id;
        global_bit_id++;
    }
}

BitInBittree::BitInBittree(BittreeNode* _parent, Name name, BitInBittreeType _bit_type, BitInBittree* _copied_from): TreeNode(_parent, name, this)
{
    bit_type = _bit_type;
    if(bit_type == new_machine_bit || bit_type == shared_machine_bit)
    {
        bit_id = global_bit_id;
        global_bit_id++;
    }
    copied_from = _copied_from;
    _copied_from->copies.push_back(this);
    is_bit_set = copied_from->bit_in_bittree->is_bit_set;
    bit_val = copied_from->bit_in_bittree->bit_val;
}

string BitInBittree::to_string() {
    if (!is_bit_set) {
        string bit_id_str;
        if (bit_type == new_machine_bit || bit_type == shared_machine_bit) {
            bit_id_str =  std::to_string(bit_id) +": bit_id";
        } else {
            bit_id_str =  "_ : bit_id";
        }
        return "[" + bit_id_str + "]";

    } else {
        return std::to_string(bit_val);
    }
}


void BitInBittree::append_bits(vector<BitInBittree *>& bits) {
    if(!visited(vis_bits)) {
//        cout << "not visited" << endl;
        visit(vis_bits);
        bits.push_back(this);
    }
    else
    {
//        cout << "visited" << endl;
    }
}

string BitInBittree::to_string__one_line() {
    if(is_bit_set)
    {
        return std::to_string(bit_val);
    }
    else
    {
        return "_";
    }
}

string BitInBittree::slim_bit_to_string(int tab) {
    return tabs(tab) + to_string__one_line();
}

void TreeNode::append_bits(vector<BitInBittree*>& bits) {

    bool enter = false;
    if(bittree_type_node != nullptr)
    {
        enter = true;
        bittree_type_node->append_bits(bits);
    }
    if(bittree_task_type != nullptr)
    {
        assert(!enter);
        enter = true;
        bittree_task_type->append_bits(bits);
    }
    if(bit_in_bittree != nullptr)
    {
        assert(!enter);
        enter = true;
        bit_in_bittree->append_bits(bits);
    }
    if(bittree_io_type != nullptr)
    {
        assert(!enter);
        enter = true;
        bittree_io_type->append_bits(bits);
    }
    if(bittree_task_decomposition != nullptr)
    {
        assert(!enter);
        enter = true;
        bittree_task_decomposition->append_bits(bits);
    }
    assert(enter);
}