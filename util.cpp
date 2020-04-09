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
        if(get_bit(bitvector, i))
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
    assert(bitvector.get_size() == n);
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

int get_bit(Bitvector bitvector, int idx) {
    return bitvector.test(idx);
}

Bitvector::Bitvector(BitvectorConstructorType type, int _size)
{
    size_defined = true;
    size = _size;
    if(type == all_ones)
    {
        for(int i = 0;i<size;i++)
        {
            set(i);
        }
    }
}

Bitvector::Bitvector(bitset<BITVECTOR_SIZE> _bitset): bitset<BITVECTOR_SIZE>(_bitset)
{
}

Bitvector::Bitvector(long long num): bitset<BITVECTOR_SIZE>(num)
{
    assert(num >= 0);
}

unsigned int Bitvector::num_trailing_zeroes() {
    int ret = 0;
    for(int i = 0;i<BITVECTOR_SIZE; i++)
    {
        if(test(i) == false)
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
    assert(!size_defined);
    for(int i = _size; i<BITVECTOR_SIZE; i++)
    {
        assert(!test(i));
    }
    size_defined = true;
    size = _size;
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