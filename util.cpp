//
// Created by Kliment Serafimov on 2019-11-19.
//


#include "util.h"
#include "Bitvector.h"
#include <iostream>

#include <vector>

string tabs(int num_tabs) {
    string ret;
    for(int i = 0;i<num_tabs;i++)
    {
        ret += "\t";
    }
    return ret;
}

string bitvector_to_str(int bitvector, int n)
{
    string ret = "";
    for(int i = n-1;i>=0;i--)
    {
        if(get_bit(Bitvector(bitvector, n), i))
        {
            ret+="1";
        }
        else
        {
            ret+="0";
        }
    }
    return ret;
}

string bitvector_to_str(Bitvector bitvector, int n)
{
    assert(bitvector.get_size() >= n);
    return bitvector.to_string();
//    string to_remove_0s = bitvector.to_string();
//    string ret;
//    for(int i = to_remove_0s.size()-1; i >= to_remove_0s.size()-n ;i--)
//    {
//        ret += to_remove_0s[i];
//    }
//    reverse(ret.begin(), ret.end());
//    return ret;
}

int get_bit_of_int(int bitvector, int idx) {
    return (bitvector & (1<<idx)) != 0;
}

int get_bit_of_int(unsigned long long bitvector, int idx) {
    unsigned long long one = 1;
    return (bitvector & (one<<idx)) != 0;
}

int get_bit(Bitvector bitvector, int idx) {
    int ret = bitvector.get_bit(idx);
    return ret;
}

Bitvector::Bitvector(BitvectorConstructorType type, int _size)
{
    set_size(_size);

    if(type == all_ones)
    {
        set_range(0, size-1);
    }

    if(CONTROL) {
        for(int i = 0;i<size;i++)
        {
            get_bit(i);
        }
    }
}

Bitvector::Bitvector(bitset<MAX_BITVECTOR_SIZE> _bitset)//: bitset<MAX_BITVECTOR_SIZE>(_bitset)
{
    set_size(MAX_BITVECTOR_SIZE);

    for(int i = 0;i<MAX_BITVECTOR_SIZE;i++)
    {
        set(i, _bitset[i]);
    }

    control = _bitset;

    if(CONTROL) {
        for(int i = 0;i<size;i++)
        {
            get_bit(i);
        }
    }
}

Bitvector::Bitvector(unsigned long long num)//: bitset<MAX_BITVECTOR_SIZE>(num)
{
    set_size(MAX_BITVECTOR_SIZE);

    int begin = 0;
    int end = get_size()-1;

    int init_block = 0 / MAX_BLOCK_SIZE;
    int end_block = end / MAX_BLOCK_SIZE;

    for(int i = init_block; i<=end_block;i++) {
        int first_idx = block_ranges[i].first;
        int second_idx = block_ranges[i].second;
        if (i == init_block) {
            first_idx = begin;
        }
        if (i == end_block) {
            second_idx = end;
        }
        unsigned long long slice = int_get_subint(num, first_idx, second_idx);

        blocks[i] = (slice >> first_idx);
    }

    if(CONTROL) {
        control = bitset<MAX_BITVECTOR_SIZE>(num);
        for(int i = 0;i<size;i++)
        {
            get_bit(i);
        }
    }
}

unsigned int Bitvector::num_trailing_zeroes() {
    int ret = 0;
    for(int i = 0;i<size; i++)
    {
        if(get_bit(i) == false)
        {
            ret++;
        } else {
            break;
        }
    }
    return ret;
}

void Bitvector::set_size(int _size)
{
   // assert(!size_defined || _size == size);
//    for(int i = _size; i<MAX_BITVECTOR_SIZE; i++)
//    {
//        assert(!test(i));
//    }

    if(!defined)
    {
        defined = true;
        for(int i = 0;i<MAX_NUM_BLOCKS;i++)
        {
            block_defined[i] = false;
        }
    }

    assert(_size <= MAX_BITVECTOR_SIZE);

    if(!size_defined || size != _size) {
        size_defined = true;
        size = _size;
        num_blocks = _size / MAX_BLOCK_SIZE + (_size % MAX_BLOCK_SIZE != 0);
        int at_bit_idx = 0;
        for (int i = 0; i < num_blocks; i++) {
            int next_at_bit_id = min(_size, at_bit_idx + MAX_BLOCK_SIZE);
            block_ranges[i] = make_pair(at_bit_idx, next_at_bit_id-1);
            if(!block_defined[i]) {
                blocks[i] = 0;
                block_defined[i] = true;
            }
            at_bit_idx = next_at_bit_id;
        }
        assert(at_bit_idx == _size);
    }
}

