//
// Created by Kliment Serafimov on 1/8/20.
//

#ifndef COMPACTPOSET_BITVECTORTASKS_H
#define COMPACTPOSET_BITVECTORTASKS_H

#include "PartialFunction.h"
#include "MetaExample.h"
#include "util.h"
#include <iostream>

class BittreeTypeNode;

class DeltaBittreeType
{
public:
    NodeType node_type;

    //if node_type == internal_node
    vector<DeltaBittreeType*> children;
    
    //if node_type == leaf_node;
    BittreeTypeNode* delta = NULL;

    DeltaBittreeType(NodeType delta_node_type);
};

static int global_bit_id = 0;

enum BitType {new_machine_bit, shared_machine_bit, shared_blanko_bit, new_blanko_bit};

class Bit
{
public:
    BitType bit_type;

    int bit_id;
    bool is_bit_set = false;
    int bit_val;
    Bit(BitType _bit_type)
    {
        bit_type = _bit_type;
        if(bit_type == new_machine_bit || bit_type == shared_machine_bit)
        {
            bit_id = global_bit_id;
            global_bit_id++;
        }
    }
};

class BittreeTypeNode
{
public:
    NodeType node_type;

    //if internal_node
    vector<BittreeTypeNode*> children;

    Bit *bit = NULL;
    //if bit_node;
    //no additional information needed;

    BittreeTypeNode(NodeType _node_type)
    {
        assert(_node_type == internal_node);
        node_type = _node_type;
    };

    BittreeTypeNode(NodeType _node_type, BitType bit_type)
    {
        assert(_node_type == leaf_node);
        node_type = _node_type;
        if(node_type == leaf_node)
        {
            bit = new Bit(bit_type);
        }
    };

    BittreeTypeNode(BittreeTypeNode* to_copy, bool all_new_bits)
    {
        node_type = to_copy->node_type;
        if(node_type == leaf_node)
        {
            if(to_copy->bit->is_bit_set && to_copy->bit->bit_type == shared_machine_bit)
            {
                bit = to_copy->bit;
            }
            else
            {
                if (to_copy->bit->bit_type == new_blanko_bit || to_copy->bit->bit_type == new_machine_bit)
                {
                    bit = new Bit(new_machine_bit);
                }
                else if (to_copy->bit->bit_type == shared_machine_bit)
                {
                    if (!all_new_bits)
                    {
                        bit = to_copy->bit;
                    }
                    else
                    {
                        bit = new Bit(shared_machine_bit);
                    }
                }
                else
                {
                    assert(to_copy->bit->bit_type == shared_blanko_bit);
                    bit = new Bit(shared_machine_bit);
                }
            }
            assert(to_copy->children.size() == 0);
        }
        for(int i = 0;i<to_copy->children.size();i++)
        {
            children.push_back(new BittreeTypeNode(to_copy->children[i], all_new_bits));
        }
    };


    string to_string()
    {
        if(node_type == leaf_node)
        {
            if(!bit->is_bit_set) {
                if (bit->bit_type == new_machine_bit || bit->bit_type == shared_machine_bit) {
                    return "b[" + std::to_string(bit->bit_id) + "]";
                } else {
                    return "b[]";
                }
            }
            else
            {
                return std::to_string(bit->bit_val);
            }
        }
        else
        {
            assert(node_type == internal_node);
            string ret;
            bool all_bits = true;
            for(int i = 0;i<children.size();i++)
            {
                all_bits &= (children[i]->node_type == leaf_node);
            }
            if(!all_bits) {
                for (int i = 0; i < children.size(); i++) {
                    if (i >= 1) {
                        ret += ",";
                    }
                    ret += "child[" + std::to_string(i) + "]={" + children[i]->to_string() + "}";
                }
            }
            else
            {
                for (int i = 0; i < children.size(); i++) {
                    ret += children[i]->to_string();
                }
            }
            return ret;
        }
    }

    void apply_delta(DeltaBittreeType *delta_bittree_type) {
        if(node_type == internal_node && delta_bittree_type->node_type == internal_node)
        {
            assert(children.size() == delta_bittree_type->children.size());
            for(int i = 0;i<children.size();i++)
            {
                children[i]->apply_delta(delta_bittree_type->children[i]);
            }
        }
        else
        {
            if(node_type == leaf_node)
            {
                //too early to think of recursive extensions.
                assert(false);
//                if(delta_bittree_type->node_type == internal_node)
//                {
//                    assert(false);
//                }
//                else
//                {
//                    assert(delta_bittree_type->node_type == leaf_node);
//                    assign_bittree_type_node(delta_bittree_type->delta);
//                }
            }
            else
            {
                assert(node_type == internal_node);
                assert(delta_bittree_type->node_type == leaf_node);
                append_children_from(delta_bittree_type->delta);
            }
        }
    }

//    void assign_bittree_type_node(BittreeTypeNode* new_value) {
//        node_type = new_value->node_type;
//        children.clear();
//        append_children_from(new_value);
//    }
    void append_children_from(BittreeTypeNode* new_value)
    {
        if(node_type == leaf_node)
        {
            assert(new_value->children.size() == 0);
        }
        for(int i = 0;i<new_value->children.size();i++)
        {
            children.push_back(new BittreeTypeNode(new_value->children[i], false));
        }
    }

