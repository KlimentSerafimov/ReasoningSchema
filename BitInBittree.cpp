//
// Created by Kliment Serafimov on 1/11/20.
//

#include "BitInBittree.h"
#include "BittreeTaskType.h"


BitInBittree::BitInBittree(BittreeTypeNode *parent, Name name, BitInBittreeType _bit_type) : TreeNode(parent, name, this)
{
    bit_type = _bit_type;
    if(bit_type == new_machine_bit || bit_type == shared_machine_bit)
    {
        bit_id = global_bit_id;
        global_bit_id++;
    }
}

BitInBittree::BitInBittree(BittreeTypeNode* _parent, Name name, BitInBittreeType _bit_type, BitInBittree* _copied_from): TreeNode(_parent, name, this)
{
    bit_type = _bit_type;
    if(bit_type == new_machine_bit || bit_type == shared_machine_bit)
    {
        bit_id = global_bit_id;
        global_bit_id++;
    }
    copied_from = _copied_from;
}

string BitInBittree::to_string() {
    if (!is_bit_set) {
        string copied_from_str = "";
        if(copied_from != NULL){
            if(copied_from->bit_type == new_machine_bit || copied_from->bit_type == shared_machine_bit){
                copied_from_str = ", "+std::to_string(copied_from->bit_id) + ": original_id";
            }
        }
        string bit_id_str;
        if (bit_type == new_machine_bit || bit_type == shared_machine_bit) {
            bit_id_str =  std::to_string(bit_id) +": bit_id";
        } else {
            bit_id_str =  "_ : bit_id";
        }
        return "[" + bit_id_str + copied_from_str + "]";

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

void TreeNode::append_bits(vector<BitInBittree*>& bits) {

    bool enter = false;
    if(bittree_type_node != NULL)
    {
//        cout << "in bittree_type_node" << endl;
        enter = true;
        bittree_type_node->append_bits(bits);
    }
    if(bittree_task_type != NULL)
    {
//        cout << "in bittree_task_type" << endl;
        assert(!enter);
        enter = true;
        bittree_task_type->append_bits(bits);
    }
    if(bit_in_bittree != NULL)
    {
//        cout << "in bit_in_bittree" << endl;
        assert(!enter);
        enter = true;
        bit_in_bittree->append_bits(bits);
    }
    assert(enter);
}