Bitvector::Bitvector(unsigned long long bits, int _size)// : bitset<MAX_BITVECTOR_SIZE>(bits)
{
    set_size(_size);

    int begin = 0;
    int end = get_size()-1;

    int init_block = 0 / MAX_BLOCK_SIZE;
    int end_block = end / MAX_BLOCK_SIZE;

    for(int i = init_block; i<=end_block;i++) {
        int first_idx = block_ranges[i].first;
        int second_idx = block_ranges[i].second;
        if (i == init_block) {
            first_idx = begin;
        }
        if (i == end_block) {
            second_idx = end;
        }

        unsigned long long slice = int_get_subint(bits, first_idx, second_idx);

        blocks[i] = (slice >> first_idx);
    }

    if(CONTROL) {
        control = bitset<MAX_BITVECTOR_SIZE>(bits);
        for(int i = 0;i<size;i++)
        {
            get_bit(i);
        }
    }
}

int Bitvector::get_bit(int idx) const {
    assert(idx < get_size());
    int block_idx = idx/MAX_BLOCK_SIZE;
    int offset = block_idx*MAX_BLOCK_SIZE;
    int int_idx = idx-offset;

    int ret = get_bit_of_int(blocks[block_idx], int_idx);
    if(CONTROL) {
        int second_ret = control.test(idx);
        if(ret != second_ret)
        {
            cout << control.to_string() << endl;
            for(int i = 0;i<num_blocks;i++)
            {
                cout << blocks[i] <<" ";
            }
            cout << endl;
        }
        assert(ret == second_ret);
    }
    return ret;

}

unsigned long long Bitvector::to_ullong() const {
    bitset<MAX_BITVECTOR_SIZE> local_bitset;
    for(int i = 0;i<get_size();i++)
    {
        local_bitset.set(i, get_bit(i));
    }
    return local_bitset.to_ullong();
}

string vector_of_vector_of_int_to_string(vector<vector<int> > to_print, string title)
{
    string ret;
    ret += title + ": \n";
    for(int i = 0;i<to_print.size();i++)
    {
        ret += tabs(1) + "i = " + std::to_string(i) + ": ";
        for(int j = 0;j<to_print[i].size();j++)
        {
            ret += std::to_string(to_print[i][j]) + " ";
        }
        ret += "\n";
    }
    return ret;
}

int string_to_bitvector(string str)
{
    int ret = 0;
    int power = 1;
    for(int i = (int) str.size()-1;i>=0;i--)
    {
        ret+=(str[i]-'0')*power;
        power*=2;
    }
    return ret;
}

unsigned int num_trailing_zeroes(Bitvector v)
{
    return v.num_trailing_zeroes();
    //v: 32-bit word input to count zero bits on right
    unsigned int c;     // c will be the number of zero bits on the right,
    // so if v is 1101000 (base 2), then c will be 3
    // NOTE: if 0 == v, then c = 31.
//    if (v & 0x1)
//    {
//        // special case for odd v (assumed to happen half of the time)
//        c = 0;
//    }
//    else
//    {
//        c = 1;
//        if ((v & 0xffff) == 0)
//        {
//            v >>= 16;
//            c += 16;
//        }
//        if ((v & 0xff) == 0)
//        {
//            v >>= 8;
//            c += 8;
//        }
//        if ((v & 0xf) == 0)
//        {
//            v >>= 4;
//            c += 4;
//        }
//        if ((v & 0x3) == 0)
//        {
//            v >>= 2;
//            c += 2;
//        }
//        c -= v & 0x1;
//    }
//    return c;
}