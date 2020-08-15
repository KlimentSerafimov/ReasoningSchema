//
// Created by Kliment Serafimov on 1/19/20.
//

#ifndef COMPACTPOSET_BITVECTOR_H
#define COMPACTPOSET_BITVECTOR_H

#include <bitset>
#include <assert.h>
#include <algorithm>
#include <iostream>

#define CONTROL false
#define CONTROL_BITVECTOR_SIZE 0

using namespace std;

#define NUM_BITS_IN_BLOCK_SIZE 5
#define BLOCK_SIZE (1<<NUM_BITS_IN_BLOCK_SIZE)
#define MAX_BITVECTOR_SIZE BLOCK_SIZE*32

#define MAX_NUM_BLOCKS MAX_BITVECTOR_SIZE/BLOCK_SIZE + (MAX_BITVECTOR_SIZE % BLOCK_SIZE != 0)

enum BitvectorConstructorType {all_zeroes, all_ones};

int get_op_counter();

void increment_op_counter();



class Bitvector//: public bitset<MAX_BITVECTOR_SIZE>
{

    int defined = false;
    int num_blocks;
    unsigned long long blocks[MAX_NUM_BLOCKS];
    pair<int, int> block_ranges[MAX_NUM_BLOCKS];

    int size;

    bitset<CONTROL_BITVECTOR_SIZE> control;

//    LabeledGraph labeled_hypergraph;

public:

