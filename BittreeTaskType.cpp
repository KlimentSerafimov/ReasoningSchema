//
// Created by Kliment Serafimov on 1/11/20.
//

#include "BittreeTaskType.h"
#include "MetaExample.h"
#include <iostream>
#include <set>

using namespace std;

void BittreeNode::init()
{
}

BittreeNode::BittreeNode(TreeNode *parent, Name name, NodeType _node_type) : TreeNode(parent, name, this)
{
//    assert(_node_type == internal_node);
    init();
    node_type = _node_type;
    leaf_node_type = not_leaf_node;
};

BittreeNode::BittreeNode(TreeNode *parent, Name name, NodeType _node_type, BitInBittreeType bit_type)
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

BittreeNode::BittreeNode(TreeNode *parent, Name name, NodeType _node_type,
                         BittreeTypeLeafNodeType _leaf_node_type) : TreeNode(parent, name, this)
{
    assert(_node_type == leaf_node);
    assert(_leaf_node_type == double_node);
    init();
    leaf_node_type = _leaf_node_type;
}

void BittreeNode::copy_leaf_node(BittreeNode *to_copy, bool all_new_bits)
{
    copy_leaf_node(to_copy, all_new_bits, false);
}

void BittreeNode::copy_leaf_node(BittreeNode *to_copy, bool all_new_bits, bool hard_pointer_assign_bits)
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
                bit->parents.push_back(this);
                bit->names.push_back(Name("bit"));
            } else {
                if (to_copy->bit->bit_type == new_blanko_bit || to_copy->bit->bit_type == new_machine_bit) {
                    bit = new BitInBittree(this, Name("bit"), new_machine_bit, to_copy->bit);
                } else if (to_copy->bit->bit_type == shared_machine_bit) {
                    if (!all_new_bits) {
                        bit = to_copy->bit;
                        bit->parents.push_back(this);
                        bit->names.push_back(Name("bit"));
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
        delta = new BittreeNode(
                this, Name("delta"), to_copy->delta, all_new_bits, hard_pointer_assign_bits);
    }
    else
    {
        assert(to_copy->leaf_node_type == double_node);
        leaf_node_type = double_node;
    }
    assert(to_copy->children.size() == 0);
}

BittreeNode::BittreeNode(TreeNode *parent, Name name, BittreeNode *to_copy, bool all_new_bits)
        : TreeNode(parent, name, this)
{
    init();
    copied_from = to_copy;
    to_copy->copies.push_back(this);
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
            children.push_back(new BittreeNode(this, Name("children", children.size()), to_copy->children.at(i),
                                               all_new_bits));
        }
    }
    else
    {
        assert(false);
    }
};

BittreeNode::BittreeNode(TreeNode *parent, Name name, BittreeNode *to_copy, bool all_new_bits,
                         bool hard_pointer_assign_bits) : TreeNode(parent, name, this)
{
    init();
    copied_from = to_copy;
    to_copy->copies.push_back(this);
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
                    new BittreeNode(
                            this, Name("children", children.size()), to_copy->children.at(i), all_new_bits));
        }
    }
    else
    {
        assert(false);
    }
};

string BittreeNode::to_string(int num_tabs)
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


string BittreeNode::bits_to_string(int num_tabs)
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

void BittreeNode::apply_delta(BittreeNode *delta_bittree_type) {
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

//    void assign_bittree_type_node(BittreeNode* new_value) {
//        node_type = new_value->node_type;
//        children.clear();
//        append_children_from(new_value);
//    }
void BittreeNode::append_children_from(BittreeNode* new_value)
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
            children.push_back(new BittreeNode(this, Name("children", children.size()), new_value->children.at(i), false,
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
                        new BittreeNode(
                                this, Name("children", children.size()), children.at(i), false));
            }
        }
        else
        {
            assert(false);
        }
    }
}

void BittreeNode::append_bits(vector<BitInBittree *>& bits) {

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
//            assert(false);
        }
    }
    else
    {
        assert(false);
    }
}

BittreeNode *BittreeNode::set_delta(NodeType delta_type_node, int num_children, BitInBittreeType bit_type) {
    assert(delta == NULL);
    assert(delta_type_node == internal_node);
    assert(node_type == leaf_node);
    assert(leaf_node_type == not_leaf_node);
    leaf_node_type = delta_node;
    delta = new BittreeNode(this, Name("delta"), delta_type_node);

    assert(delta->children.size() == 0);
    for(int i = 0; i<num_children; i++)
    {
        assert(i == delta->children.size());
        delta->children.push_back(
                new BittreeNode(
                        delta,Name("children", i),leaf_node, bit_type));
    }
    return delta;
}

BittreeNode *BittreeNode::set_delta(NodeType delta_type_node, BittreeTypeLeafNodeType delta_leaf_node_type) {
    assert(delta_leaf_node_type == double_node);
    assert(delta == NULL);
    assert(delta_type_node == leaf_node);
    assert(node_type == leaf_node);
    leaf_node_type = delta_node;
    delta = new BittreeNode(this, Name("delta"), delta_type_node, delta_leaf_node_type);

    return delta;
}