    void append_bits(vector<Bit *>& bits) {
        if(node_type == leaf_node)
        {
            bits.push_back(bit);
        }
        else
        {
            for(int i = 0;i<children.size();i++)
            {
                children[i]->append_bits(bits);
            }
        }
    }
};

class DeltaBittreeTaskType
{
public:
    DeltaBittreeType* delta_input = NULL;
    DeltaBittreeType* delta_output = NULL;
    DeltaBittreeTaskType(NodeType delta_input_type, NodeType delta_output_typee)
    {
        delta_input = new DeltaBittreeType(delta_input_type);
        delta_output = new DeltaBittreeType(delta_output_typee);
    }
};

class BittreeTaskType
{
public:
    BittreeTypeNode* input = NULL;
    BittreeTypeNode* output = NULL;

    BittreeTaskType* subtask = NULL;

    BittreeTaskType() = default;
    
    BittreeTaskType(NodeType input_node_type, NodeType output_node_type)
    {
        input = new BittreeTypeNode(input_node_type);
        output = new BittreeTypeNode(output_node_type);
    }

    BittreeTaskType(BittreeTaskType* to_copy, bool all_new_bits)
    {
        input = new BittreeTypeNode(to_copy->input, all_new_bits);
        output = new BittreeTypeNode(to_copy->output, all_new_bits);
    }

    void append_bits(vector<Bit*>& bits)
    {
        input->append_bits(bits);
        output->append_bits(bits);
    }

    string to_string()
    {
        string ret;
        ret += "input={" + input->to_string() + "}\n";
        ret += "output={" + output->to_string() + "}\n";

        if(subtask != NULL)
        {
            ret += "subtask = \n";
            ret += subtask->to_string();
        }

        return ret;
    };

    void apply_delta(DeltaBittreeTaskType type) {
        input->apply_delta(type.delta_input);
        output->apply_delta(type.delta_output);
    }

    BittreeTaskType get_delta_application(DeltaBittreeTaskType type)
    {
        BittreeTaskType ret = BittreeTaskType(this, false);
        ret.apply_delta(type);
        return ret;
    }

    BittreeTaskType get_supertask_type(DeltaBittreeTaskType type)
    {
        BittreeTaskType ret = BittreeTaskType(this, false);
        ret.apply_delta(type);
        ret.subtask = this;
        return ret;
    }

    void solve_sum() {
        const int num_operands = 2;
        assert(input->children.size() == num_operands);
        int operands[num_operands] = {0, 0};
        for(int i = 0;i<num_operands;i++)
        {
            int pow = 1;
            for(int j = 0;j<input->children[i]->children.size();j++)
            {
                assert(input->children[i]->children[j]->bit->is_bit_set == true);
                operands[i]+=pow*input->children[i]->children[j]->bit->bit_val;
                pow*=2;
            }
        }
        int sum = operands[0] + operands[1];
        for(int i = 0;i<output->children.size();i++)
        {
            assert(output->children[i]->bit->is_bit_set == false);
            output->children[i]->bit->is_bit_set = true;
            output->children[i]->bit->bit_val = get_bit(sum, i);
        }
    }
};

static const int max_instance_tree_degree = 4;

class InstanceTree
{
public:
    BittreeTaskType* instance;
    DeltaBittreeTaskType* delta;

    BittreeTaskType superinstance_type;

    int num_superinstances;
    BittreeTaskType superinstances[max_instance_tree_degree];

    bool deepened = false;
    vector<InstanceTree*> superinstance_trees;

    InstanceTree(BittreeTaskType* _instance, DeltaBittreeTaskType* _delta)
    {
        instance = _instance;
        delta = _delta;

        superinstance_type = instance->get_supertask_type(*_delta);

        vector<Bit*> input_bits;

        superinstance_type.input->append_bits(input_bits);

        int num_input_bits = 0;
        for(int i = 0;i<input_bits.size();i++)
        {
            if(!input_bits[i]->is_bit_set)
            {
                num_input_bits += 1;
            }
        }

        num_superinstances = (1 << num_input_bits);

        assert(num_superinstances <= max_instance_tree_degree);

        for(int i = 0; i < num_superinstances; i++)
        {
            superinstances[i] = BittreeTaskType(&superinstance_type, true);
            superinstances[i].subtask = superinstance_type.subtask;
            cout << "---------------------------------------------------" << endl;
            cout << superinstances[i].to_string() << endl;
            vector<Bit*> local_input_bits;
            superinstances[i].input->append_bits(local_input_bits);
            int num_unassigned_bits = 0;
            for(int j = 0, k = 0;j<local_input_bits.size();j++)
            {
                if(local_input_bits[j]->is_bit_set == false)
                {
                    num_unassigned_bits++;
                    local_input_bits[j]->is_bit_set = true;
                    local_input_bits[j]->bit_val = get_bit(i, k);
                    k++;
                }
            }
            assert(num_unassigned_bits == num_input_bits);
            cout << superinstances[i].to_string() << endl;
            superinstances[i].solve_sum();
            cout << superinstances[i].to_string() << endl;
            cout << "---------------------------------------------------" << endl;
        }
    }