    Bitvector()
    {
//        set_size(0);
        if(CONTROL) {
            control = bitset<CONTROL_BITVECTOR_SIZE>(0);
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
    };

//    Bitvector(LabeledGraph labeled_graph);

    Bitvector(BitvectorConstructorType type, int _size);
    Bitvector(unsigned long long num, int _size);
    Bitvector(bitset<MAX_BITVECTOR_SIZE> num);
    Bitvector(unsigned long long num);
    Bitvector(const Bitvector *to_copy, bool do_copy, bool do_copy_2)
    {
        if(CONTROL) {
            for (int i = 0; i < to_copy->get_size(); i++) {
                to_copy->get_bit(i);
            }
        }
        assert(do_copy && do_copy_2);
        assert(to_copy->defined);
        defined = true;
        size = to_copy->get_size();
        num_blocks = to_copy->num_blocks;
        for(int i = 0;i<num_blocks;i++)
        {
            blocks[i] = to_copy->blocks[i];
            block_ranges[i] = to_copy->block_ranges[i];
        }
//        for(int i = 0;i<get_size();i++)
//        {
//            set(i, to_copy->get_bit(i));
//        }
        if(CONTROL) {
//            control = to_copy->control;
            for (int i = 0; i < size; i++) {
                control.set(i, to_copy->control.test(i));
                get_bit(i);
            }
        }
    }

    unsigned long long int_get_subint(unsigned long long the_int, int left, int right) const
    {
        unsigned long long one = 1;
        return the_int & ((one << (right + 1)) - 1 - ((one << left)-1));
    }

    int idx_to_block(int idx) const
    {
        return idx >> NUM_BITS_IN_BLOCK_SIZE;
        return idx / BLOCK_SIZE;
    }

    void place_block(int idx, unsigned long long bits);

    Bitvector operator + (const Bitvector& other) const
    {
        Bitvector ret = Bitvector();
        ret.set_size(max(other.get_size(), get_size())+1);
        int carry = 0;
        for(int i = 0;i<num_blocks;i++)
        {
            cout << ((1ull<<BLOCK_SIZE)-1) << endl;
            cout << (other.blocks[i] & (((1ull<<(64-BLOCK_SIZE))-1) << BLOCK_SIZE)) << endl;
            assert((other.blocks[i] & (((1ull<<(64-BLOCK_SIZE))-1) << BLOCK_SIZE)) == 0);
            assert((blocks[i] & (((1ull<<(64-BLOCK_SIZE))-1) << BLOCK_SIZE)) == 0);
            unsigned long long block_sum = blocks[i]+other.blocks[i]+carry;
            carry = ((block_sum & (1ull<<BLOCK_SIZE)) != 0);
            if(carry)
            {
                block_sum &= (unsigned long long)((1ull<<BLOCK_SIZE)-1);
            }
            ret.place_block(i, block_sum);
        }
        return ret;
    }
    
    bool operator == (const Bitvector other) const
    {
        int end = min(get_size()-1, other.get_size()-1);

        int init_block = idx_to_block(0);
        int end_block = idx_to_block(end);

        bool ret = true;
        for(int i = init_block; i<=end_block;i++)
        {
            if(blocks[i] != other.blocks[i])
            {
                ret = false;
                break;
            };
        }

        if(CONTROL) {
            bool second_ret = true;
            for (int i = 0; i < min(get_size(), other.get_size()); i++) {
                get_bit(i);
                if(control.test(i) != other.control.test(i) ) {
                    second_ret = false;
                    break;
                }
            }
            assert(ret == second_ret);
        }


        return ret;
    }

    bool operator != (const Bitvector other) const
    {
        return !(*this == other);
        return true;
    }

    bool operator < (const int &other) const
    {
        return to_ullong() < other;
    }

    int get_size() const
    {
        return size;
    }

    bool operator < (const Bitvector &other) const
    {
        bool ret = false;
        if(num_blocks == 1)
        {
            ret = blocks[0] < other.blocks[0];
        }
        else {
//            assert(false);
            for (int i = min(get_size() - 1, other.get_size() - 1); i >= 0; i--) {
                if (get_bit(i) ^ other.get_bit(i)){
                    ret = other.get_bit(i);
                    break;
                }
            }
        }
        return ret;
    }

    void operator ++(int)
    {
        for(int i = 0;i < size; i++)
        {
            if(!get_bit(i))
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

    Bitvector& operator&=(const Bitvector& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        /* addition of rhs to *this takes place here */


//        if(num_blocks == 1)
//        {
//            blocks[0] &= rhs.blocks[0];
//            return *this;
//        }

        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        int begin = 0;
        int end = min(get_size()-1, rhs.get_size()-1);

        int init_block = idx_to_block(0);
        int end_block = idx_to_block(end);

        //int offset = 0;
        for(int i = init_block; i<=end_block;i++)//, offset+=BLOCK_SIZE)
        {
//            int first_idx = block_ranges[i].first-offset;
//            int second_idx = block_ranges[i].second-offset;
//            if(i == init_block)
//            {
//                first_idx = begin-offset;
//            }
//            if(i == end_block)
//            {
//                second_idx = end-offset;
//            }

            blocks[i] &= rhs.blocks[i];

            //unsigned long long one = 1;
            //blocks[i] &= (one<<(second_idx+1))-1;
        }

        if(CONTROL) {
//            control &= rhs.control;

            for (int i = 0; i < size; i++) {
                control.set(i, control.test(i) & rhs.control.test(i));
                get_bit(i);
            }
        }

        return *this; // return the result by reference
    }

//    void rep()
//    {
//        for (int i = 0; i < size; i++) {
//            get_bit(i);
//        }
//    }

    Bitvector operator & (const Bitvector& other) const {
        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
            for (int i = 0; i < other.get_size(); i++) {
                other.get_bit(i);
            }
        }

        Bitvector ret = Bitvector(this, true, true);
        ret &= other;

        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
        return ret;
    }

    Bitvector& operator|=(const Bitvector& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        /* addition of rhs to *this takes place here */

        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        int begin = 0;
        int end = min(get_size()-1, rhs.get_size()-1);

        int init_block = idx_to_block(0);
        int end_block = idx_to_block(end);

//        int offset = 0;
        for(int i = init_block; i<=end_block;i++)//,offset += BLOCK_SIZE)
        {
//            int first_idx = block_ranges[i].first-offset;
//            int second_idx = block_ranges[i].second-offset;
//            if(i == init_block)
//            {
//                first_idx = begin-offset;
//            }
//            if(i == end_block)
//            {
//                second_idx = end-offset;
//            }

            blocks[i] |= rhs.blocks[i];

//            unsigned long long one = 1;
//            blocks[i] &= (one<<(second_idx+1))-1;
        }

//        *this = bitset<MAX_BITVECTOR_SIZE>::operator|=(rhs);
//
        if(CONTROL) {
            control |= rhs.control;

            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        return *this; // return the result by reference
    }
    Bitvector operator | (const Bitvector& other) const {

        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        Bitvector ret = Bitvector(this, true, true);
        ret |= other;

        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }


        return ret;
    }

    void my_xor_eq(const Bitvector & rhs)
    {

        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        int begin = 0;
        int end = min(get_size()-1, rhs.get_size()-1);

        int init_block = idx_to_block(0);
        int end_block = idx_to_block(end);

        for(int i = init_block; i<=end_block;i++)
        {
//            int offset = i * BLOCK_SIZE;
//            int first_idx = block_ranges[i].first-offset;
//            int second_idx = block_ranges[i].second-offset;
//            if(i == init_block)
//            {
//                first_idx = begin-offset;
//            }
//            if(i == end_block)
//            {
//                second_idx = end-offset;
//            }

            blocks[i] ^= rhs.blocks[i];

//            unsigned long long one = 1;
//            blocks[i] &= (one<<(second_idx+1))-1;

        }


//        *this = bitset<MAX_BITVECTOR_SIZE>::operator^=(rhs);

        if(CONTROL) {
//            control ^= rhs.control;
            for (int i = 0; i < size; i++) {
                control.set(i, control.test(i) ^ rhs.control.test(i));
                get_bit(i);
            }
        }
    }

    Bitvector operator ^ (const Bitvector& other) const {
        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
        Bitvector ret = Bitvector(this, true, true);
        ret.my_xor_eq(other);
        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
        return ret;
    }

    void int_flip(unsigned long long & the_int)
    {
        the_int = ~the_int;
    }

    void my_flip()
    {
        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
        int begin = 0;
        int end = size-1;

        int init_block = idx_to_block(0);
        int end_block = idx_to_block(end);

        for(int i = init_block; i<=end_block;i++)
        {
            int offset = i * BLOCK_SIZE;
            int first_idx = block_ranges[i].first-offset;
            int second_idx = block_ranges[i].second-offset;
            if(i == init_block)
            {
                first_idx = begin-offset;
            }
            if(i == end_block)
            {
                second_idx = end-offset;
            }
            int_flip(blocks[i]);
            unsigned long long one = 1ull;
            blocks[i] &= (one<<(second_idx+1))-1;
        }
        if(CONTROL) {
            for(int i = begin;i<=end;i++)
            {
                control.flip(i);
            }
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
    }

    Bitvector get_flipped()
    {
        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
        Bitvector ret = Bitvector(this, true, true);
        ret.my_flip();

//        Bitvector second_ret = Bitvector(this, true, true);
//        for(int i = 0;i<=size-1;i++)
//        {
//            second_ret.flip(i);
//        }
//
//        assert(ret == second_ret);

        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        return ret;
    }

    int count()
    {
        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
        int begin = 0;
        int end = get_size()-1;

        int init_block = idx_to_block(0);
        int end_block = idx_to_block(end);

        int sum = 0;
        for(int i = init_block; i<=end_block;i++) {
            int offset = i * BLOCK_SIZE;
            int first_idx = block_ranges[i].first-offset;
            int second_idx = block_ranges[i].second-offset;
            if(i == init_block)
            {
                first_idx = begin-offset;
            }
            if(i == end_block)
            {
                second_idx = end-offset;
            }
            sum += __builtin_popcount(int_get_subint(blocks[i], first_idx, second_idx));
        }

        if(CONTROL) {
            int contol_count = control.count();
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
            assert(contol_count == sum);
        }
        return sum;
    }

    unsigned int num_trailing_zeroes();

    bool int_test_range(unsigned long long the_int, int begin, int end)
    {
        unsigned long long one = 1;
        unsigned long long range_of_ones = ((one << (end+1))-1 - ((one << begin)-1));
        unsigned long long range = the_int & range_of_ones;
        return range == range_of_ones;
    }

    bool test_range(int begin, int end)
    {

        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        bool ret = true;

        int init_block = begin / BLOCK_SIZE;
        int end_block = idx_to_block(end);

        for(int i = init_block; i<=end_block;i++)
        {
            int offset = i * BLOCK_SIZE;
            int first_idx = block_ranges[i].first-offset;
            int second_idx = block_ranges[i].second-offset;
            if(i == init_block)
            {
                first_idx = begin-offset;
            }
            if(i == end_block)
            {
                second_idx = end-offset;
            }
            if(!int_test_range(blocks[i], first_idx, second_idx))
            {
                ret = false;
                break;
            }
        }
//
        if(CONTROL) {

            bool second_ret = true;
            for(int i = begin; i<=end;i++)
            {
                if(!control.test(i))
                {
                    second_ret = false;
                    break;
                }
            }
            assert(ret == second_ret);

            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        return ret;
    }

    void int_set_range_of_bits(unsigned long long & the_int, int begin, int end)
    {
        unsigned long long one = 1;
        the_int |= ((one << (end+1)) - 1 - ((one << begin) -1));
    }

    void int_set_bit(unsigned long long & the_int, int idx)
    {
        unsigned long long one = 1;
        the_int |= (one<<idx);
    }

    void int_unset_bit(unsigned long long & the_int, int idx)
    {
        unsigned long long one = 1;
        the_int &= (one<<BLOCK_SIZE)-1-(one<<idx);
    }

    void set_range(int begin, int end)
    {
//
        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        assert(begin <= end);

        int init_block = begin / BLOCK_SIZE;
        int end_block = idx_to_block(end);

        for(int i = init_block; i<=end_block;i++)
        {
            int offset = i * BLOCK_SIZE;
            int first_idx = block_ranges[i].first-offset;
            int second_idx = block_ranges[i].second-offset;
            if(i == init_block)
            {
                first_idx = begin-offset;
            }
            if(i == end_block)
            {
                second_idx = end-offset;
            }
            int_set_range_of_bits(blocks[i], first_idx, second_idx);
        }

        if(CONTROL) {
            for(int i = begin; i<=end;i++)
            {
                control.set(i);
            }
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
    }

    void set(int idx)
    {
        if(CONTROL) {
            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }

        int block_idx = idx / BLOCK_SIZE;
        int offset = block_idx * BLOCK_SIZE;
        int int_idx = idx-offset;

        int_set_bit(blocks[block_idx], int_idx);

        if(CONTROL) {
            //bitset<MAX_BITVECTOR_SIZE>::set(idx);

            control.set(idx);

            for (int i = 0; i < size; i++) {
                get_bit(i);
            }
        }
    }

    void set(int idx, int val)
    {
        if(CONTROL)
        {
            get_bit(idx);
        }
        int block_idx = idx / BLOCK_SIZE;
        int offset = block_idx * BLOCK_SIZE;
        int int_idx = idx-offset;

        if(val == 0) {
            int_unset_bit(blocks[block_idx], int_idx);
        }
        else if(val == 1)
        {
            int_set_bit(blocks[block_idx], int_idx);
        }
        else
        {
            assert(false);
        }

        if(CONTROL) {
            //bitset<MAX_BITVECTOR_SIZE>::set(idx, val);

            control.set(idx, val);
            get_bit(idx);
        }
    }


    void set_size(int i);

    string to_string() const
    {
        /*string to_remove_0s = bitset<MAX_BITVECTOR_SIZE>::to_string();
        string ret;
        for(int i = to_remove_0s.size()-1; i >= to_remove_0s.size()-get_size() ;i--)
        {
            ret += to_remove_0s[i];
        }
        reverse(ret.begin(), ret.end());
        for(int i = 0;i<size;i++)
        {
            get_bit(i);
        }*/

        string ret = "";
        for(int i = 0;i < size;i++)
        {
            ret += std::to_string(get_bit(i));
        }

        reverse(ret.begin(), ret.end());

        if(CONTROL)
        {
            string to_remove_0s = control.to_string();
            string second_ret = "";
            int lb = to_remove_0s.size()-get_size();
            for(int i = to_remove_0s.size()-1; i >= lb ;i--)
            {
                second_ret += to_remove_0s[i];
            }
            reverse(second_ret.begin(), second_ret.end());

            assert(second_ret == ret);

            for(int i = 0;i<size;i++)
            {
                get_bit(i);
            }
        }

        return ret;
    }

    int get_bit(int i) const;

    unsigned long long to_ullong() const;

    Bitvector get_prefix(const int &i);
};



#endif //COMPACTPOSET_BITVECTOR_H
