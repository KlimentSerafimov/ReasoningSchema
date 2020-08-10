//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "PartialFunction.h"
#include <iostream>
#include "util.h"
#include <set>
#include "BittreeTaskType.h"
#include <fstream>

PartialFunction::PartialFunction() = default;

PartialFunction::PartialFunction(int _function_size, Bitvector _total_function) {
    function_size = _function_size;
    total_function = _total_function;
    partition.set_range(0, function_size-1);
}

PartialFunction::PartialFunction(int _function_size, Bitvector _total_function, Bitvector _partition) {
    function_size = _function_size;
    total_function = _total_function;
    partition = _partition;
}

string PartialFunction::to_string() {
    string ret;
    for(int i = function_size-1;i>=0;i--)
    {
        ret += bit_to_string(i);
    }
    return ret;
}

bool PartialFunction::is_contained_in(PartialFunction other_partial_function) {
    assert(function_size == other_partial_function.function_size);
    if((partition & other_partial_function.partition) == other_partial_function.partition) {
        Bitvector left = (total_function & other_partial_function.partition);
        Bitvector right = (other_partial_function.total_function & other_partial_function.partition);
        bool ret = (left == right);
        return ret;
    } else
    {
        return false;
    }
}

PartialFunction PartialFunction::get_composition(PartialFunction other) {
    assert(function_size == other.function_size);

    Bitvector common_partition = partition & other.partition;

    assert((total_function & common_partition) == (other.total_function & common_partition));

    Bitvector this_part = total_function & other.partition.get_flipped();
    Bitvector other_part = other.total_function & partition.get_flipped();
    Bitvector common_part = total_function & common_partition;

    return PartialFunction(function_size, this_part | common_part | other_part, partition | other.partition);
}

int PartialFunction::has_output(int idx) {
    return partition.get_bit(idx);
}

int PartialFunction::get_output(int idx) {
    return total_function.get_bit(idx);
}

void PartialFunction::apply_common_partition(PartialFunction other) {

    Bitvector init_partition = partition;
    Bitvector init_other_partition = other.partition;
    Bitvector init_total_function = total_function;
    Bitvector init_other_total_function = other.total_function;

    partition &= other.partition;
    Bitvector intermediate_partition = partition;
    Bitvector left = (partition & total_function);
    Bitvector right = (partition & other.total_function );
    Bitvector the_xor = left ^ right;
    Bitvector the_xor_flipped = the_xor.get_flipped();
    partition &= the_xor_flipped;

    Bitvector masked_this = (partition & total_function);
    Bitvector masked_other = (partition & other.total_function);

    assert(masked_this == masked_other);
}

int PartialFunction::partition_size() {
    return partition.count();
}

void PartialFunction::set_bit(int idx, int value) {
    if(!get_bit(partition, idx))
    {
        partition.set(idx);
    }
    total_function.set(idx, value);
    assert(get_bit(total_function, idx) == value);
}

void PartialFunction::clear_bit(int idx) {
    assert(get_bit(partition, idx) == 1);
    partition.set(idx, 0);
}

bool PartialFunction::empty() {
    return partition == 0;
}

bool PartialFunction::has_empty_intersection_with(PartialFunction other) {
    Bitvector common_partition = partition & other.partition;
    Bitvector difference_mask = ((total_function & common_partition) ^ (other.total_function & common_partition));
    return !(difference_mask == Bitvector(0, difference_mask.get_size()));
}

void PartialFunction::append_difference_with(PartialFunction other, vector<PartialFunction> &to_append_to) {

    if (!has_empty_intersection_with(other)) {
        Bitvector template_for_output__partition = partition;
        Bitvector template_for_output__function = total_function & partition;

        Bitvector other_contains_but_this_doesnt = other.partition ^ (other.partition & partition);

        while(!(other_contains_but_this_doesnt == 0))
        {
            int idx_of_first_one = num_trailing_zeroes(other_contains_but_this_doesnt);

            template_for_output__partition.set(idx_of_first_one);

            Bitvector new_template_for_output__function = template_for_output__function;
            new_template_for_output__function.set(idx_of_first_one, 1-get_bit(other.total_function, idx_of_first_one));

            to_append_to.push_back(
                    PartialFunction(
                            function_size,
                            new_template_for_output__function,
                            template_for_output__partition
                            )
                    );

            template_for_output__function.set(idx_of_first_one, get_bit(other.total_function, idx_of_first_one));

            other_contains_but_this_doesnt.set(idx_of_first_one, 0);
        }
    }
    else
    {
        to_append_to.push_back(PartialFunction(function_size, total_function, partition));
    }
}

void PartialFunction::append_intersection_with(PartialFunction other, vector<PartialFunction> &to_append_to) {
    if(!has_empty_intersection_with(other))
    {
        to_append_to.push_back(
                PartialFunction(
                        function_size,
                        (total_function & partition) | (other.total_function & other.partition),
                        partition | other.partition
                        )
                );
    }
}

bool PartialFunction::full()
{
    return partition.test_range(0, function_size - 1);
}

PartialFunction::PartialFunction(vector<BitInBittree*> _bits)
{
    init_via_bits(_bits);
}