BittreeNode* BittreeNode::add_child(NodeType node_type, BitInBittreeType bit_in_bittree_type) {
    BittreeNode* new_child =
            new BittreeNode(this, Name("children", children.size()), node_type, bit_in_bittree_type);
    children.push_back(new_child);
    return new_child;
}

string BittreeNode::to_string__one_line() {
    string ret = "";
    if(node_type == leaf_node)
    {
        assert(leaf_node_type == bit_node);
        ret += bit->to_string__one_line();
    }
    else if(node_type == internal_node)
    {
        assert(leaf_node_type == not_leaf_node);
        for(int i = 0;i<children.size();i++)
        {
            string local = children[i]->to_string__one_line();
            ret += local;
            if(children[i]->node_type == internal_node)
            {
                ret += " ";
            }
        }
    } else{
        assert(false);
    }
//    cout << "ret = " <<ret << endl;
    return ret;
}

void BittreeNode::push_back_child(BittreeNode *child) {
    children.push_back(child);
}

void BittreeNode::populate_leaf_internals_and_bit_ids(vector<pair<BittreeNode *, vector<int> >> & ret) {
    if(node_type == leaf_node)
    {
        assert(leaf_node_type == bit_node);
        assert(false);
    }
    else if(node_type == internal_node)
    {
        assert(leaf_node_type == not_leaf_node);
        vector<int> ids;
        for(int i = 0;i<children.size();i++)
        {
            if(children[i]->node_type == leaf_node)
            {
                assert(children[i]->leaf_node_type == bit_node);
                assert(children[i]->bit->is_bit_set);
                if(children[i]->bit->bit_val)
                {
                    ids.push_back(i);
                }
            } else{
                children[i]->populate_leaf_internals_and_bit_ids(ret);
            }
        }
        if(ids.size()>=1) {
            ret.emplace_back(make_pair(this, ids));
        }
    } else{
        assert(false);
    }
}

//BittreeInputOutputType::BittreeInputOutputType(): TreeNode(NULL, Name("NULL"), this)
//{
//}

BittreeInputOutputType::BittreeInputOutputType(TreeNode *_parent, Name name, NodeType input_node_type, NodeType output_node_type)
        : TreeNode(_parent, name, this)
{
    input = new BittreeNode(this, Name("input"), input_node_type);
    output = new BittreeNode(this, Name("output"), output_node_type);
}

BittreeInputOutputType::BittreeInputOutputType(TreeNode *_parent, Name name, BittreeInputOutputType *to_copy, bool all_new_bits)
        : TreeNode(_parent, name, this)
{
    copied_from = to_copy;
    to_copy->copies.push_back(this);
    input = new BittreeNode(this, Name("input"), to_copy->input, all_new_bits);
    output = new BittreeNode(this, Name("output"), to_copy->output, all_new_bits);
}

BittreeTaskType::BittreeTaskType(): TreeNode(NULL, Name("NULL"), this)
{
}

BittreeTaskType::BittreeTaskType(TreeNode *_parent, Name name, NodeType input_node_type, NodeType output_node_type)
        : TreeNode(_parent, name, this)
{
    io = new BittreeInputOutputType(this, Name("io"), input_node_type, output_node_type);
//    input = new BittreeNode(this, Name("input"), input_node_type);
//    output = new BittreeNode(this, Name("output"), output_node_type);
}

BittreeTaskType::BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits)
        : TreeNode(_parent, name, this)
{
    copied_from = to_copy;
    to_copy->copies.push_back(this);
    assert(to_copy->io != NULL);
    io = new BittreeInputOutputType(this, Name("io"), to_copy->io, all_new_bits);
//    if(name.to_string() == "solution")
//    {
//        if(to_copy->solution != NULL)
//        {
//            if (to_copy->solution->decomposition != NULL)
//            {
//                decomposition = new BittreeTaskDecomposition(
//                        this, Name("decomposition"), to_copy->solution->decomposition, all_new_bits);
//            }
//        }
//    }
//    else
//    {
//        if (to_copy->decomposition != NULL) {
//            decomposition = new BittreeTaskDecomposition(
//                    this, Name("decomposition"), to_copy->decomposition, all_new_bits);
//        }
//        if(to_copy->solution != NULL)
//        {
//            solution = new BittreeTaskType(this, Name("solution"), to_copy->solution, all_new_bits);
//        }
//    }
//    input = new BittreeNode(this, Name("input"), to_copy->input, all_new_bits);
//    output = new BittreeNode(this, Name("output"), to_copy->output, all_new_bits);
}

BittreeTaskType::BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits,
                                 bool copy_all): TreeNode(_parent, name, this) {
    copied_from = to_copy;
    to_copy->copies.push_back(this);
    assert(to_copy->io != NULL);
    io = new BittreeInputOutputType(this, Name("io"), to_copy->io, all_new_bits);
    if (decomposition != NULL) {
        decomposition =
                new BittreeTaskDecomposition(this, Name("decomposition"), to_copy->decomposition, all_new_bits, copy_all);
    }
    if(solution != NULL)
    {
        solution =
                new BittreeTaskType(this, Name("decomposition"), to_copy->solution, all_new_bits, copy_all);
    }
}

