//
// Created by Kliment Serafimov on 1/11/20.
//

#include "BitInBittree.h"
#include "BittreeTaskType.h"

BitInBittree::BitInBittree(BittreeTypeNode* _parent, BitInBittreeType _bit_type)
{
    parent = _parent;
    bit_type = _bit_type;
    if(bit_type == new_machine_bit || bit_type == shared_machine_bit)
    {
    bit_id = global_bit_id;
    global_bit_id++;
    }
}