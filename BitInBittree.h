//
// Created by Kliment Serafimov on 1/11/20.
//

#ifndef COMPACTPOSET_BITINBITTREE_H
#define COMPACTPOSET_BITINBITTREE_H


static int global_bit_id = 0;

enum BitInBittreeType {new_machine_bit, shared_machine_bit, shared_blanko_bit, new_blanko_bit};

class BitInBittree
{
public:
    BitInBittreeType bit_type;

    int bit_id;
    bool is_bit_set = false;
    int bit_val;
    BitInBittree(BitInBittreeType _bit_type)
    {
        bit_type = _bit_type;
        if(bit_type == new_machine_bit || bit_type == shared_machine_bit)
        {
            bit_id = global_bit_id;
            global_bit_id++;
        }
    }
};

#endif //COMPACTPOSET_BITINBITTREE_H
