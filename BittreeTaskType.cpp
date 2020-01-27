//
// Created by Kliment Serafimov on 1/11/20.
//

#include "BittreeTaskType.h"
#include "MetaExample.h"
#include <iostream>
#include <set>

using namespace std;

void BittreeTypeNode::init()
{
}

BittreeTypeNode::BittreeTypeNode(TreeNode *parent, Name name, NodeType _node_type) : TreeNode(parent, name, this)
{
//    assert(_node_type == internal_node);
    init();
    node_type = _node_type;
    leaf_node_type = not_leaf_node;
};

BittreeTypeNode::BittreeTypeNode(TreeNode *parent, Name name, NodeType _node_type, BitInBittreeType bit_type)
        : TreeNode(parent, name, this)
{
    assert(_node_type == leaf_node);
    init();
    node_type = _node_type;
    if(node_type == leaf_node)
    {
        bit = new BitInBittree(this, Name("bit"), bit_type);
    }
    leaf_node_type = bit_node;
};

BittreeTypeNode::BittreeTypeNode(TreeNode *parent, Name name, NodeType _node_type,
                                 BittreeTypeLeafNodeType _leaf_node_type) : TreeNode(parent, name, this)
{
    assert(_node_type == leaf_node);
    assert(_leaf_node_type == double_node);
    init();
    leaf_node_type = _leaf_node_type;
}

void BittreeTypeNode::copy_leaf_node(BittreeTypeNode *to_copy, bool all_new_bits)
{
    copy_leaf_node(to_copy, all_new_bits, false);
}

void BittreeTypeNode::copy_leaf_node(BittreeTypeNode *to_copy, bool all_new_bits, bool hard_pointer_assign_bits)
{
    if(to_copy->bit != NULL) {
        if(hard_pointer_assign_bits)
        {
            bit = to_copy->bit;
            bit->parents.push_back(this);
            bit->names.push_back(Name("bit"));
        }
        else {
            if (to_copy->bit->is_bit_set && to_copy->bit->bit_type == shared_machine_bit) {
                bit = to_copy->bit;
            } else {
                if (to_copy->bit->bit_type == new_blanko_bit || to_copy->bit->bit_type == new_machine_bit) {
                    bit = new BitInBittree(this, Name("bit"), new_machine_bit, to_copy->bit);
                } else if (to_copy->bit->bit_type == shared_machine_bit) {
                    if (!all_new_bits) {
                        bit = to_copy->bit;
                        bit->parents.push_back(this);
                    } else {
                        bit = new BitInBittree(this, Name("bit"), shared_machine_bit, to_copy->bit);
                    }
                } else {
                    assert(to_copy->bit->bit_type == shared_blanko_bit);
                    bit = new BitInBittree(this, Name("bit"), shared_machine_bit, to_copy->bit);
                }
            }
        }
    }
    else if(to_copy->delta != NULL)
    {
        delta = new BittreeTypeNode(
                this, Name("delta"), to_copy->delta, all_new_bits, hard_pointer_assign_bits);
    }
    else
    {
        assert(to_copy->leaf_node_type == double_node);
        leaf_node_type = double_node;
    }
    assert(to_copy->children.size() == 0);
}

BittreeTypeNode::BittreeTypeNode(TreeNode *parent, Name name, BittreeTypeNode *to_copy, bool all_new_bits)
        : TreeNode(parent, name, this)
{
    init();
    node_type = to_copy->node_type;
    leaf_node_type = to_copy->leaf_node_type;
    if(node_type == leaf_node)
    {
        copy_leaf_node(to_copy, all_new_bits);
    }
    else if(node_type == internal_node)
    {
        for (int i = 0; i < to_copy->children.size(); i++)
        {
            children.push_back(new BittreeTypeNode(this, Name("children", children.size()), to_copy->children.at(i),
                                                   all_new_bits));
        }
    }
    else
    {
        assert(false);
    }
};

