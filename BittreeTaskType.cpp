//
// Created by Kliment Serafimov on 1/11/20.
//

#include "BittreeTaskType.h"
#include "MetaExample.h"

void BittreeTypeNode::init(TreeNode *_parent)
{
    parent = _parent;
    children = new vector<BittreeTypeNode*>();
}

BittreeTypeNode::BittreeTypeNode(TreeNode* _parent, NodeType _node_type)
{
//    assert(_node_type == internal_node);
    init(_parent);
    node_type = _node_type;
    leaf_node_type = not_leaf_node;
};

BittreeTypeNode::BittreeTypeNode(TreeNode* _parent, NodeType _node_type, BitInBittreeType bit_type)
{
    assert(_node_type == leaf_node);
    init(_parent);
    node_type = _node_type;
    if(node_type == leaf_node)
    {
        bit = new BitInBittree(this, bit_type);
    }
    leaf_node_type = bit_node;
};

BittreeTypeNode::BittreeTypeNode(TreeNode *_parent, NodeType _node_type, BittreeTypeLeafNodeType _leaf_node_type)
{
    assert(_node_type == leaf_node);
    assert(_leaf_node_type == double_node);
    init(_parent);
    leaf_node_type = _leaf_node_type;
}

void BittreeTypeNode::copy_leaf_node(BittreeTypeNode *to_copy, bool all_new_bits)
{
    if(to_copy->bit->is_bit_set && to_copy->bit->bit_type == shared_machine_bit)
    {
        bit = to_copy->bit;
    }
    else
    {
        if (to_copy->bit->bit_type == new_blanko_bit || to_copy->bit->bit_type == new_machine_bit)
        {
            bit = new BitInBittree(this, new_machine_bit);
        }
        else if (to_copy->bit->bit_type == shared_machine_bit)
        {
            if (!all_new_bits)
            {
                bit = to_copy->bit;
            }
            else
            {
                bit = new BitInBittree(this, shared_machine_bit);
            }
        }
        else
        {
            assert(to_copy->bit->bit_type == shared_blanko_bit);
            bit = new BitInBittree(this, shared_machine_bit);
        }
    }
    assert(to_copy->children->size() == 0);
}

