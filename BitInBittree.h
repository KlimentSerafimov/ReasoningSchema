//
// Created by Kliment Serafimov on 1/11/20.
//

#ifndef COMPACTPOSET_BITINBITTREE_H
#define COMPACTPOSET_BITINBITTREE_H


static int global_bit_id = 0;

class BittreeTypeNode;

enum BitInBittreeType {new_machine_bit, shared_machine_bit, shared_blanko_bit, new_blanko_bit};

class BitInBittree
{
public:
    BittreeTypeNode* parent;
    BitInBittreeType bit_type;

    int bit_id;
    bool is_bit_set = false;
    int bit_val;

    BitInBittree(BittreeTypeNode* parent, BitInBittreeType _bit_type);
};

#endif //COMPACTPOSET_BITINBITTREE_H