BittreeTypeNode::BittreeTypeNode(TreeNode *parent, Name name, BittreeTypeNode *to_copy, bool all_new_bits,
                                 bool hard_pointer_assign_bits) : TreeNode(parent, name, this)
{
    init();
    node_type = to_copy->node_type;
    leaf_node_type = to_copy->leaf_node_type;
    if(node_type == leaf_node)
    {
        copy_leaf_node(to_copy, all_new_bits, hard_pointer_assign_bits);
    }
    else if(node_type == internal_node)
    {
        for (int i = 0; i < to_copy->children.size(); i++)
        {
            children.push_back(
                    new BittreeTypeNode(
                            this, Name("children", children.size()), to_copy->children.at(i), all_new_bits));
        }
    }
    else
    {
        assert(false);
    }
};

string BittreeTypeNode::to_string(int num_tabs)
{
    int next_num_tabs = num_tabs+1;
//    string node_id_str = TreeNode::to_string();
    string open_bracket = "\n"+tabs(num_tabs) + "{\n";
    string close_bracket = tabs(num_tabs) + "}";
    if(node_type == leaf_node)
    {
        if(bit != NULL)
        {
            return tabs(num_tabs)+"bit"+bit->TreeNode::to_string()+"{"+bit->to_string()+"}\n";
        }
        else if(delta != NULL)
        {
            return tabs(num_tabs)+"delta"+delta->TreeNode::to_string()+ open_bracket + delta->to_string(next_num_tabs) + close_bracket + "\n";
        }
        else
        {
            assert(leaf_node_type == double_node);
            return tabs(num_tabs)+"double_node"+"\n";
        }
    }
    else
    {
        string ret;
        assert(node_type == internal_node);
        for (int i = 0; i < children.size(); i++) {
            if (i >= 1) {
                ret += ",\n";
            }
            ret +=  tabs(num_tabs)+ "[" + std::to_string(i) + ": child]" + children.at(i)->TreeNode::to_string() + open_bracket + children.at(i)->to_string(next_num_tabs) + close_bracket;
        }
        ret += "\n";

        return ret;
    }
}


string BittreeTypeNode::bits_to_string(int num_tabs)
{
    int next_num_tabs = num_tabs+1;
//    string node_id_str = TreeNode::to_string();
    string open_bracket = "{\n";
    string close_bracket = tabs(num_tabs)+"}";
    if(node_type == leaf_node)
    {
        if(bit != NULL)
        {
            if(bit->is_bit_set)
            {
                return tabs(num_tabs)+std::to_string(bit->bit_val)+"\n";
            }
            else
            {
                return tabs(num_tabs)+"_"+"\n";
            }
        }
        else
        {
            assert(delta != NULL);
            return tabs(num_tabs)+"delta" +  open_bracket + delta->bits_to_string(next_num_tabs) + close_bracket+"\n";
        }
    }
    else
    {
        string ret;
        assert(node_type == internal_node);
        for (int i = 0; i < children.size(); i++) {
            if (i >= 1) {
                ret += ",\n";
            }
            ret +=  tabs(num_tabs)+"[" + std::to_string(i) + ": child]" + open_bracket + children.at(i)->bits_to_string(next_num_tabs) + close_bracket;
        }
        ret += "\n";

        return ret;
    }
}