void BittreeTaskType::append_bits(vector<BitInBittree*>& bits)
{
    append_IO_bits(bits);
    if(solution!=NULL)
    {
        solution->append_bits(bits);
    }
    if(decomposition!=NULL)
    {
        decomposition->append_bits(bits);
    }
}

void BittreeTaskType::append_bits_of_prefix_subtree(vector<BitInBittree*>& bits, int num_subtasks)
{
    append_IO_bits(bits);
    if(solution!=NULL)
    {
        solution->append_bits(bits);
    }
    if(decomposition!=NULL && num_subtasks-1 >= 0)
    {
        decomposition->append_bits_of_prefix_subtree(bits, num_subtasks-1);
    }
}

void BittreeTaskType::append_IO_bits(vector<BitInBittree*>& bits)
{
    io->append_bits(bits);
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
    string close_bracket = tabs(next_tabs) + "}\n";
    ret += io->to_string(next_tabs);

    if(solution != NULL)
    {
        ret += tabs(next_tabs)+"solution"+solution->TreeNode::to_string()+open_bracket;
        ret += solution->to_string(next_next_tabs);
        ret += close_bracket;
    }
    if(decomposition != NULL)
    {
        ret+=decomposition->to_string(next_tabs);
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
    ret += io->bits_to_string(next_tabs);

    if(solution != NULL)
    {
        ret += tabs(next_tabs)+"solution"+open_bracket;
        ret += solution->bits_to_string(next_next_tabs);
        ret += close_bracket;
    }

    if(decomposition != NULL)
    {
        ret+=decomposition->bits_to_string(next_tabs);
    }

    return ret;
}

void BittreeTaskType::apply_delta(BittreeInputOutputType* type) {
    io->apply_delta(type);
}

//BittreeTaskType BittreeTaskType::get_delta_application(BittreeTaskType* type)
//{
//    BittreeTaskType ret = BittreeTaskType(this, false);
//    ret.apply_delta(type);
//    return ret;
//}

BittreeTaskType* BittreeTaskType::get_supertask_type(BittreeInputOutputType* delta)
{
    assert(names.size() == 1);
    BittreeTaskType* ret =
            new BittreeTaskType(
                    NULL, Name("supertask_of_"+names[0].to_string()), this, false);
    ret->decomposition = new BittreeTaskDecomposition(ret, Name("decomposition"), delta, this);
    ret->apply_delta(ret->decomposition->delta);
    return ret;
}

void BittreeTaskType::solve(TaskName task_name) {

    solution = new BittreeTaskType(this, Name("solution"), this, true);

    if(decomposition != NULL)
    {
        if(decomposition->subtask->solution != NULL)
        {
            solution->decomposition =
                    new BittreeTaskDecomposition(
                            solution, Name("decomposition"), NULL, decomposition->subtask->solution);
        }

    }
    
    solution->io->solve(task_name);
}

void BittreeInputOutputType::solve(TaskName task_name)
{
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

    if(task_name.do__add)
    {
        solve__add_by(task_name.param__add_by);
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

    if(task_name.do__max_window_between_bits_with_state)
    {
        solve__max_window_between_bits_with_state();
    }

    if(task_name.do__linear_and_or_expression)
    {
        solve__linear_and_or_expresson();
    }

    if(task_name.do__linear_and_or_nand_nor_expression)
    {
        solve__linear_and_or_nand_nor__expression();
    }

    if(task_name.do__sort_bits)
    {
        solve__sort_bits();
    }
}

void BittreeInputOutputType::solve__sort_bits()
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    int len_operands[num_operands];
    for(int i = 0;i<num_operands;i++) {
        int pow = 1;
        len_operands[i] = input->children.at(i)->children.size();
        for (int j = 0; j < input->children.at(i)->children.size(); j++) {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i] += pow * input->children.at(i)->children.at(j)->bit->bit_val;
            pow *= 2;
        }
    }

    Bitvector sorted_bits = 0;

    int num_ones = __builtin_popcount(operands[0]);

    if(num_ones >= 1)
    {
        sorted_bits.set_range(len_operands[0]-1-num_ones+1, len_operands[0]-1);
    }

    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sorted_bits, i);
    }
}

void BittreeInputOutputType::solve__linear_and_or_nand_nor__expression()
{
    const int num_operands = 3;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0, 0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }

    int rez = get_bit(operands[0], 0);
    int len = input->children.at(0)->children.size();
    for(int i = 1;i<len;i++)
    {
        if(get_bit(operands[2], i-1)) {
            if (get_bit(operands[1], i - 1)) {
                rez &= get_bit(operands[0], i);
            } else {
                rez |= get_bit(operands[0], i);
            }
        }
        else {
            if (get_bit(operands[1], i - 1)) {
                rez = !get_bit((rez & get_bit(operands[0], i)), 0);
            } else {
                rez = !get_bit((rez | get_bit(operands[0], i)), 0);
            }
        }
    }

    for(int i = 0;i<output->children.at(0)->children.size();i++)
    {
        assert(output->children.at(0)->children.at(i)->bit->is_bit_set == false);
        output->children.at(0)->children.at(i)->bit->is_bit_set = true;
        output->children.at(0)->children.at(i)->bit->bit_val = get_bit(rez, i);
    }
}