BittreeTypeNode::BittreeTypeNode(TreeNode* _parent, BittreeTypeNode* to_copy, bool all_new_bits)
{
    init(_parent);
    node_type = to_copy->node_type;
    leaf_node_type = to_copy->leaf_node_type;
    if(node_type == leaf_node)
    {
        copy_leaf_node(to_copy, all_new_bits);
    }
    for(int i = 0;i<to_copy->children->size();i++)
    {
        children->push_back(new BittreeTypeNode(this, to_copy->children->at(i), all_new_bits));
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

void BittreeTypeNode::apply_delta(BittreeTypeNode *delta_bittree_type) {
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
        assert(new_value->node_type == leaf_node);
    }
    if(new_value->node_type == internal_node)
    {
        for (int i = 0; i < new_value->children->size(); i++) {
            children->push_back(new BittreeTypeNode(this, new_value->children->at(i), false));
        }
    }
    else
    {
        assert(new_value->node_type == leaf_node);
        if(new_value->leaf_node_type == double_node)
        {
            int init_children_size = children->size();
            for (int i = 0; i < init_children_size; i++)
            {
                children->push_back(new BittreeTypeNode(this, children->at(i), false));
            }
        }
        else
        {
            assert(false);
        }
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

//
//DeltaBittreeTaskType::DeltaBittreeTaskType(NodeType delta_input_type, NodeType delta_output_typee)
//{
//    delta_input = new DeltaBittreeType(delta_input_type);
//    delta_output = new DeltaBittreeType(delta_output_typee);
//}

BittreeTaskType::BittreeTaskType(NodeType input_node_type, NodeType output_node_type)
{
    input = new BittreeTypeNode(this, input_node_type);
    output = new BittreeTypeNode(this, output_node_type);
}

BittreeTaskType::BittreeTaskType(BittreeTaskType* to_copy, bool all_new_bits)
{
    input = new BittreeTypeNode(this, to_copy->input, all_new_bits);
    output = new BittreeTypeNode(this, to_copy->output, all_new_bits);
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

void BittreeTaskType::apply_delta(BittreeTaskType* type) {
    input->apply_delta(type->input);
    output->apply_delta(type->output);
}

BittreeTaskType BittreeTaskType::get_delta_application(BittreeTaskType* type)
{
    BittreeTaskType ret = BittreeTaskType(this, false);
    ret.apply_delta(type);
    return ret;
}

BittreeTaskType BittreeTaskType::get_supertask_type(BittreeTaskType* type)
{
    BittreeTaskType ret = BittreeTaskType(this, false);
    ret.apply_delta(type);
    ret.subtask = this;
    return ret;
}

void BittreeTaskType::solve(TaskName task_name) {

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
    solution = new BittreeTaskType(this, true);
    if(subtask != NULL)
    {
        solution->subtask = subtask->solution;
    }

    const int num_operands = 1;
    assert(solution->input->children->size() == num_operands);
    int operands[num_operands] = {0};
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

    int prev_set_bit = -1;
    pair<int, pair<int, int> > max_window = make_pair(-1, make_pair(-1, -1));
    for(int i = 0; i<solution->input->children->at(0)->children->size();i++)
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

    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__least_set_bit()
{
    solution = new BittreeTaskType(this, true);
    if(subtask != NULL)
    {
        solution->subtask = subtask->solution;
    }

    const int num_operands = 1;
    assert(solution->input->children->size() == num_operands);
    int operands[num_operands] = {0};
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

    assert(solution->output->children->at(0)->children->at(0)->bit->is_bit_set == false);
    solution->output->children->at(0)->children->at(0)->bit->is_bit_set = true;
    solution->output->children->at(0)->children->at(0)->bit->bit_val = edge_case_bit;

    for(int i = 0;i<solution->output->children->at(1)->children->size();i++)
    {
        assert(solution->output->children->at(1)->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(1)->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(1)->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__unary_sum()
{
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
    Bitvector sum = 0;
    sum.set(operands[0]+operands[1]);
    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__count_unary__reverse_subtask(int init_size)
{
    solution = new BittreeTaskType(this, true);
    if(subtask != NULL)
    {
        solution->subtask = subtask->solution;
    }

    const int num_operands = 1;
    assert(solution->input->children->size() == num_operands);
    int operands[num_operands] = {0};
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
    Bitvector sum = 0;
    sum.set_range(0, (operands[0] << (init_size - solution->input->children->at(0)->children->size())));
    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__one_shift_idx__reverse_subtask(int init_size)
{
    solution = new BittreeTaskType(this, true);
    if(subtask != NULL)
    {
        solution->subtask = subtask->solution;
    }

    const int num_operands = 1;
    assert(solution->input->children->size() == num_operands);
    int operands[num_operands] = {0};
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
    Bitvector sum = 0;
    sum.set(operands[0] << (init_size - solution->input->children->at(0)->children->size()));
    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__count_unary()
{
    solution = new BittreeTaskType(this, true);
    if(subtask != NULL)
    {
        solution->subtask = subtask->solution;
    }

    const int num_operands = 1;
    assert(solution->input->children->size() == num_operands);
    int operands[num_operands] = {0};
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
    Bitvector sum = 0;
    sum.set_range(0,operands[0]-1);
    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__one_shift_idx()
{
    solution = new BittreeTaskType(this, true);
    if(subtask != NULL)
    {
        solution->subtask = subtask->solution;
    }

    const int num_operands = 1;
    assert(solution->input->children->size() == num_operands);
    int operands[num_operands] = {0};
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
    Bitvector sum = 0;
    sum.set(operands[0]);
    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__multiply_by(int multiply_by)
{
    solution = new BittreeTaskType(this, true);
    if(subtask != NULL)
    {
        solution->subtask = subtask->solution;
    }

    const int num_operands = 1;
    assert(solution->input->children->size() == num_operands);
    int operands[num_operands] = {0};
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
    int sum = operands[0] * multiply_by;
    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__bitwise_binary_operator()
{
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
    int sum = operands[0] ^ operands[1];
    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void BittreeTaskType::solve__cumulative_binary_operator()
{
   solution = new BittreeTaskType(this, true);
   if(subtask != NULL)
   {
       solution->subtask = subtask->solution;
   }
   const int num_operands = 1;
   assert(solution->input->children->size() == num_operands);
   int operands[num_operands] = {1};
   for(int i = 0;i<num_operands;i++)
   {
       int pow = 1;
       for(int j = 0;j<solution->input->children->at(i)->children->size();j++)
       {
           assert(solution->input->children->at(i)->children->at(j)->bit->is_bit_set == true);
           operands[i]&=solution->input->children->at(i)->children->at(j)->bit->bit_val;
       }
   }
   int culumative_binary_operator = operands[0];
   for(int i = 0;i<solution->output->children->size();i++)
   {
       assert(solution->output->children->at(i)->bit->is_bit_set == false);
       solution->output->children->at(i)->bit->is_bit_set = true;
       solution->output->children->at(i)->bit->bit_val = get_bit(culumative_binary_operator, i);
   }
}


void BittreeTaskType::solve__greater()
{
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
    int is_0th_greater_than_1st = operands[0] > operands[1];
    for(int i = 0;i<solution->output->children->size();i++)
    {
        assert(solution->output->children->at(i)->bit->is_bit_set == false);
        solution->output->children->at(i)->bit->is_bit_set = true;
        solution->output->children->at(i)->bit->bit_val = get_bit(is_0th_greater_than_1st, i);
    }
}

void BittreeTaskType::solve__sum()
{
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