void BittreeTypeNode::apply_delta(BittreeTypeNode *delta_bittree_type) {
    if(node_type == internal_node && delta_bittree_type->node_type == internal_node)
    {
        assert(children.size() == delta_bittree_type->children.size());
        for(int i = 0;i<children.size();i++)
        {
            children.at(i)->apply_delta(delta_bittree_type->children.at(i));
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
void BittreeTypeNode::append_children_from(BittreeTypeNode* new_value)
{
    if(node_type == leaf_node)
    {
        assert(new_value->children.size() == 0);
        assert(new_value->node_type == leaf_node);
    }
    if(new_value->node_type == internal_node)
    {
        assert(node_type == internal_node);
        for (int i = 0; i < new_value->children.size(); i++) {
            children.push_back(new BittreeTypeNode(this, Name("children", children.size()), new_value->children.at(i), false,
                                                   true));
        }
    }
    else
    {
        assert(new_value->node_type == leaf_node);
        assert(node_type == internal_node);
        if(new_value->leaf_node_type == double_node)
        {
            int init_children_size = children.size();
            for (int i = 0; i < init_children_size; i++)
            {
                children.push_back(
                        new BittreeTypeNode(
                                this, Name("children", children.size()), children.at(i), false));
            }
        }
        else
        {
            assert(false);
        }
    }
}

void BittreeTypeNode::append_bits(vector<BitInBittree *>& bits) {

    if(node_type == internal_node || (node_type == leaf_node && leaf_node_type == not_leaf_node))
    {
        assert(delta==NULL);
        assert(bit==NULL);
        for(int i = 0;i<children.size();i++)
        {
            children.at(i)->append_bits(bits);
        }
    }
    else if(node_type == leaf_node)
    {
        if(leaf_node_type == bit_node)
        {
            bit->append_bits(bits);
        }
        else if(leaf_node_type == delta_node)
        {
            delta->append_bits(bits);
        }
        else
        {
            assert(leaf_node_type == double_node);
            assert(false);
        }
    }
    else
    {
        assert(false);
    }
}

BittreeTypeNode *BittreeTypeNode::set_delta(NodeType delta_type_node, int num_children, BitInBittreeType bit_type) {
    assert(delta == NULL);
    assert(delta_type_node == internal_node);
    assert(node_type == leaf_node);
    assert(leaf_node_type == not_leaf_node);
    leaf_node_type = delta_node;
    delta = new BittreeTypeNode(this, Name("delta"), delta_type_node);

    assert(delta->children.size() == 0);
    for(int i = 0; i<num_children; i++)
    {
        assert(i == delta->children.size());
        delta->children.push_back(
                new BittreeTypeNode(
                        delta,Name("children", i),leaf_node, bit_type));
    }
    return delta;
}

BittreeTypeNode *BittreeTypeNode::set_delta(NodeType delta_type_node, BittreeTypeLeafNodeType delta_leaf_node_type) {
    assert(delta_leaf_node_type == double_node);
    assert(delta == NULL);
    assert(delta_type_node == leaf_node);
    assert(node_type == leaf_node);
    leaf_node_type = delta_node;
    delta = new BittreeTypeNode(this, Name("delta"), delta_type_node, delta_leaf_node_type);

    return delta;
}

BittreeTypeNode* BittreeTypeNode::add_child(NodeType node_type, BitInBittreeType bit_in_bittree_type) {
    BittreeTypeNode* new_child =
            new BittreeTypeNode(this, Name("children", children.size()), node_type, bit_in_bittree_type);
    children.push_back(new_child);
    return new_child;
}

//
//DeltaBittreeTaskType::DeltaBittreeTaskType(NodeType delta_input_type, NodeType delta_output_typee)
//{
//    delta_input = new DeltaBittreeType(delta_input_type);
//    delta_output = new DeltaBittreeType(delta_output_typee);
//}

BittreeTaskType::BittreeTaskType(): TreeNode(NULL, Name("NULL"), this)
{
}

BittreeTaskType::BittreeTaskType(TreeNode *_parent, Name name, NodeType input_node_type, NodeType output_node_type)
        : TreeNode(_parent, name, this)
{
    input = new BittreeTypeNode(this, Name("input"), input_node_type);
    output = new BittreeTypeNode(this, Name("output"), output_node_type);
}

BittreeTaskType::BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits)
        : TreeNode(_parent, name, this)
{
    input = new BittreeTypeNode(this, Name("input"), to_copy->input, all_new_bits);
    output = new BittreeTypeNode(this, Name("output"), to_copy->output, all_new_bits);
}

BittreeTaskType::BittreeTaskType(TreeNode *_parent, BittreeTaskType *to_copy, bool all_new_bits,
                                 bool hard_pointer_assign_bits,
                                 Name name) : TreeNode(_parent, name, this)
{
    assert(hard_pointer_assign_bits);
    input = new BittreeTypeNode(this, Name("input"), to_copy->input, all_new_bits,
                                hard_pointer_assign_bits);
    output = new BittreeTypeNode(this, Name("output"), to_copy->output, all_new_bits,
                                 hard_pointer_assign_bits);
}

void BittreeTaskType::append_bits(vector<BitInBittree*>& bits)
{
    append_IO_bits(bits);
    if(solution!=NULL)
    {
        solution->append_bits(bits);
    }
    if(subtask!=NULL)
    {
        subtask->append_bits(bits);
    }
    if(delta!=NULL)
    {
        delta->append_bits(bits);
    }
}

void BittreeTaskType::append_IO_bits(vector<BitInBittree*>& bits)
{
    input->append_bits(bits);
    output->append_bits(bits);
}

string BittreeTaskType::to_string()
{
    string open_bracket = "\n"+tabs(0) + "{\n";
    string ret = tabs(0)+"task"+TreeNode::to_string()+"]"+tabs(0)+open_bracket + to_string(1) + "}";
    return ret;
}

string BittreeTaskType::to_string(int num_tabs)
{
    string ret; // = tabs(num_tabs)+"task"+TreeNode::to_string()+"]\n"+tabs(num_tabs)+open_bracket;
    int next_tabs = num_tabs;
    int next_next_tabs = next_tabs+1;
    string open_bracket = "\n"+tabs(next_tabs) + "{\n";
    string close_bracket = tabs(num_tabs) + "}\n";
    ret += tabs(next_tabs)+"input"+input->TreeNode::to_string()+open_bracket + input->to_string(next_next_tabs) + close_bracket;
    ret += tabs(next_tabs)+"output"+output->TreeNode::to_string()+open_bracket + output->to_string(next_next_tabs) + close_bracket;

    if(solution != NULL)
    {
        ret += tabs(next_tabs)+"solution"+solution->TreeNode::to_string()+open_bracket;
        ret += solution->to_string(next_next_tabs);
        ret += close_bracket;
    }

    if(subtask != NULL)
    {
        ret += tabs(next_tabs)+"subtask"+subtask->TreeNode::to_string()+open_bracket;
        ret += subtask->to_string(next_next_tabs);
        ret += close_bracket;
    }

    if(delta != NULL)
    {
        ret += tabs(next_tabs)+"delta"+delta->TreeNode::to_string()+open_bracket;
        ret += delta->to_string(next_next_tabs);
        ret += close_bracket;
    }

    return ret;
};

string BittreeTaskType::bits_to_string(int num_tabs)
{
    string local_tabs = tabs(num_tabs);
    string ret;
    int next_tabs = num_tabs;
    int next_next_tabs = next_tabs+1;
    string open_bracket = "{\n";
    string close_bracket = tabs(num_tabs) + "}\n";
    ret += tabs(next_tabs)+"input"+open_bracket + input->bits_to_string(next_next_tabs) + close_bracket;
    ret += tabs(next_tabs)+"output"+open_bracket + output->bits_to_string(next_next_tabs) + close_bracket;

    if(solution != NULL)
    {
        ret += tabs(next_tabs)+"solution"+open_bracket;
        ret += solution->bits_to_string(next_next_tabs);
        ret += close_bracket;
    }

    if(subtask != NULL)
    {
        ret += tabs(next_tabs)+"subtask"+open_bracket;
        ret += subtask->bits_to_string(next_next_tabs);
        ret += close_bracket;
    }

    if(delta != NULL)
    {
        ret += tabs(next_tabs)+"delta"+open_bracket;
        ret += delta->bits_to_string(next_next_tabs);
        ret += close_bracket;
    }

    return ret;
}

void BittreeTaskType::apply_delta(BittreeTaskType* type) {
    input->apply_delta(type->input);
    output->apply_delta(type->output);
}

//BittreeTaskType BittreeTaskType::get_delta_application(BittreeTaskType* type)
//{
//    BittreeTaskType ret = BittreeTaskType(this, false);
//    ret.apply_delta(type);
//    return ret;
//}

BittreeTaskType* BittreeTaskType::get_supertask_type(BittreeTaskType* type)
{
    BittreeTaskType* ret = new BittreeTaskType(NULL, Name("root"), this, false);
    ret->delta = new BittreeTaskType(ret, Name("delta"), type, true);
    ret->apply_delta(ret->delta);
//    ret->apply_delta(type);
//    assert(false);
    ret->subtask = this;
    parents.push_back(ret);
    names.push_back(Name("subtask"));
    return ret;
}

void BittreeTaskType::solve(TaskName task_name) {

    solution = new BittreeTaskType(this, Name("solution"), this, true);
    if(subtask != NULL)
    {
        solution->subtask = subtask->solution;
    }

    if(task_name.do__sum){
        solve__sum();
    }

    if(task_name.do__greater)
    {
        solve__greater();
    }

    if(task_name.do__cummulative_binary_operator)
    {
        solve__cumulative_binary_operator();
    }

    if(task_name.do__bitwise_binary_operator)
    {
        solve__bitwise_binary_operator();
    }

    if(task_name.do__multiply_by)
    {
        solve__multiply_by(task_name.param__multiply_by);
    }

    if(task_name.do__one_shift_idx)
    {
        solve__one_shift_idx();
    }

    if(task_name.do__one_shift_idx__reverse_subtask)
    {
        solve__one_shift_idx__reverse_subtask(task_name.param__init_size);
    }

    if(task_name.do__count_unary)
    {
        solve__count_unary();
    }

    if(task_name.do__count_unary__reverse_subtask)
    {
        solve__count_unary__reverse_subtask(task_name.param__init_size);
    }

    if(task_name.do__unary_sum)
    {
        solve__unary_sum();
    }

    if(task_name.do__least_set_bit)
    {
        solve__least_set_bit();
    }

    if(task_name.do__max_window_between_bits)
    {
        solve__max_window_between_bits();
    }
}

void BittreeTaskType::solve__max_window_between_bits()
{
    const int num_operands = 1;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }

    int prev_set_bit = -1;
    pair<int, pair<int, int> > max_window = make_pair(-1, make_pair(-1, -1));
    for(int i = 0; i<solution->input->children.at(0)->children.size();i++)
    {
        if(get_bit(operands[0], i))
        {
            if(prev_set_bit == -1)
            {
                prev_set_bit = i;
            } else
            {
                max_window = max(make_pair(i-prev_set_bit+1, make_pair(prev_set_bit, i)), max_window);
                prev_set_bit = i;
            }
        }
    }

    Bitvector sum = 0;

    if(max_window.first != -1)
    {
        sum.set_range(max_window.second.first, max_window.second.second);
    }

    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__least_set_bit()
{
    const int num_operands = 1;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    int edge_case_bit = 0;
    if(operands[0] == 0)
    {
        edge_case_bit = 1;
    }
    else
    {
        sum.set(num_trailing_zeroes(operands[0]));
    }

    assert(solution->output->children.at(0)->children.at(0)->bit->is_bit_set == false);
    solution->output->children.at(0)->children.at(0)->bit->is_bit_set = true;
    solution->output->children.at(0)->children.at(0)->bit->bit_val = edge_case_bit;

    for(int i = 0;i<solution->output->children.at(1)->children.size();i++)
    {
        assert(solution->output->children.at(1)->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(1)->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(1)->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__unary_sum()
{
    const int num_operands = 2;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    assert(operands[0]+operands[1] < solution->output->children.size());
    sum.set(operands[0]+operands[1]);
    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__count_unary__reverse_subtask(int init_size)
{
    const int num_operands = 1;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    sum.set_range(0, (operands[0] << (init_size - solution->input->children.at(0)->children.size())));
    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__one_shift_idx__reverse_subtask(int init_size)
{
    const int num_operands = 1;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    sum.set(operands[0] << (init_size - solution->input->children.at(0)->children.size()));
    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__count_unary()
{
    const int num_operands = 1;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    sum.set_range(0,operands[0]-1);
    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__one_shift_idx()
{
    const int num_operands = 1;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    sum.set(operands[0]);
    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__multiply_by(int multiply_by)
{
    const int num_operands = 1;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int sum = operands[0] * multiply_by;
    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__bitwise_binary_operator()
{
    const int num_operands = 2;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int sum = operands[0] ^ operands[1];
    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__cumulative_binary_operator()
{
   const int num_operands = 1;
   assert(solution->input->children.size() == num_operands);
   int operands[num_operands] = {1};
   for(int i = 0;i<num_operands;i++)
   {
       int pow = 1;
       for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
       {
           assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
           operands[i]&=solution->input->children.at(i)->children.at(j)->bit->bit_val;
       }
   }
   int culumative_binary_operator = operands[0];
   for(int i = 0;i<solution->output->children.size();i++)
   {
       assert(solution->output->children.at(i)->bit->is_bit_set == false);
       solution->output->children.at(i)->bit->is_bit_set = true;
       solution->output->children.at(i)->bit->bit_val = get_bit(culumative_binary_operator, i);
   }
}


void BittreeTaskType::solve__greater()
{
    const int num_operands = 2;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int is_0th_greater_than_1st = operands[0] > operands[1];
    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(is_0th_greater_than_1st, i);
    }
}

void BittreeTaskType::solve__sum()
{
    const int num_operands = 2;
    assert(solution->input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<solution->input->children.at(i)->children.size();j++)
        {
            assert(solution->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*solution->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int sum = operands[0] + operands[1];
    for(int i = 0;i<solution->output->children.size();i++)
    {
        assert(solution->output->children.at(i)->bit->is_bit_set == false);
        solution->output->children.at(i)->bit->is_bit_set = true;
        solution->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

PartialFunction BittreeTaskType::to_partial_function() {
    vector<BitInBittree*> partial_bits;
    memset_visited(vis_bits);
    append_bits(partial_bits);
    return PartialFunction(partial_bits);
}

MetaExample BittreeTaskType::to_meta_example(int id) {
    if(solution != NULL) {
        return MetaExample(to_partial_function(), solution->to_partial_function(), id);
    }
    else
    {
        PartialFunction partial_function = to_partial_function();
        return MetaExample(to_partial_function(), partial_function, id);
    }
}

MetaExample BittreeTaskType::to_meta_example_of_subtask_decomposition(int id, int subtask_depth) {
    auto partial_function = BittreeTaskTypeAsPartialFunction(this, subtask_depth);
    auto total_function = BittreeTaskTypeAsPartialFunction(solution, subtask_depth);
    return MetaExample(partial_function, total_function, id);
}

BittreeTypeNode* BittreeTaskType::add_input_child(NodeType child_type) {
    assert(input != NULL);
    BittreeTypeNode* new_child = new BittreeTypeNode(input, Name("children", input->children.size()), child_type);
    input->children.push_back(new_child);
    return new_child;
}

BittreeTypeNode* BittreeTaskType::add_output_child(NodeType child_type) {
    assert(output != NULL);
    BittreeTypeNode* new_child =
            new BittreeTypeNode(
                    output, Name("children", input->children.size()), child_type);
    output->children.push_back(new_child);
    return new_child;
}

BittreeTypeNode* BittreeTaskType::add_output_child(NodeType child_type, BitInBittreeType bit_in_bittree_type) {
    assert(output != NULL);
    BittreeTypeNode* new_child =
            new BittreeTypeNode(
                    output, Name("children", input->children.size()), child_type, bit_in_bittree_type);
    output->children.push_back(new_child);
    return new_child;
}

void BittreeProgram::populate_bittree_programs()
{
    if(is_root)
    {
        bittree_programs.push_back(this);
    }

    bool enter = false;
    if (node->bittree_type_node != NULL) {
        BittreeTypeNode *local_bittree = node->bittree_type_node;
        enter = true;
        if (local_bittree->node_type == internal_node ||
            (local_bittree->node_type == leaf_node && local_bittree->leaf_node_type == not_leaf_node)) {
            assert(local_bittree->delta == NULL);
            assert(local_bittree->bit == NULL);
            for (int i = 0; i < local_bittree->children.size(); i++) {
                bittree_programs.push_back(new BittreeProgram(local_bittree->children[i], root, rec_depth));
            }
        } else if (local_bittree->node_type == leaf_node) {
            if (local_bittree->leaf_node_type == bit_node) {
                bittree_programs.push_back(new BittreeProgram(local_bittree->bit, root, rec_depth));
            } else if (local_bittree->leaf_node_type == delta_node) {
                bittree_programs.push_back(new BittreeProgram(local_bittree->delta, root, rec_depth));
            } else {
                assert(false);
            }
        } else {
            assert(false);
        }
    }
    if (node->bittree_task_type != NULL) {
        assert(!enter);
        enter = true;
        BittreeTaskType *local_bittree = node->bittree_task_type;
        assert(local_bittree->input != NULL);
        bittree_programs.push_back(new BittreeProgram(local_bittree->input, root, rec_depth));
        assert(local_bittree->output != NULL);
        bittree_programs.push_back(new BittreeProgram(local_bittree->output, root, rec_depth));
        if (local_bittree->delta != NULL) {
            bittree_programs.push_back(new BittreeProgram(local_bittree->delta, root, rec_depth));
        }
        if (local_bittree->subtask != NULL) {
            bittree_programs.push_back(new BittreeProgram(local_bittree->subtask, root, rec_depth));
        }
//        if (local_bittree->solution != NULL) {
//            bittree_programs.push_back(new BittreeProgram(local_bittree->solution, root, rec_depth));
//        }
    }
    if (node->bit_in_bittree != NULL) {
        assert(!enter);
        enter = true;
        BitInBittree *local_bittree = node->bit_in_bittree;
    }

    assert(enter);
}

//delta u subtask->delta u subtask->subtask->delta u ...
//


void BittreeProgram::recurse_on_bittree_programs()
{
    for (int i = 0; i < bittree_programs.size(); i++) {
        if (bittree_programs[i]->node != NULL) {
            vector<BitInBittree *> bits;
            memset_visited(vis_bits);
//            cout << "AFTER MEMSET VISITED" << endl;
//            cout << "APPEND BITS HERE" << endl;
            bittree_programs[i]->node->append_bits(bits);
//            cout << "END APPEND BITS" << endl;
            for (int j = 0; j < bits.size(); j++) {
                bits[j]->flip();
            }
//            cout << bittree_programs[i]->node->root_to_node_path__to__string() << " :: " << endl;

            PartialFunction local_partial_function = root->to_partial_function();
            assert(local_partial_function.full());
            bittree_masks_as_bitvectors.push_back(local_partial_function.total_function);
            cout << root->to_partial_function().to_string() << " rec_depth " << rec_depth << " :" << endl;

            if(rec_depth>=2)
            {
                memset_visited(vis_type, rec_depth - 1);
                next_rec_programs.push_back(
                        new BittreeProgram(bittree_programs[i]->node, root, rec_depth - 1));
                cout << endl;
            }
            else
            {
                next_rec_programs.push_back(NULL);
            }

//        cout << root->bits_to_string(0) << endl;
            for (int j = 0; j < bits.size(); j++) {
                bits[j]->flip();
            }
        }
        else
        {
            if(rec_depth>=2)
            {
                next_rec_programs.push_back(NULL);
            }
            else
            {
                next_rec_programs.push_back(NULL);
            }
        }
    }
    assert(next_rec_programs.size() == bittree_programs.size());
}

void BittreeProgram::extract_partial_functions(vector<Bitvector> &ret)
{
    for(int i = 0; i < bittree_masks_as_bitvectors.size(); i++)
    {
        ret.push_back(bittree_masks_as_bitvectors[i]);
        if(bittree_programs[i]!=NULL && bittree_programs[i] != this)
        {
            bittree_programs[i]->extract_partial_functions(ret);
        }
        if(next_rec_programs[i]!=NULL)
        {
            next_rec_programs[i]->extract_partial_functions(ret);
        }
    }
}

BittreeProgram::BittreeProgram(TreeNode *_node, BittreeTaskType *_root, int _rec_depth)
{
    rec_depth = _rec_depth;
    if(rec_depth == 0)
    {
        assert(false);
        return;
    }
    if(_node->visited(vis_type, rec_depth))
    {
        return;
    }
    else
    {
        _node->visit(vis_type, rec_depth);
    }

    node = _node;

    is_root = _root == NULL;

    if(is_root)
    {
        assert(node->bittree_task_type != NULL);
        root = node->bittree_task_type;
        vector<BitInBittree*> bits;
        memset_visited(vis_bits);
        root->append_bits(bits);
        for(int j = 0;j<bits.size();j++)
        {
            bits[j]->set(0);
        }
    }
    else
    {
        root = _root;
    }

    populate_bittree_programs();

    recurse_on_bittree_programs();

    if(is_root)
    {
        assert(all_bittree_masks_as_bitvectors.size() == 0);
        extract_partial_functions(all_bittree_masks_as_bitvectors);
        set<Bitvector> uniques;
        for(int i = 0; i < all_bittree_masks_as_bitvectors.size(); i++)
        {
            uniques.insert(all_bittree_masks_as_bitvectors[i]);
        }
        all_bittree_masks_as_bitvectors.clear();
        cout << "----------------------------" << endl;
        for(auto item: uniques)
        {

            all_bittree_masks_as_bitvectors.push_back(item);
            cout << item.to_string() << endl;
        }
        cout << "all_partial_functions.size() " << all_bittree_masks_as_bitvectors.size() << endl;
    }
}