void PartialFunction::init_via_bits(vector<BitInBittree*> _bits)
{
    bits = _bits;
    function_size = 0;
    total_function = Bitvector(0);
    partition = 0;
    set<int> bit_ids;
    vector<BitInBittree*> remaining_bits;
    for(int i = 0;i<bits.size();i++)
    {
        if(bit_ids.find(bits[i]->bit_id) == bit_ids.end())
        {
            bit_ids.insert(bits[i]->bit_id);
            remaining_bits.push_back(bits[i]);
            if (bits[i]->is_bit_set) {
                partition.set(function_size, 1);
                total_function.set(function_size, bits[i]->bit_val);
            }
            function_size++;
        }
    }
    total_function.set_size(function_size);
    bits = remaining_bits;
    assert(bits.size() == function_size);
}

bool PartialFunction::operator < (const PartialFunction& other) const {
    if(total_function == other.total_function)
    {
        if(partition == other.partition)
        {
            return function_size < other.function_size;
        }
        return partition < other.partition;
    }
    return total_function < other.total_function;
}

bool PartialFunction::operator == (const PartialFunction& other) const {
    if(total_function == other.total_function)
    {
        if(partition == other.partition)
        {
            return function_size == other.function_size;
        }
        return false;
    }
    return false;
}

string PartialFunction::bit_to_string(int i) {
    string ret = "";
    if(partition.get_bit(i))
    {
        ret+='0'+get_bit(total_function, i);
    }
    else
    {
        ret+="_";
    }
    return ret;
}

BittreeTaskTypeAsPartialFunction::BittreeTaskTypeAsPartialFunction(BittreeTaskType *_bittree_taks_type, int _subtask_depth)
{
    //bittree_task_type = new BittreeTaskType(nullptr, Name("copy_for_print"),_bittree_taks_type, true);
    bittree_task_type = _bittree_taks_type;
    subtask_depth = _subtask_depth;
    vector<BitInBittree*> partial_bits;
    BittreeTaskType* local_subtask = bittree_task_type;
    int num_prev_subtasks = 0;
    while(local_subtask != nullptr)
    {
        if(num_prev_subtasks < subtask_depth || subtask_depth == -1 || subtask_depth == 0 || local_subtask->solution == nullptr)
        {
            memset_visited(vis_bits);
            local_subtask->append_IO_bits(partial_bits);

            if(subtask_depth == 0)
            {
                break;
            }

            if(local_subtask->decomposition != nullptr)
            {
                local_subtask = local_subtask->decomposition->subtask;
            }
            else
            {
                local_subtask = nullptr;
            }
        } else
        {
            memset_visited(vis_bits);
            local_subtask->solution->append_IO_bits(partial_bits);
            if(local_subtask->decomposition != nullptr)
            {
                local_subtask = local_subtask->decomposition->subtask;
            }
            else
            {
                local_subtask = nullptr;
            }
        }
        if(num_prev_subtasks == subtask_depth && subtask_depth != -1)
        {
            break;
        }
        num_prev_subtasks+=1;
    }

    init_via_bits(partial_bits);

    //bittree_task_type = new BittreeTaskType(nullptr, Name("copy_for_print"),_bittree_taks_type, true);
}

BittreeTaskTypeAsPartialFunction::BittreeTaskTypeAsPartialFunction(const PartialFunction& partial_function) {

    function_size = partial_function.function_size;
    total_function = partial_function.total_function;
    partition = partial_function.partition;

    bits = partial_function.bits;
}

string BittreeTaskTypeAsPartialFunction::to_string__one_line() {
    return bittree_task_type->to_string__one_line(subtask_depth);
}

BittreeTaskTypeAsPartialFunction::BittreeTaskTypeAsPartialFunction(vector<BitInBittree *> _bits,
                                                                   BittreeTaskType *_bittree_taks_type,
                                                                   int _subtask_depth) {

    bits = std::move(_bits);
    init_via_bits(bits);
    bittree_task_type = _bittree_taks_type;
    subtask_depth = _subtask_depth;
}

void BittreeTaskTypeAsPartialFunction::assign_bits(Bitvector &bitvector) {

    assert(bits.size() == function_size);
    assert(bitvector.get_size() == bits.size());

    for(int k = 0; k < bits.size(); k++)
    {
        bits[k]->is_bit_set = true;
        bits[k]->bit_val = bitvector.get_bit(k);
    }

}

vector<string> BittreeTaskTypeAsPartialFunction::get_path_of_bit_id(int bit_id) {
    
    TreeNode *at = bits[bit_id]->get_last_parent();

    vector<string> path;

    vector<int> coord;
//    cout << at->get_last_name().to_string() <<" ";
    path.push_back(at->get_last_name().to_string());
    while (at->get_last_name().get_is_child()) {
        coord.push_back(at->get_last_name().get_id());
        at = at->get_last_parent();
        path.push_back(at->get_last_name().to_string());
//        cout << at->get_last_name().to_string() <<" ";
        if (!at->get_last_name().get_is_child()) {
            if (at->get_last_name().to_string() == "input") {
            }
        }
    }
//    cout << endl;

    return path;
}

void BittreeTaskTypeAsPartialFunction::update_bitvector() {
    init_via_bits(bits);
}

vector<MaskAndCost> BittreeTaskTypeAsPartialFunction::generate_variety(ofstream * fout) {
    return bittree_task_type->generate_variety(subtask_depth, fout);
}

CanvasAsPartialFunction::CanvasAsPartialFunction(BittreeNode *_canvas)
{
    canvas = _canvas;
    vector<BitInBittree*> partial_bits;

    memset_visited(vis_bits);
    canvas->append_bits(partial_bits);

    init_via_bits(partial_bits);
}