void BittreeInputOutputType::solve__linear_and_or_expresson()
{
    const int num_operands = 2;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }

    int rez = get_bit(operands[0], 0);
    int len = input->children.at(0)->children.size();
    for(int i = 1;i<len;i++)
    {
        if(get_bit(operands[1], i-1))
        {
            rez &= get_bit(operands[0], i);
        } else
        {
            rez |= get_bit(operands[0], i);
        }
    }

    for(int i = 0;i<output->children.at(0)->children.size();i++)
    {
        assert(output->children.at(0)->children.at(i)->bit->is_bit_set == false);
        output->children.at(0)->children.at(i)->bit->is_bit_set = true;
        output->children.at(0)->children.at(i)->bit->bit_val = get_bit(rez, i);
    }
}


void BittreeInputOutputType::solve__max_window_between_bits_with_state()
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }

    int prev_set_bit = -1;
    pair<int, pair<int, int> > max_window = make_pair(-1, make_pair(-1, -1));
    int len = input->children.at(0)->children.size();
    for(int i = 0; i<len;i++)
    {
        if(get_bit(operands[0], i))
        {
            if(prev_set_bit == -1)
            {
                prev_set_bit = i;
            }
            else
            {
                max_window = max(make_pair(i-prev_set_bit+1, make_pair(prev_set_bit, i)), max_window);
                prev_set_bit = i;
            }
        }
    }

    Bitvector sum_result = 0;
    Bitvector sum_counting = 0;

    if(max_window.first != -1)
    {
        sum_result.set_range(0, max_window.second.second-max_window.second.first+1-1);
    }
    if(prev_set_bit != -1)
    {
        sum_counting.set_range(0, len-1-prev_set_bit+1-1);
    }

    for(int i = 0;i<output->children.at(0)->children.size();i++)
    {
        assert(output->children.at(0)->children.at(i)->bit->is_bit_set == false);
        output->children.at(0)->children.at(i)->bit->is_bit_set = true;
        output->children.at(0)->children.at(i)->bit->bit_val = get_bit(sum_result, i);
    }
    for(int i = 0;i<output->children.at(1)->children.size();i++)
    {
        assert(output->children.at(1)->children.at(i)->bit->is_bit_set == false);
        output->children.at(1)->children.at(i)->bit->is_bit_set = true;
        output->children.at(1)->children.at(i)->bit->bit_val = get_bit(sum_counting, i);
    }
}


