//
// Created by Kliment Serafimov on 1/19/20.
//

#ifndef COMPACTPOSET_BITVECTOR_H
#define COMPACTPOSET_BITVECTOR_H

#include <bitset>

using namespace std;

#define BITVECTOR_SIZE 64

class Bitvector: public bitset<BITVECTOR_SIZE>
{
public:
    Bitvector() = default;
    Bitvector(bitset<BITVECTOR_SIZE> num);
    Bitvector(unsigned long long num);

    bool operator < (const int &other) const
    {
        return to_ullong() < other;
    }

    bool operator < (const Bitvector &other) const
    {
        for (int i = BITVECTOR_SIZE-1; i >= 0; i--) {
            if (test(i) ^ other.test(i)) return other.test(i);
        }
        return false;
    }

    void operator ++(int)
    {
        for(int i = 0;i < BITVECTOR_SIZE; i++)
        {
            if(!test(i))
            {
                set(i, 1);
                break;
            }
            else
            {
                set(i, 0);
            }
        }
//        unsigned long long ullong = to_ullong();
//        ullong++;
//        Bitvector ret = ullong;
//        return ret;
    }

    Bitvector get_flipped()
    {
        Bitvector ret = *this;
        ret.flip();
        return ret;
    }

    unsigned int num_trailing_zeroes();

    bool test_range(int begin, int end)
    {
        for(int i = begin; i<=end;i++)
        {
            if(!test(i))
            {
                return false;
            }
        }
        return true;
    }

    void set_range(int begin, int end)
    {
        for(int i = begin; i<=end;i++)
        {
            set(i);
        }
    }

};



#endif //COMPACTPOSET_BITVECTOR_H