    void deepen()
    {
        if(deepened)
        {
            for (int i = 0; i < num_superinstances; i++) {
                superinstance_trees[i]->deepen();
            }
        }
        else
        {
            for (int i = 0; i < num_superinstances; i++) {
                superinstance_trees.push_back(new InstanceTree(&superinstances[i], delta));
            }
            deepened = true;
        }
    }
};

class BittreeTaskInstance
{
public:

    BittreeTaskType* initialization;
    DeltaBittreeType* delta;

    BittreeTaskType curriculum[5];

    BittreeTaskType* bittree_task_type;

    vector<BittreeTaskType> instances;

    BittreeTaskInstance(BittreeTaskType* _bittree_task_type)
    {
        bittree_task_type = _bittree_task_type;

        vector<Bit*> input_bits;
        vector<Bit*> output_bits;

        bittree_task_type->input->append_bits(input_bits);
        bittree_task_type->output->append_bits(output_bits);

        int num_input_bits = input_bits.size();
        int num_instances = (1<<num_input_bits);
        for(int i = 0; i<num_instances; i++)
        {
            instances.push_back(BittreeTaskType(_bittree_task_type, true));
            vector<Bit*> local_input_bits;
            instances[i].input->append_bits(local_input_bits);
            assert(local_input_bits.size() == num_input_bits);
            for(int j = 0;j<local_input_bits.size();j++)
            {
                local_input_bits[j]->is_bit_set = true;
                local_input_bits[j]->bit_val = get_bit(i, j);
            }
            cout << instances[i].to_string() << endl;
        }
    }
};


class BitvectorTasks {

    int num_inputs;
    int function_size;

    vector<PartialFunction> ordering_over_boolean_functions;
    void populate_ordering_over_boolean_functions_with_small_sum();
    void populate_ordering_over_boolean_functions_with_bitwise_and();
    MetaExample get_meta_example(PartialFunction partial_function);

public:

    BitvectorTasks(int function_size, int task_id);
    
    
    BitvectorTasks()
    {
        BittreeTaskType sum_task_type(internal_node, internal_node);

        sum_task_type.input->node_type = internal_node;
        sum_task_type.input->children.push_back(new BittreeTypeNode(internal_node));
        sum_task_type.input->children.push_back(new BittreeTypeNode(internal_node));

        sum_task_type.output->node_type = internal_node;
        sum_task_type.output->children.push_back(new BittreeTypeNode(leaf_node, new_machine_bit));

        DeltaBittreeTaskType delta_bittree_task_type(internal_node, internal_node);

        delta_bittree_task_type.delta_input->node_type = internal_node;
        delta_bittree_task_type.delta_input->children.push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children.push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children[0]->delta = new BittreeTypeNode(internal_node);
        delta_bittree_task_type.delta_input->children[0]->delta->children.push_back(new BittreeTypeNode(leaf_node, shared_blanko_bit));
        delta_bittree_task_type.delta_input->children[1]->delta = new BittreeTypeNode(internal_node);
        delta_bittree_task_type.delta_input->children[1]->delta->children.push_back(new BittreeTypeNode(leaf_node, shared_blanko_bit));

        delta_bittree_task_type.delta_output->delta->children.push_back(new BittreeTypeNode(leaf_node, new_blanko_bit));

        cout << sum_task_type.to_string() << endl;

        if(true)
        {
            BittreeTaskType curriculum[5];
            curriculum[0] = sum_task_type;
            for (int i = 0; i < 4; i++) {
                curriculum[i + 1] = curriculum[i].get_supertask_type(delta_bittree_task_type);
                cout << curriculum[i + 1].to_string() << endl;
            }

//            BittreeTaskInstance instances = BittreeTaskInstance(&curriculum[4]);
        }
        else
        {
            sum_task_type.solve_sum();
            cout << sum_task_type.to_string() << endl;
            InstanceTree instances = InstanceTree(&sum_task_type, &delta_bittree_task_type);
            instances.deepen();
            instances.deepen();
        }
    }
};

#endif //COMPACTPOSET_BITVECTORTASKS_H