void BittreeInputOutputType::solve__max_window_between_bits()
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    int len_operands[num_operands];
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        len_operands[i] = input->children.at(i)->children.size();
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }

    int prev_set_bit = -1;
    pair<int, pair<int, int> > max_window = make_pair(-1, make_pair(-1, -1));
    for(int i = 0; i<input->children.at(0)->children.size();i++)
    {
        if(get_bit(Bitvector(operands[0], len_operands[0]) , i))
        {
            if(prev_set_bit == -1)
            {
                prev_set_bit = i;
            }
            else
            {
                max_window = max(make_pair(i-prev_set_bit+1, make_pair(prev_set_bit, i)), max_window);
                prev_set_bit = i;
            }
        }
    }

    Bitvector sum = 0;

    if(max_window.first != -1)
    {
        sum.set_range(0, max_window.second.second-max_window.second.first+1-1);
    }

    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeInputOutputType::solve__least_set_bit()
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
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

    assert(output->children.at(0)->children.at(0)->bit->is_bit_set == false);
    output->children.at(0)->children.at(0)->bit->is_bit_set = true;
    output->children.at(0)->children.at(0)->bit->bit_val = edge_case_bit;

    for(int i = 0;i<output->children.at(1)->children.size();i++)
    {
        assert(output->children.at(1)->children.at(i)->bit->is_bit_set == false);
        output->children.at(1)->children.at(i)->bit->is_bit_set = true;
        output->children.at(1)->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeInputOutputType::solve__unary_sum()
{
    const int num_operands = 2;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    assert(operands[0]+operands[1] < output->children.size());
    sum.set(operands[0]+operands[1]);
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeInputOutputType::solve__count_unary__reverse_subtask(int init_size)
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    sum.set_range(0, (operands[0] << (init_size - input->children.at(0)->children.size())));
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeInputOutputType::solve__one_shift_idx__reverse_subtask(int init_size)
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    sum.set(operands[0] << (init_size - input->children.at(0)->children.size()));
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeInputOutputType::solve__count_unary()
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    if(operands[0]>=1) {
        sum.set_range(0, operands[0] - 1);
    }
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeInputOutputType::solve__one_shift_idx()
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    Bitvector sum = 0;
    sum.set(operands[0]);
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}


void BittreeInputOutputType::solve__add_by(int add_by)
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int sum = operands[0] + add_by;
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeInputOutputType::solve__multiply_by(int multiply_by)
{
    const int num_operands = 1;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int sum = operands[0] * multiply_by;
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeInputOutputType::solve__bitwise_binary_operator()
{
    const int num_operands = 2;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int sum = operands[0] ^ operands[1];
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeInputOutputType::solve__cumulative_binary_operator()
{
   const int num_operands = 1;
   assert(input->children.size() == num_operands);
   int operands[num_operands] = {1};
   for(int i = 0;i<num_operands;i++)
   {
       int pow = 1;
       for(int j = 0;j<input->children.at(i)->children.size();j++)
       {
           assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
           operands[i]&=input->children.at(i)->children.at(j)->bit->bit_val;
       }
   }
   int culumative_binary_operator = operands[0];
   for(int i = 0;i<output->children.size();i++)
   {
       assert(output->children.at(i)->bit->is_bit_set == false);
       output->children.at(i)->bit->is_bit_set = true;
       output->children.at(i)->bit->bit_val = get_bit(culumative_binary_operator, i);
   }
}

void BittreeInputOutputType::solve__greater()
{
    const int num_operands = 2;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int is_0th_greater_than_1st = operands[0] > operands[1];
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(is_0th_greater_than_1st, i);
    }
}

void BittreeInputOutputType::solve__sum()
{
    const int num_operands = 2;
    assert(input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<input->children.at(i)->children.size();j++)
        {
            assert(input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int sum = operands[0] + operands[1];
    for(int i = 0;i<output->children.size();i++)
    {
        assert(output->children.at(i)->bit->is_bit_set == false);
        output->children.at(i)->bit->is_bit_set = true;
        output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

BittreeTaskTypeAsPartialFunction BittreeTaskType::to_partial_function(int num_subtasks) {
    vector<BitInBittree*> partial_bits;
    memset_visited(vis_bits);
    append_bits_of_prefix_subtree(partial_bits, num_subtasks);
    return PartialFunction(partial_bits);
}

MetaExample BittreeTaskType::to_meta_example(int id, int num_subtasks) {
    if(solution != NULL) {
        return MetaExample(to_partial_function(num_subtasks), solution->to_partial_function(num_subtasks), id);
    }
    else
    {
        BittreeTaskTypeAsPartialFunction partial_function = to_partial_function(num_subtasks);
        return MetaExample(to_partial_function(num_subtasks), partial_function, id);
    }
}

MetaExample BittreeTaskType::to_meta_example_of_subtask_decomposition(int id, int subtask_depth) {
    auto partial_function = BittreeTaskTypeAsPartialFunction(this, subtask_depth);
    auto total_function = BittreeTaskTypeAsPartialFunction(solution, subtask_depth);
    auto ret = MetaExample(partial_function, total_function, id);
    return ret;
}

string BittreeTaskType::to_string__one_line__first_part(int subtask_depth) {
    string ret = io->input->to_string__one_line();
    ret += io->output->to_string__one_line();
    int init_subtask_depth = subtask_depth;
    if(subtask_depth > 0) {
        BittreeTaskType* at_subtask = decomposition->subtask->solution;
        if(at_subtask == NULL)
        {
            at_subtask = decomposition->subtask;
        }
        while(subtask_depth>0)
        {
            ret += " ";
            ret += at_subtask->io->output->to_string__one_line();

            if(at_subtask->decomposition != NULL)
            {
                assert(at_subtask->decomposition->subtask != NULL);
                at_subtask = at_subtask->decomposition->subtask;
            } else
            {
                break;
            }
            subtask_depth-=1;
        }
    }

    return ret;
}

enum Rules {inherit_from_parent, stay, move_right, move_left, move_to_last_copy, copy_right, copy_left};
int rule_cost[7] = {0, 1, 2, 2, 4, 20, 20};

bool next_rule(vector<vector<Rules> > & rules, vector<Rules> possible_rules)
{
    for(int i = 0;i<rules.size();i++)
    {
        for(int j = 0;j<rules[i].size();j++) {
            for(int k = 0;k<possible_rules.size();k++)
            {
                if(rules[i][j] == possible_rules[k])
                {
                    if(k == possible_rules.size()-1)
                    {
                        rules[i][j] = possible_rules[0];
                    } else{
                        rules[i][j] = possible_rules[k+1];
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

vector<Bitvector> BittreeTaskType::generate_variety(int subtask_depth)
{
    BittreeNode* local_parent = new BittreeNode(NULL, Name("local_parent"), internal_node);
    local_parent->push_back_child(io->input);
    local_parent->push_back_child(io->output);

    int init_subtask_depth = subtask_depth;
    if(subtask_depth > 0) {
        BittreeTaskType* at_subtask = decomposition->subtask->solution;
        if(at_subtask == NULL)
        {
            at_subtask = decomposition->subtask;
        }
        while(subtask_depth>0)
        {
            local_parent->push_back_child(at_subtask->io->output);

            if(at_subtask->decomposition != NULL)
            {
                assert(at_subtask->decomposition->subtask != NULL);
                at_subtask = at_subtask->decomposition->subtask;
            } else
            {
                break;
            }
            subtask_depth-=1;
        }
    }

    subtask_depth = init_subtask_depth;

    string init_str = local_parent->to_string__one_line();
    cout << "HERE: " << init_str << endl;

    vector<pair<BittreeNode*, vector<int> > > leaf_internals_and_bit_ids;

    local_parent->populate_leaf_internals_and_bit_ids(leaf_internals_and_bit_ids);

    for(int i = 0;i<leaf_internals_and_bit_ids.size();i++)
    {
        for(int j = 0;j<leaf_internals_and_bit_ids[i].second.size();j++) {
            cout << leaf_internals_and_bit_ids[i].second[j] << " ";
        }
        cout << endl;
    }

    vector<Rules> possible_rules;
    possible_rules.push_back(stay);
    possible_rules.push_back(move_right);
    possible_rules.push_back(move_left);
//    possible_rules.push_back(move_to_last_copy);
//    possible_rules.push_back(copy_right);
//    possible_rules.push_back(copy_left);

    vector<vector<Rules> > rules;

    for(int i = 0;i<leaf_internals_and_bit_ids.size();i++)
    {
        rules.emplace_back();
        for(int j = 0;j<leaf_internals_and_bit_ids[i].second.size();j++) {
            assert(possible_rules.size() >= 1);
            rules[i].push_back(possible_rules[0]);
        }
    }

    set<Bitvector> ret_set;

    vector<vector<int> > init_vals;
    for(int i = 0;i<leaf_internals_and_bit_ids.size();i++)
    {
        init_vals.emplace_back(vector<int>());
        BittreeNode* local_node = leaf_internals_and_bit_ids[i].first;
        for(int j = 0;j<local_node->children.size();j++)
        {
            local_node->children[j]->bit->is_bit_set = true;
            int bit_val = local_node->children[j]->bit->bit_val;
            init_vals[i].push_back(bit_val);
        }
    }

    do
    {
        for(int i = 0;i<leaf_internals_and_bit_ids.size();i++)
        {
            BittreeNode* local_node = leaf_internals_and_bit_ids[i].first;
            for(int j = 0;j<leaf_internals_and_bit_ids[i].second.size();j++) {
                int bit_id = leaf_internals_and_bit_ids[i].second[j];
                local_node->children[bit_id]->bit->is_bit_set = true;
                local_node->children[bit_id]->bit->bit_val = 0;
            }
        }

        int cost = 0;
        for(int i = 0;i<leaf_internals_and_bit_ids.size();i++)
        {

//            for(int j = 0;j<leaf_internals_and_bit_ids[i].second.size();j++) {
//                cost+=
//            }
            for(int j = 0;j<leaf_internals_and_bit_ids[i].second.size();j++) {
                int bit_id = leaf_internals_and_bit_ids[i].second[j];
                int next_bit_id;
                if(rules[i][j] == move_left) {
                    if (bit_id != 0) {
                        int left_bit_id = bit_id-1;
                        next_bit_id = left_bit_id;
                    }
                    else
                    {
                        int last_bit_id = leaf_internals_and_bit_ids[i].first->children.size()-1;
                        next_bit_id = last_bit_id;
                    }
                }
                else if(rules[i][j] == move_right) {
                    if (bit_id+1 < leaf_internals_and_bit_ids[i].first->children.size()) {
                        int left_bit_id = bit_id+1;
                        next_bit_id = left_bit_id;
                    }
                    else
                    {
                        int first_bit_id = 0;
                        next_bit_id = first_bit_id;
                    }
                }
                else if(rules[i][j] == move_to_last_copy) {
                    next_bit_id = -1;
                    TreeNode* copied_from = leaf_internals_and_bit_ids[i].first->children[bit_id]->bit->copied_from;
                    assert(copied_from->bit_in_bittree != NULL);
                    assert(copied_from->bit_in_bittree->copies.size() >= 2);
                    TreeNode* last_copy = copied_from->bit_in_bittree->copies[copied_from->bit_in_bittree->copies.size()-2];
                    assert(last_copy->bit_in_bittree != NULL);
                    last_copy->bit_in_bittree->is_bit_set = true;
                    last_copy->bit_in_bittree->bit_val = 1;
                    cout << "move_to_last_copy" << endl;
                    cout << "copied_from:"<<copied_from->to_string() << endl;
                    cout << "copied_from:"<<copied_from->bit_in_bittree->to_string() << endl;
                    cout << "last_copy:"<<last_copy->to_string() << endl;
                    cout << "last_copy:"<<last_copy->bit_in_bittree->to_string() << endl;
                } else{
                    assert(rules[i][j] == copy_right || rules[i][j] == copy_left || rules[i][j] == stay);
                    next_bit_id = -1;
                }
                if(next_bit_id != -1) {
                    leaf_internals_and_bit_ids[i].first->children[next_bit_id]->bit->is_bit_set = true;
                    leaf_internals_and_bit_ids[i].first->children[next_bit_id]->bit->bit_val = 1;
                }
            }
            for(int j = 0;j<leaf_internals_and_bit_ids[i].second.size();j++) {
                int bit_id = leaf_internals_and_bit_ids[i].second[j];
                int next_bit_id;
                if(rules[i][j] == copy_left) {
                    if (bit_id != 0) {
                        int left_bit_id = bit_id-1;
                        next_bit_id = left_bit_id;
                    }
                    else
                    {
                        int last_bit_id = leaf_internals_and_bit_ids[i].first->children.size()-1;
                        next_bit_id = last_bit_id;
                    }
                }
                else if(rules[i][j] == copy_right) {
                    if (bit_id+1 < leaf_internals_and_bit_ids[i].first->children.size()) {
                        int left_bit_id = bit_id+1;
                        next_bit_id = left_bit_id;
                    }
                    else
                    {
                        int first_bit_id = 0;
                        next_bit_id = first_bit_id;
                    }
                }
                else if(rules[i][j] == stay) {
                    next_bit_id = bit_id;
                }else{
                    assert(rules[i][j] == move_right || rules[i][j] == move_left ||  rules[i][j] == move_to_last_copy);
                    next_bit_id = -1;
                }
                if(next_bit_id != -1) {
                    leaf_internals_and_bit_ids[i].first->children[bit_id]->bit->is_bit_set = true;
                    leaf_internals_and_bit_ids[i].first->children[bit_id]->bit->bit_val = 1;
                    leaf_internals_and_bit_ids[i].first->children[next_bit_id]->bit->is_bit_set = true;
                    leaf_internals_and_bit_ids[i].first->children[next_bit_id]->bit->bit_val = 1;
                }
            }
        }

        cout << " HERE2: " << local_parent->to_string__one_line() << endl;

//        ret_set.insert(make_pair(cost, BittreeTaskTypeAsPartialFunction(this, subtask_depth).total_function));

        ret_set.insert(BittreeTaskTypeAsPartialFunction(this, subtask_depth).total_function);

        for(int i = 0;i<leaf_internals_and_bit_ids.size();i++)
        {
            BittreeNode* local_node = leaf_internals_and_bit_ids[i].first;
            for(int j = 0;j<local_node->children.size();j++)
            {
                assert(local_node->children[j]->bit->is_bit_set);
                local_node->children[j]->bit->bit_val = init_vals[i][j];
            }
        }

    }while(next_rule(rules, possible_rules));

    vector<Bitvector> ret;


    cout << "HOPE: " << endl;
    for(auto bitvector : ret_set)
    {
        ret.push_back(bitvector);
        cout << bitvector.to_string() << endl;
    }

    return ret;
}


BittreeNode* BittreeInputOutputType::add_input_child(NodeType child_type) {
    assert(input != NULL);
    BittreeNode* new_child = new BittreeNode(input, Name("children", input->children.size()), child_type);
    input->children.push_back(new_child);
    return new_child;
}

BittreeNode* BittreeInputOutputType::add_output_child(NodeType child_type) {
    assert(output != NULL);
    BittreeNode* new_child =
            new BittreeNode(
                    output, Name("children", input->children.size()), child_type);
    output->children.push_back(new_child);
    return new_child;
}

BittreeNode* BittreeInputOutputType::add_output_child(NodeType child_type, BitInBittreeType bit_in_bittree_type) {
    assert(output != NULL);
    BittreeNode* new_child =
            new BittreeNode(
                    output, Name("children", output->children.size()), child_type, bit_in_bittree_type);
    output->children.push_back(new_child);
    return new_child;
}

BittreeProgram* BittreeProgram::get_child_bittree_program(TreeNode* child)
{
    return new BittreeProgram(child, root, num_subtree_markers, max_mask_type_depth-1, num_subtasks);
}

void BittreeProgram::populate_bittree_programs()
{
    if(is_root)
    {
        bittree_programs.push_back(this);
    }

    bool enter = false;
    if (node->bittree_type_node != NULL) {
        BittreeNode *local_bittree = node->bittree_type_node;
        enter = true;
        if (local_bittree->node_type == internal_node ||
            (local_bittree->node_type == leaf_node && local_bittree->leaf_node_type == not_leaf_node)) {
            assert(local_bittree->delta == NULL);
            assert(local_bittree->bit == NULL);
            for (int i = 0; i < local_bittree->children.size(); i++) {
                bittree_programs.push_back(get_child_bittree_program(local_bittree->children[i]));
            }
        } else if (local_bittree->node_type == leaf_node) {
            if (local_bittree->leaf_node_type == bit_node) {
                bittree_programs.push_back(get_child_bittree_program(local_bittree->bit));
            } else if (local_bittree->leaf_node_type == delta_node) {
                bittree_programs.push_back(get_child_bittree_program(local_bittree->delta));
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
        assert(local_bittree->io != NULL);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->io));
//        assert(local_bittree->io->output != NULL);
//        bittree_programs.push_back(get_child_bittree_program(local_bittree->output));

        if (local_bittree->decomposition != NULL) {
            bittree_programs.push_back(get_child_bittree_program(local_bittree->decomposition));
        }
//        if (local_bittree->delta != NULL) {
//            bittree_programs.push_back(get_child_bittree_program(local_bittree->delta));
//        }
//        if (local_bittree->subtask != NULL) {
//            bittree_programs.push_back(get_child_bittree_program(local_bittree->subtask));
//        }
//        if (local_bittree->solution != NULL) {
//            bittree_programs.push_back(new BittreeProgram(local_bittree->solution, root, num_subtree_markers));
//        }
    }

    if(node->bittree_io_type != NULL)
    {
        assert(!enter);
        enter = true;
        BittreeInputOutputType *local_bittree = node->bittree_io_type;
        assert(local_bittree->output != NULL);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->output));
        assert(local_bittree->output != NULL);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->output));
    }

    if(node->bittree_task_decomposition != NULL)
    {
        assert(!enter);
        enter = true;
        BittreeTaskDecomposition *local_bittree = node->bittree_task_decomposition;
        assert (local_bittree->delta != NULL);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->delta));
        assert(local_bittree->subtask != NULL);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->subtask));
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


void BittreeProgram::recurse_on_bittree_programs(int num_subtasks)
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
            cout << bittree_programs[i]->node->root_to_node_path__to__string() << " :: " << endl;

            PartialFunction local_partial_function = root->to_partial_function(num_subtasks);
            assert(local_partial_function.full());
            bittree_masks_as_bitvectors.push_back(local_partial_function.total_function);
            cout << root->to_partial_function(num_subtasks).to_string() << " num_subtree_markers " << num_subtree_markers << " :" << endl;

            if(num_subtree_markers>=2)
            {
                memset_visited(vis_type, num_subtree_markers - 1);
                next_rec_programs.push_back(
                        new BittreeProgram(
                                bittree_programs[i]->node, root, num_subtree_markers - 1, max_mask_type_depth, num_subtasks));
//                cout << endl;
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
            if(num_subtree_markers>=2)
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

BittreeProgram::BittreeProgram(TreeNode *_node, BittreeTaskType *_root, int _num_subtree_markers, int _max_mask_type_depth, int _num_subtasks)
{
    num_subtasks = _num_subtasks;
    max_mask_type_depth = _max_mask_type_depth;
    num_subtree_markers = _num_subtree_markers;
    if(num_subtree_markers == 0)
    {
        assert(false);
        return;
    }
    if(max_mask_type_depth == 0)
    {
        return;
    }
    if(_node->visited(vis_type, num_subtree_markers))
    {
        return;
    }
    else
    {
        _node->visit(vis_type, num_subtree_markers);
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

    recurse_on_bittree_programs(num_subtasks);

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
//        cout << "----------------------------" << endl;
        for(auto item: uniques)
        {

            all_bittree_masks_as_bitvectors.push_back(item);
//            cout << item.to_string() << endl;
        }
//        cout << "all_partial_functions.size() " << all_bittree_masks_as_bitvectors.size() << endl;
    }
}


void BittreeTaskDecomposition::append_bits(vector<BitInBittree *>& bits)
{
    if(subtask!=NULL)
    {
        subtask->append_bits(bits);
    }
    if(delta!=NULL)
    {
        delta->append_bits(bits);
    }
}

void BittreeTaskDecomposition::append_bits_of_prefix_subtree(vector<BitInBittree *>& bits, int num_subtasks)
{
    assert(num_subtasks>=0);
    if(subtask!=NULL)
    {
        subtask->append_bits_of_prefix_subtree(bits, num_subtasks);
    }
    if(delta!=NULL)
    {
        delta->append_bits(bits);
    }
}


string BittreeTaskDecomposition::to_string(int num_tabs)
{
    string ret;
    int next_tabs = num_tabs;
    int next_next_tabs = next_tabs+1;
    string open_bracket = "\n"+tabs(next_tabs) + "{\n";
    string close_bracket = tabs(next_tabs) + "}\n";

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
}

string BittreeTaskDecomposition::bits_to_string(int num_tabs) {

    string ret;
    int next_tabs = num_tabs;
    int next_next_tabs = next_tabs+1;
    string open_bracket = "\n"+tabs(next_tabs) + "{\n";
    string close_bracket = tabs(next_tabs) + "}\n";

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

BittreeTaskDecomposition::BittreeTaskDecomposition(
        TreeNode *parent, Name name, BittreeInputOutputType *delta_to_copy, BittreeTaskType *_subtask): TreeNode(parent, name, this) {
    if(delta_to_copy != NULL)
    {
        delta = new BittreeInputOutputType(this, Name("delta"), delta_to_copy, true);
    }
    if(_subtask != NULL)
    {
        subtask = _subtask;
        subtask->parents.push_back(this);
        subtask->names.push_back(Name("subtask"));
    }
}

BittreeTaskDecomposition::BittreeTaskDecomposition(
        TreeNode *parent, Name name, BittreeTaskDecomposition *to_copy, bool all_new_bits): TreeNode(parent, name, this)
{
    if(to_copy != NULL)
    {
        copied_from = to_copy;
        to_copy->copies.push_back(this);
        delta = new BittreeInputOutputType(this, Name("delta"), to_copy->delta, all_new_bits);
        subtask = new BittreeTaskType(this, Name("subtask"), to_copy->subtask, all_new_bits);
    }
}

BittreeTaskDecomposition::BittreeTaskDecomposition(TreeNode *parent, Name name, BittreeTaskDecomposition *to_copy,
                                                   bool all_new_bits, bool copy_all) : TreeNode(parent, name, this){

    if(to_copy != NULL)
    {
        copied_from = to_copy;
        to_copy->copies.push_back(this);
        delta = new BittreeInputOutputType(this, Name("delta"), to_copy->delta, all_new_bits);
        subtask = new BittreeTaskType(this, Name("subtask"), to_copy->subtask, all_new_bits, copy_all);
    }
}
