//
// Created by Kliment Serafimov on 1/11/20.
//

#include "BittreeTaskType.h"
#include "MetaExample.h"

void BittreeTypeNode::init()
{
    children = new vector<BittreeTypeNode*>();
}

BittreeTypeNode::BittreeTypeNode(NodeType _node_type)
{
    assert(_node_type == internal_node);
    init();
    node_type = _node_type;
};

BittreeTypeNode::BittreeTypeNode(NodeType _node_type, BitInBittreeType bit_type)
{
    assert(_node_type == leaf_node);
    init();
    node_type = _node_type;
    if(node_type == leaf_node)
    {
        bit = new BitInBittree(bit_type);
    }
};

BittreeTypeNode::BittreeTypeNode(BittreeTypeNode* to_copy, bool all_new_bits)
{
    init();
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
                bit = new BitInBittree(new_machine_bit);
            }
            else if (to_copy->bit->bit_type == shared_machine_bit)
            {
                if (!all_new_bits)
                {
                    bit = to_copy->bit;
                }
                else
                {
                    bit = new BitInBittree(shared_machine_bit);
                }
            }
            else
            {
                assert(to_copy->bit->bit_type == shared_blanko_bit);
                bit = new BitInBittree(shared_machine_bit);
            }
        }
        assert(to_copy->children->size() == 0);
    }
    for(int i = 0;i<to_copy->children->size();i++)
    {
        children->push_back(new BittreeTypeNode(to_copy->children->at(i), all_new_bits));
    }
};

string BittreeTypeNode::to_string()
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
        for(int i = 0;i<children->size();i++)
        {
            all_bits &= (children->at(i)->node_type == leaf_node);
        }
        if(!all_bits) {
            for (int i = 0; i < children->size(); i++) {
                if (i >= 1) {
                    ret += ",";
                }
                ret += "child[" + std::to_string(i) + "]={" + children->at(i)->to_string() + "}";
            }
        }
        else
        {
            for (int i = 0; i < children->size(); i++) {
                ret += children->at(i)->to_string();
            }
        }
        return ret;
    }
}

void BittreeTypeNode::apply_delta(DeltaBittreeType *delta_bittree_type) {
    if(node_type == internal_node && delta_bittree_type->node_type == internal_node)
    {
        assert(children->size() == delta_bittree_type->children->size());
        for(int i = 0;i<children->size();i++)
        {
            children->at(i)->apply_delta(delta_bittree_type->children->at(i));
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
//        children->clear();
//        append_children_from(new_value);
//    }
void BittreeTypeNode::append_children_from(BittreeTypeNode* new_value)
{
    if(node_type == leaf_node)
    {
        assert(new_value->children->size() == 0);
    }
    for(int i = 0;i<new_value->children->size();i++)
    {
        children->push_back(new BittreeTypeNode(new_value->children->at(i), false));
    }
}

void BittreeTypeNode::append_bits(vector<BitInBittree *>& bits) {
    if(node_type == leaf_node)
    {
        bits.push_back(bit);
    }
    else
    {
        for(int i = 0;i<children->size();i++)
        {
            children->at(i)->append_bits(bits);
        }
    }
}

DeltaBittreeTaskType::DeltaBittreeTaskType(NodeType delta_input_type, NodeType delta_output_typee)
{
    delta_input = new DeltaBittreeType(delta_input_type);
    delta_output = new DeltaBittreeType(delta_output_typee);
}

BittreeTaskType::BittreeTaskType(NodeType input_node_type, NodeType output_node_type)
{
    input = new BittreeTypeNode(input_node_type);
    output = new BittreeTypeNode(output_node_type);
}

BittreeTaskType::BittreeTaskType(BittreeTaskType* to_copy, bool all_new_bits)
{
    input = new BittreeTypeNode(to_copy->input, all_new_bits);
    output = new BittreeTypeNode(to_copy->output, all_new_bits);
}

void BittreeTaskType::append_bits(vector<BitInBittree*>& bits)
{
    input->append_bits(bits);
    output->append_bits(bits);
}

string BittreeTaskType::to_string()
{
    string ret;
    ret += "input={" + input->to_string() + "}\n";
    ret += "output={" + output->to_string() + "}\n";

    if(solution != NULL)
    {
        ret += "solution = \n";
        ret += solution->to_string();
    }

    if(subtask != NULL)
    {
        ret += "subtask = \n";
        ret += subtask->to_string();
    }

    return ret;
};

void BittreeTaskType::apply_delta(DeltaBittreeTaskType type) {
    input->apply_delta(type.delta_input);
    output->apply_delta(type.delta_output);
}

BittreeTaskType BittreeTaskType::get_delta_application(DeltaBittreeTaskType type)
{
    BittreeTaskType ret = BittreeTaskType(this, false);
    ret.apply_delta(type);
    return ret;
}

BittreeTaskType BittreeTaskType::get_supertask_type(DeltaBittreeTaskType type)
{
    BittreeTaskType ret = BittreeTaskType(this, false);
    ret.apply_delta(type);
    ret.subtask = this;
    return ret;
}

void BittreeTaskType::solve_sum() {
    solution = new BittreeTaskType(this, true);
    if(subtask != NULL)
    {
        solution->subtask = subtask->solution;
    }

    const int num_operands = 2;
    assert(solution->input->children->size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children->at(i)->children->size();j++)
        {
            assert(solution->input->children->at(i)->children->at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children->at(i)->children->at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int sum = operands[0] + operands[1];
    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}



MetaExample BittreeTaskType::to_meta_example(int id) {
    vector<BitInBittree*> partial_bits;
    append_bits(partial_bits);
    PartialFunction partial_function = PartialFunction(partial_bits);

    vector<BitInBittree*> total_bits;
    assert(solution != NULL);
    solution->append_bits(total_bits);
    PartialFunction total_function = PartialFunction(total_bits);

    return MetaExample(partial_function, total_function, id);
}

MetaExample BittreeTaskType::to_meta_example_of_subtask_decomposition(int id) {
    auto partial_function = BittreeTaskTypeAsPartialFunction(this);
    auto total_function = BittreeTaskTypeAsPartialFunction(solution);

//    vector<BitInBittree*> total_bits;
//    assert(solution != NULL);
//    local_subtask = this;
//    while(local_subtask != NULL)
//    {
//        local_subtask->solution->append_bits(total_bits);
//        local_subtask = local_subtask->subtask;
//    }
//    PartialFunction total_function = PartialFunction(total_bits);

    return MetaExample(partial_function, total_function, id);
}