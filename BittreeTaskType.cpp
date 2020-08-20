//
// Created by Kliment Serafimov on 1/11/20.
//

#include "BittreeTaskType.h"
#include "MetaExample.h"
#include "AutomatonRuleCost.h"
#include "CanvasAndBittreeProgram.h"
#include "PrimitiveRule.h"
#include "BittreeInputOutputType.h"
#include "MaskAndCostAndInstantiatedModules.h"

#include <iostream>
#include <set>
#include <utility>
#include <fstream>

using namespace std;

void BittreeNode::init()
{

}

BittreeNode::BittreeNode(TreeNode *parent, Name name, NodeType _node_type) : TreeNode(parent, std::move(name), this)
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
                         BittreeLeafNodeType _leaf_node_type) : TreeNode(parent, name, this)
{
    assert(_node_type == leaf_node);
    assert(_leaf_node_type == double_node);
    init();
    leaf_node_type = _leaf_node_type;
}

void BittreeNode::copy_leaf_node(BittreeNode *to_copy, bool all_new_bits)
{
    copy_leaf_node(to_copy, all_new_bits, false, false);
}

void BittreeNode::copy_leaf_node(BittreeNode *to_copy, bool all_new_bits, bool hard_pointer_assign_bits,
                                 bool hard_all_new_bits)
{
    if(to_copy->bit != nullptr) {
        if(hard_pointer_assign_bits)
        {
            bit = to_copy->bit;
            bit->push_back_parent(this);
            bit->push_back_name(Name("bit"));
        }
        else {
            if (to_copy->bit->is_bit_set && to_copy->bit->bit_type == shared_machine_bit) {
                if(!hard_all_new_bits)
                {
                    bit = to_copy->bit;
                    bit->push_back_parent(this);
                    bit->push_back_name(Name("bit"));
                }
                else
                {
                    bit = new BitInBittree(this, Name("bit"), shared_machine_bit, to_copy->bit);
                }
            } else {
                if (to_copy->bit->bit_type == new_blanko_bit || to_copy->bit->bit_type == new_machine_bit) {
                    bit = new BitInBittree(this, Name("bit"), new_machine_bit, to_copy->bit);
                } else if (to_copy->bit->bit_type == shared_machine_bit) {
                    if (!all_new_bits) {
                        bit = to_copy->bit;
                        bit->push_back_parent(this);
                        bit->push_back_name(Name("bit"));
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
    else if(to_copy->delta != nullptr)
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
    subtree_sum = to_copy->subtree_sum;
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
    subtree_sum = to_copy->subtree_sum;
    copied_from = to_copy;
    to_copy->copies.push_back(this);
    node_type = to_copy->node_type;
    leaf_node_type = to_copy->leaf_node_type;
    if(node_type == leaf_node)
    {
        copy_leaf_node(to_copy, all_new_bits, hard_pointer_assign_bits, false);
    }
    else if(node_type == internal_node)
    {
        for (int i = 0; i < to_copy->children.size(); i++)
        {
            children.push_back(
                    new BittreeNode(
                            this, Name("children", children.size()), to_copy->children.at(i), all_new_bits, hard_pointer_assign_bits));
        }
    }
    else
    {
        assert(false);
    }
};

BittreeNode::BittreeNode(TreeNode *parent, Name name, BittreeNode *to_copy, bool all_new_bits,
                         bool hard_pointer_assign_bits, bool hard_all_new_bits) : TreeNode(parent, name, this)
{
    init();
    subtree_sum = to_copy->subtree_sum;
    copied_from = to_copy;
    to_copy->copies.push_back(this);
    node_type = to_copy->node_type;
    leaf_node_type = to_copy->leaf_node_type;
    if(node_type == leaf_node)
    {
        copy_leaf_node(to_copy, all_new_bits, hard_pointer_assign_bits, hard_all_new_bits);
    }
    else if(node_type == internal_node)
    {
        for (int i = 0; i < to_copy->children.size(); i++)
        {
            children.push_back(
                    new BittreeNode(
                            this, Name("children", children.size()), to_copy->children.at(i), all_new_bits, hard_pointer_assign_bits, hard_all_new_bits));
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
        if(bit != nullptr)
        {
            return tabs(num_tabs)+"bit"+bit->TreeNode::to_string()+"{"+bit->to_string()+"}\n";
        }
        else if(delta != nullptr)
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
        if(bit != nullptr)
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
            assert(delta != nullptr);
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
        assert(delta==nullptr);
        assert(bit==nullptr);
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
        }
    }
    else
    {
        assert(false);
    }
}


BittreeNode *BittreeNode::set_delta(NodeType delta_type_node, int num_children, BitInBittreeType bit_type) {
    assert(delta == nullptr);
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


BittreeNode *BittreeNode::set_delta(BittreeNode *_delta) {
    assert(delta == nullptr);
    assert(node_type == leaf_node);
    leaf_node_type = delta_node;
    delta = _delta;
    return delta;
}

BittreeNode *BittreeNode::set_delta(NodeType delta_type_node, BittreeLeafNodeType delta_leaf_node_type) {
    assert(delta_leaf_node_type == double_node);
    assert(delta == nullptr);
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

string BittreeNode::to_string__one_line() const {
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

BittreeNode * BittreeNode::push_back_child(BittreeNode *child) {
    children.push_back(child);
    return child;
}


void BittreeNode::populate_leaf_internals_and_bit_ids(vector<BittreeNode*> path, vector<pair<BittreeNode *, vector<int> >> & ret) {
    if(node_type == leaf_node)
    {
        assert(leaf_node_type == bit_node);
        assert(false);
    }
    else if(node_type == internal_node)
    {
        path.push_back(this);
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
                children[i]->populate_leaf_internals_and_bit_ids(path,ret);
            }
        }
        if(ids.size()>=1) {
            ret.emplace_back(make_pair(this, ids));
        }
    } else{
        assert(false);
    }
}

string BittreeNode::slim_tree_to_string(int tab) const {
//    NodeType node_type;
//
//    //if internal_node
//    vector<BittreeNode*> children;
//
//    //if leaf_node;
//    BittreeLeafNodeType leaf_node_type;
//
//    //if leaf_node_type == bit_node;
//    BitInBittree *bit = nullptr;
//
//    //if leaf_node_type == double_node;
//
//    //if leaf_nod_type == delta_node;
//    BittreeNode* delta = nullptr;

    string ret;

    if(node_type == internal_node)
    {
        ret += tabs(tab) + "width = " + std::to_string(children.size()) + "\n";
        int count_bits = 0;
        for(int i = 0;i<children.size();i++)
        {
            if(children[i]->node_type == leaf_node && children[i]->leaf_node_type == bit_node)
            {
                count_bits++;
            }
        }
        assert(count_bits == 0 || count_bits == children.size());
        if(count_bits == children.size())
        {
            ret += tabs(tab);
        }
        for(int i = 0;i<children.size();i++)
        {
            ret += children[i]->slim_tree_to_string(tab+1);
        }
        if(ret[ret.size()-1]!='\n')
        {
            ret+="\n";
        }
    }
    else if (node_type == leaf_node)
    {
        if(leaf_node_type == bit_node)
        {
            ret += bit->slim_bit_to_string();
        }
        else if(leaf_node_type == double_node)
        {
            ret += tabs(tab) + "double_node";
        }
        else if(leaf_node_type == delta_node)
        {
            ret += tabs(tab) + "delta {" + "\n";
            ret += delta -> slim_tree_to_string(tab+1);
            ret += tabs(tab) + "}" + "\n";
        }
        else
        {
            assert(false);
        }
    }
    else
    {
        assert(false);
    }
    return ret;
}


pair<BittreeNode *, vector<int>* >
BittreeNode::apply_primitive_rule_to_bit(PrimitiveRule rule, BittreeNode *pointer_on_canvas, vector<int>* path) const
{
    int child_id = -1;
    int grandchild_id = -1;
    int greatgrandchild_id = -1;
    if(path->size() >= 1) {
        assert(pointer_on_canvas->get_parent() != nullptr);
        assert(path->at(path->size() - 1) == pointer_on_canvas->get_self_as_child_id());
        child_id = path->at(path->size()-1);
        if(path->size()>=2)
        {
            assert(pointer_on_canvas->get_parent()->get_parent() != nullptr);
            assert(path->at(path->size()-2) == pointer_on_canvas->get_parent()->get_self_as_child_id());
            grandchild_id = path->at(path->size()-2);
            if(path->size()>=3)
            {

                assert(pointer_on_canvas->get_parent()->get_parent()->get_parent() != nullptr);
                assert(path->at(path->size() - 3) == pointer_on_canvas->get_parent()->get_parent()->get_self_as_child_id());
                greatgrandchild_id = path->at(path->size()-3);
            }
        }
    }

    BittreeNode* ret_bit_on_canvas = nullptr;
    vector<int>* ret_context = &(*path);
    assert(pointer_on_canvas->leaf_node_type == bit_node);
    assert(bit->is_bit_set);
    if(bit->bit_val)
    {
        if(get_parent()!=nullptr)
        {
            assert(pointer_on_canvas->get_parent() != nullptr);
            if(rule == move_left || rule == move_right || rule == stay) {
                int next_id;
                if (rule == move_left) {
                    if (child_id == 0) {
                        assert(get_parent()->children.size() == pointer_on_canvas->get_parent()->children.size());
                        next_id = (int) get_parent()->children.size() - 1;
                    } else {
                        next_id = child_id - 1;
                    }
                } else if (rule == move_right) {
                    assert(get_parent()->children.size() == pointer_on_canvas->get_parent()->children.size());
                    if (child_id == (int) get_parent()->children.size() - 1) {
                        next_id = 0;
                    } else {
                        next_id = child_id + 1;
                    }
                } else if (rule == stay) {
                    next_id = child_id;
                } else {
                    assert(false);
                }

                pointer_on_canvas->bit->bit_val = max(0, pointer_on_canvas->bit->bit_val - 1);
                assert(get_parent()->children.size() == pointer_on_canvas->get_parent()->children.size());
                assert(0 <= next_id && next_id < get_parent()->children.size());
                BittreeNode *sibling = pointer_on_canvas->get_parent()->children[next_id];
                assert(sibling->node_type == leaf_node && sibling->leaf_node_type == bit_node && sibling->bit->is_bit_set);
                sibling->bit->bit_val++;
                ret_bit_on_canvas = sibling;
                ret_context->at(ret_context->size()-1) = next_id;
            }
            else if(rule == move_up || rule == move_down) {
                if (get_parent()->get_parent() != nullptr) {
                    assert(pointer_on_canvas->get_parent()->get_parent() != nullptr);
                    assert(grandchild_id != -1);
                    int next_id;
                    if (rule == move_down) {
                        if(grandchild_id == 0)
                        {
                            next_id = pointer_on_canvas->get_parent()->get_parent()->children.size() - 1;
                        }
                        else {
                            next_id = grandchild_id - 1;
                        }
                    } else if (rule == move_up) {
                        if(grandchild_id == pointer_on_canvas->get_parent()->get_parent()->children.size() - 1)
                        {
                            next_id = 0;
                        }
                        else {
//                            cout << "not here" << endl;
                            next_id = grandchild_id + 1;
                        }
                    } else {
                        assert(false);
                    }

                    assert(get_parent()->get_parent()->children.size() == pointer_on_canvas->get_parent()->get_parent()->children.size());
                    assert(0 <= next_id && next_id < get_parent()->get_parent()->children.size());

                    if(child_id < pointer_on_canvas->get_parent()->get_parent()->children[next_id]->children.size()) {
                        BittreeNode *cousin = pointer_on_canvas->get_parent()->get_parent()->children[next_id]->children[child_id];
                        if (cousin->node_type == leaf_node) {
                            if (cousin->leaf_node_type == bit_node) {
                                if (cousin->bit->is_bit_set) {
//                                    cout << "pointer_on_canvas->bit->bit_val " << pointer_on_canvas->bit->bit_val << endl;
                                    pointer_on_canvas->bit->bit_val = max(0, pointer_on_canvas->bit->bit_val - 1);
//                                    cout << "pointer_on_canvas->bit->bit_val " << pointer_on_canvas->bit->bit_val << endl;
//                                    cout << "cousin->bit->bit_val " << cousin->bit->bit_val << endl;
                                    cousin->bit->bit_val++;
//                                    cout << "cousin->bit->bit_val " << cousin->bit->bit_val << endl;
                                    ret_bit_on_canvas = cousin;
                                    ret_context->at(ret_context->size()-2) = next_id;
                                }
                                else
                                {
                                    assert(false);
                                }
                            } else {
                                assert(false);
                            }
                        }
                    }
                }
            }
            else if(rule == move_front || rule == move_back || rule == youngest_second_nephew) {
                if (get_parent()->get_parent() != nullptr) {
                    assert(pointer_on_canvas->get_parent()->get_parent() != nullptr);
                    assert(grandchild_id != -1);
                    if(get_parent()->get_parent()->get_parent() != nullptr) {
                        assert(pointer_on_canvas->get_parent()->get_parent()->get_parent() != nullptr);
                        assert(greatgrandchild_id != -1);
                        int next_id;
                        if (rule == move_back) {
                            if (greatgrandchild_id == 0) {
                                next_id = pointer_on_canvas->get_parent()->get_parent()->get_parent()->children.size() - 1;
                            } else {
                                next_id = greatgrandchild_id - 1;
                            }
                        } else if (rule == move_front || rule == youngest_second_nephew) {
                            if (greatgrandchild_id == pointer_on_canvas->get_parent()->get_parent()->get_parent()->children.size() - 1) {
                                next_id = 0;
                            } else {
                                next_id = greatgrandchild_id + 1;
                            }
                        } else {
                            assert(false);
                        }

                        assert(get_parent()->get_parent()->get_parent()->children.size() ==
                               pointer_on_canvas->get_parent()->get_parent()->get_parent()->children.size());
                        assert(0 <= next_id && next_id < get_parent()->get_parent()->get_parent()->children.size());

                        if(grandchild_id < pointer_on_canvas->get_parent()->get_parent()->get_parent()->children[next_id]->children.size()) {
                            if (child_id < pointer_on_canvas->get_parent()->get_parent()->get_parent()->children[next_id]->children[grandchild_id]->children.size()) {
                                BittreeNode *second_cousin = pointer_on_canvas->get_parent()->get_parent()->get_parent()->children[next_id]->children[grandchild_id]->children[child_id];
                                if (second_cousin->node_type == leaf_node && (rule == move_front || rule == move_back) ) {
                                    if (second_cousin->leaf_node_type == bit_node) {
                                        if (second_cousin->bit->is_bit_set) {
                                            pointer_on_canvas->bit->bit_val = max(0, pointer_on_canvas->bit->bit_val - 1);
                                            second_cousin->bit->bit_val++;
                                            ret_bit_on_canvas = second_cousin;
                                            ret_context->at(ret_context->size()-3) = next_id;
                                        }
                                        else
                                        {
                                            assert(false);
                                        }
                                    } else {
                                        assert(false);
                                    }
                                }
                                else if(second_cousin->node_type == internal_node && rule == youngest_second_nephew)
                                {
                                    if(second_cousin->children.size() >= 1)
                                    {
                                        if(second_cousin->children[0]->node_type == leaf_node)
                                        {
                                            assert(second_cousin->children[0]->leaf_node_type == bit_node);
                                            pointer_on_canvas->bit->bit_val = max(0, pointer_on_canvas->bit->bit_val - 1);
                                            second_cousin->children[0]->bit->bit_val++;

                                            ret_bit_on_canvas = second_cousin->children[0];
                                            ret_context->at(ret_context->size()-3) = next_id;
                                            ret_context->push_back(0);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if(rule == plus_one)
            {
                pointer_on_canvas->bit->bit_val++;
                ret_bit_on_canvas = pointer_on_canvas;
            }
            else if(rule == minus_one)
            {
                assert(pointer_on_canvas->bit->bit_val >= 1);
                pointer_on_canvas->bit->bit_val = max(0, pointer_on_canvas->bit->bit_val-1);
                ret_bit_on_canvas = pointer_on_canvas;
            }
            else
            {
                assert(false);
            }
        }
        else
        {
            assert(pointer_on_canvas->get_parent() == nullptr);
            assert(false);
        }
    }
    else
    {
        assert(false);
    }
    return make_pair(ret_bit_on_canvas, ret_context);
}
pair<BittreeNode *, BittreeNode*> BittreeNode::apply_rule(
        SequenceOfPrimitiveRules sequence_of_rules, BittreeNode *canvas, BittreeNode *pointer_on_canvas, vector<int> *path)
{
    BittreeNode* ret_canvas = nullptr;
    BittreeNode* ret_pointer_on_canvas = nullptr;

    if(canvas == nullptr)
    {
        cout << "passed a nullptr canvas" << endl;
        return make_pair(ret_canvas, ret_pointer_on_canvas);
    }

    int child_id = -1;
    int grandchild_id = -1;
    int greatgrandchild_id = -1;
    if(path->size() >= 1) {
        assert(pointer_on_canvas->get_parent() != nullptr);
        assert(path->at(path->size() - 1) == pointer_on_canvas->get_self_as_child_id());
        child_id = path->at(path->size()-1);
        if(path->size()>=2)
        {
            assert(pointer_on_canvas->get_parent()->get_parent() != nullptr);
            assert(path->at(path->size()-2) == pointer_on_canvas->get_parent()->get_self_as_child_id());
            grandchild_id = path->at(path->size()-2);
            if(path->size()>=3)
            {

                assert(pointer_on_canvas->get_parent()->get_parent()->get_parent() != nullptr);
                assert(path->at(path->size() - 3) == pointer_on_canvas->get_parent()->get_parent()->get_self_as_child_id());
                greatgrandchild_id = path->at(path->size()-3);
            }
        }
    }

    if(child_id != -1) {
        if (get_parent() != nullptr) {
            assert(pointer_on_canvas->get_parent()->children[child_id] == pointer_on_canvas);
        } else {
            assert(false);
        }
    }

    if(node_type == internal_node)
    {
        assert(pointer_on_canvas->node_type == internal_node);
        assert(pointer_on_canvas->children.size() == children.size());
        BittreeNode* at_canvas = canvas;
        BittreeNode* at_pointer = pointer_on_canvas;

        for(int i = 0;i<children.size();i++)
        {
            if(children[i]->subtree_sum >= 1) {
                path->push_back(i);
//                cout << "ENTER i = "<< i << endl;
                pair<BittreeNode*, BittreeNode*> new_canvas_and_child_of_pointer =
                        children[i]->apply_rule(sequence_of_rules, at_canvas, at_pointer->children[i], path);
                path->pop_back();
                at_canvas = new_canvas_and_child_of_pointer.first;
                if(at_canvas == nullptr)
                {
                    cout << "HERE if(at_canvas == nullptr)" << endl;
                    break;
                }
                BittreeNode* prev_at_pointer = at_pointer;
                at_pointer = new_canvas_and_child_of_pointer.second->get_parent();
                assert(at_pointer->get_self_as_child_id() == prev_at_pointer->get_self_as_child_id());
                assert(at_pointer->children[i]->get_self_as_child_id() == prev_at_pointer->children[i]->get_self_as_child_id());
//                cout << "EXIT i = "<< i << endl;
            }
            else
            {
//                for(int j =0;j<path->size();j++)
//                {
//                    cout << path->at(j) << " ";
//                }
//                cout << "children[i]->subtree_sum = "<<children[i]->subtree_sum << endl;
                assert(children[i]->subtree_sum>=0);
            }
        }
        ret_canvas = at_canvas;
        ret_pointer_on_canvas = at_pointer;
    }
    else if (node_type == leaf_node)
    {
        assert(pointer_on_canvas->node_type == leaf_node);
        if(leaf_node_type == bit_node)
        {
            vector<int> path_copy = *path;
            pair<BittreeNode*, vector<int>* > at =
                    make_pair(this, &path_copy);

//            vector<BitInBittree*> bits_canvas;
//            memset_visited(vis_bits);
//            canvas->append_bits(bits_canvas);
//            int count_canvas = 0;
//            for(int i =0;i<bits_canvas.size();i++)
//            {
//                count_canvas+=bits_canvas[i]->bit_val;
//            }
            BittreeNode *next_canvas = canvas;
            BittreeNode *next_pointer_on_canvas = pointer_on_canvas;
            bool print = sequence_of_rules.size() == 5;
            if(print) {
                cout << "new sequence: ";
                for(int i = 0;i<sequence_of_rules.size();i++)
                {
                    cout << rule_names[sequence_of_rules[i]] <<" ";
                }
                for(int i = 0;i<path->size();i++){
                    cout << path->at(i) <<" ";
                }
                cout << endl;
            }
            for(int i = 0;i<sequence_of_rules.size();i++)
            {
                if(print)
                {
                    cout << next_canvas->to_string__one_line() << endl;
                }
                at = at.first->apply_primitive_rule_to_bit(
                        sequence_of_rules[i], next_pointer_on_canvas, at.second);

                if(at.first == nullptr)
                {
//                    if(print)
                    {
                        cout << "break" << endl;
                    }
                    break;
                }
//                if(i+1<sequence_of_rules.size())
                {
                    next_canvas = new BittreeNode(
                            nullptr, Name("next_canvas"), next_canvas, true, false, true);
                    next_pointer_on_canvas = next_canvas->get_node(at.second);
                }
            }
            if(print) {
                cout << next_canvas->to_string__one_line() << endl;
                cout << "end sequence" << endl;
            }

//            vector<BitInBittree *> bits_next_canvas;

//            memset_visited(vis_bits);
//            next_canvas->append_bits(bits_next_canvas);

//            int count_next_canvas = 0;
//            for (int i = 0; i < bits_next_canvas.size(); i++) {
//                count_next_canvas += bits_next_canvas[i]->bit_val;
//            }
//            assert(count_canvas == count_next_canvas);

            if(at.first == nullptr)
            {
                ret_canvas = nullptr;
                ret_pointer_on_canvas = nullptr;
            }
            else
            {
                cout << "returns an actual thing" << endl;
                assert(next_canvas != nullptr);
                ret_canvas = next_canvas;
                ret_pointer_on_canvas = next_canvas->get_node(path);
            }
        }
        else if(leaf_node_type == double_node)
        {
            assert(false);
        }
        else if(leaf_node_type == delta_node)
        {
            assert(false);
        }
        else
        {
            assert(false);
        }
    }
    else
    {
        assert(false);
    }
    return make_pair(ret_canvas, ret_pointer_on_canvas);
}

void BittreeNode::initialize_special_parents(BittreeNode *parent) {

    if(node_type == internal_node)
    {
        for(int i = 0;i<children.size();i++)
        {

        }
    }
    else if (node_type == leaf_node)
    {
        if(leaf_node_type == bit_node)
        {
        }
        else if(leaf_node_type == double_node)
        {
        }
        else if(leaf_node_type == delta_node)
        {
        }
        else
        {
            assert(false);
        }
    }
    else
    {
        assert(false);
    }
}

void BittreeNode::apply_all_rules_to_subtree(
        vector<SequenceOfPrimitiveRules> *rules, CanvasAndBittreeProgram *canvas, vector<int> *path,
        AutomatonRuleCost max_cost, BehaviorToProgram *all_programs) const
{
    if (node_type == leaf_node) {
        assert(leaf_node_type == bit_node);
    }
    for (int i = 0; i < rules->size(); i++) {
        CanvasAndBittreeProgram* new_program = canvas->produce(rules->at(i), path);
        cout << "path: " << endl;
        for(int j = 0;j<path->size();j++)
        {
            cout << path->at(j) << " ";
        }
        cout << endl;
        if(new_program->get_canvas() == nullptr)
        {
            cout << "nullptr" << endl;
        }
        else
        {
            cout << "here subtree_sum = 1" << endl;
            cout << new_program->to_string() << endl;
        }
        if(new_program->get_canvas() != nullptr
           && (false || new_program->get_cost() <= max_cost)) {
            cout << "all_programs->my_push_back(new_program);" << endl;
            cout << new_program->to_string() << endl;
            cout << endl;
            all_programs->my_push_back(new_program);
        }
    }
}

void BittreeNode:: populate_programs(
        vector<SequenceOfPrimitiveRules> *rules,
        CanvasAndBittreeProgram *canvas,
        int next_child,
        vector<int> *path,
        BehaviorToProgram *all_programs,
        AutomatonRuleCost max_cost) const {

    //moves: yield program where the same operator is applied to all the bits in the subtree.
    //combine different programs generated by children;

    assert(max_cost.get_defined());

    assert(subtree_sum != -1);
    if(subtree_sum == 0)
    {
        if(next_child == -1)
        {
            assert(canvas->get_cost() <= max_cost);
            all_programs->my_push_back(canvas);
        }
        else
        {
            if(next_child == children.size())
            {
                assert(canvas->get_cost() <= max_cost);
                all_programs->my_push_back(canvas);
            }
            else
            {
                populate_programs(rules, canvas, next_child + 1, path, all_programs, max_cost);
            }
        }
    }
    else if(subtree_sum == 1)
    {
        apply_all_rules_to_subtree(rules, canvas, path, max_cost, all_programs);
    }
    else {
        if (next_child == -1 || node_type == leaf_node) {
            apply_all_rules_to_subtree(rules, canvas, path, max_cost, all_programs);
        } else {
            if (node_type == internal_node) {
                assert(next_child >= 0);
                if (next_child == children.size()){
                    assert(canvas->get_cost() <= max_cost);
                    all_programs->my_push_back(canvas);
//                    cout << "HERE CanvasAndBittreeProgram" << endl;
//                    cout << canvas->to_string() << endl;
                }
                else {
                    assert(next_child < children.size());
                    if(children[next_child]->subtree_sum >= 1) {
                        path->push_back(next_child);
                        BehaviorToProgram programs_with_next_primitive = BehaviorToProgram();
                        children[next_child]->populate_programs(rules, canvas, -1, path, &programs_with_next_primitive,
                                                                max_cost);
                        if (children[next_child]->node_type != leaf_node && children[next_child]->subtree_sum >= 2) {
                            children[next_child]->populate_programs(rules, canvas, 0, path,
                                                                    &programs_with_next_primitive, max_cost);
                        }
                        path->pop_back();
                        bool memoization = true;
                        if (memoization) {
                            for (BehaviorToProgramIterator at_behavior_bucket = programs_with_next_primitive.init_iter__iterators();
                                 !programs_with_next_primitive.end_iter__iterators();
                                 at_behavior_bucket = programs_with_next_primitive.get_next__iterators()) {
                                CanvasAndBittreeProgram *at_canvas = (*at_behavior_bucket->second.begin()).get_pointer();
                                if (at_canvas != nullptr) {
                                    if (true || at_canvas->get_cost() <= max_cost) {
                                        all_programs->my_push_back(at_canvas);
                                        populate_programs(
                                                rules, at_canvas,
                                                next_child + 1, path, all_programs, max_cost);
                                    }
                                }
                            }
                        } else {
                            for (CanvasAndBittreeProgram *at_canvas = programs_with_next_primitive.init_iter__all();
                                 !programs_with_next_primitive.end_iter__all();
                                 at_canvas = programs_with_next_primitive.get_next__all()) {
                                if (at_canvas != nullptr) {
                                    if (false || at_canvas->get_cost() <= max_cost) {
                                        all_programs->my_push_back(at_canvas);
                                        populate_programs(
                                                rules, at_canvas,
                                                next_child + 1, path, all_programs, max_cost);
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if(next_child == children.size())
                        {
                            assert(canvas->get_cost() <= max_cost);
                            all_programs->my_push_back(canvas);
//                            cout << "HERE CanvasAndBittreeProgram" << endl;
//                            cout << canvas->to_string() << endl;
                        }
                        else
                        {
                            populate_programs(rules, canvas, next_child + 1, path, all_programs, max_cost);
                        }
                    }
                }
            } else {
                assert(false);
            }
        }
    }
}

BittreeNode *BittreeNode::get_node(const vector<int> * path, int depth) {
    assert(depth <= path->size());
    if(depth == path->size())
    {
        return this;
    }
    else
    {
        assert(children.size() > path->at(depth));
        return children[path->at(depth)]->get_node(path, depth+1);
    }
}

BittreeNode *BittreeNode::get_node(const vector<int> * path)  {
    if(path->size() == 0)
    {
        return this;
    }
    return get_node(path, 0);
}

int BittreeNode::calc_subtree_sums() {
    subtree_sum = 0;
    if(node_type == internal_node)
    {
        for(int i = 0;i<children.size();i++)
        {
            subtree_sum += children[i]->calc_subtree_sums();
        }
    }
    else if(node_type == leaf_node)
    {
        assert(leaf_node_type == bit_node);
        assert(bit->is_bit_set);
        subtree_sum = bit->bit_val;
    }
    else
    {
        assert(false);
    }
    return subtree_sum;
}

//BittreeInputOutputType::BittreeInputOutputType(): TreeNode(nullptr, Name("nullptr"), this)
//{
//}

BittreeTaskType::BittreeTaskType(): TreeNode(nullptr, Name("nullptr"), this)
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
    assert(to_copy->io != nullptr);
    io = new BittreeInputOutputType(this, Name("io"), to_copy->io, all_new_bits);
//    if(name.to_string() == "solution")
//    {
//        if(to_copy->solution != nullptr)
//        {
//            if (to_copy->solution->decomposition != nullptr)
//            {
//                decomposition = new BittreeTaskDecomposition(
//                        this, Name("decomposition"), to_copy->solution->decomposition, all_new_bits);
//            }
//        }
//    }
//    else
//    {
//        if (to_copy->decomposition != nullptr) {
//            decomposition = new BittreeTaskDecomposition(
//                    this, Name("decomposition"), to_copy->decomposition, all_new_bits);
//        }
//        if(to_copy->solution != nullptr)
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
    assert(to_copy->io != nullptr);
    io = new BittreeInputOutputType(this, Name("io"), to_copy->io, all_new_bits);
    if (decomposition != nullptr) {
        decomposition =
                new BittreeTaskDecomposition(this, Name("decomposition"), to_copy->decomposition, all_new_bits, copy_all);
    }
    if(solution != nullptr)
    {
        solution =
                new BittreeTaskType(this, Name("decomposition"), to_copy->solution, all_new_bits, copy_all);
    }
}


BittreeTaskType::BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits,
                                 bool copy_all, bool hard_all_new_bits) {
    copied_from = to_copy;
    to_copy->copies.push_back(this);
    assert(to_copy->io != nullptr);
    io = new BittreeInputOutputType(this, Name("io"), to_copy->io, all_new_bits);
    if (decomposition != nullptr) {
        decomposition =
                new BittreeTaskDecomposition(this, Name("decomposition"), to_copy->decomposition, all_new_bits, copy_all, hard_all_new_bits);
    }
    if(solution != nullptr)
    {
        solution =
                new BittreeTaskType(this, Name("decomposition"), to_copy->solution, all_new_bits, copy_all ,hard_all_new_bits);
    }
}


void BittreeTaskType::append_bits(vector<BitInBittree*>& bits)
{
    append_IO_bits(bits);
    if(solution!=nullptr)
    {
        solution->append_bits(bits);
    }
    if(decomposition!=nullptr)
    {
        decomposition->append_bits(bits);
    }
}

void BittreeTaskType::append_bits_of_prefix_subtree(vector<BitInBittree*>& bits, int num_subtasks)
{
    append_IO_bits(bits);
    if(solution!=nullptr)
    {
        solution->append_bits(bits);
    }
    if(decomposition!=nullptr && num_subtasks-1 >= 0)
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

    if(solution != nullptr)
    {
        ret += tabs(next_tabs)+"solution"+solution->TreeNode::to_string()+open_bracket;
        ret += solution->to_string(next_next_tabs);
        ret += close_bracket;
    }
    if(decomposition != nullptr)
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

    if(solution != nullptr)
    {
        ret += tabs(next_tabs)+"solution"+open_bracket;
        ret += solution->bits_to_string(next_next_tabs);
        ret += close_bracket;
    }

    if(decomposition != nullptr)
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
    assert(has_unique_name());
    BittreeTaskType* ret =
            new BittreeTaskType(
                    nullptr, Name("supertask_of_"+get_last_name().to_string()), this, false);
    ret->decomposition = new BittreeTaskDecomposition(ret, Name("decomposition"), delta, this);
    ret->apply_delta(ret->decomposition->delta);
    return ret;
}

void BittreeTaskType::solve(Task *task_name) {

    solution = new BittreeTaskType(this, Name("solution"), this, true);

    if(decomposition != nullptr)
    {
        if(decomposition->subtask->solution != nullptr)
        {
            solution->decomposition =
                    new BittreeTaskDecomposition(
                            solution, Name("decomposition"), nullptr, decomposition->subtask->solution);
        }

    }
    
    solution->io->solve(task_name);
}

BittreeTaskTypeAsPartialFunction BittreeTaskType::to_partial_function(int num_subtasks) {
    vector<BitInBittree*> partial_bits;
    memset_visited(vis_bits);
    append_bits_of_prefix_subtree(partial_bits, num_subtasks);
    return PartialFunction(partial_bits);
}

MetaExample BittreeTaskType::to_meta_example(int id, int num_subtasks) {
    if(solution != nullptr) {
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
        if(at_subtask == nullptr)
        {
            at_subtask = decomposition->subtask;
        }
        while(subtask_depth>0)
        {
            ret += " ";
            ret += at_subtask->io->output->to_string__one_line();

            if(at_subtask->decomposition != nullptr)
            {
                assert(at_subtask->decomposition->subtask != nullptr);
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


bool next_rule(vector<vector<PrimitiveRule> > & rules, vector<PrimitiveRule> possible_rules)
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

BittreeNode* copy_bittree(BittreeNode* parent, Name name, BittreeNode* to_copy)
{
    return new BittreeNode(parent, name, to_copy, true);
}



vector<MaskAndCostAndInstantiatedModules*>
BittreeTaskType::generate_variety(int subtask_depth, ofstream *fout, AutomatonRuleCost max_automaton_rule_cost, BehaviorToProgram* all_behaviors)
{
    BittreeNode* local_parent = new BittreeNode(nullptr, Name("local_parent"), internal_node);
    int child_id = 0;
    local_parent->push_back_child(copy_bittree(local_parent, Name("copy_input", child_id++), io->input));
    local_parent->push_back_child(copy_bittree(local_parent, Name("copy_output", child_id++), io->output));

    int init_subtask_depth = subtask_depth;
    if(subtask_depth > 0) {
        BittreeTaskType* at_subtask = decomposition->subtask->solution;
        if(at_subtask == nullptr)
        {
            at_subtask = decomposition->subtask;
        }
        while(subtask_depth>0)
        {
            local_parent->push_back_child(
                    copy_bittree(local_parent, Name("copy_subtask_output", child_id++), at_subtask->io->output));
            if(at_subtask->decomposition != nullptr)
            {
                assert(at_subtask->decomposition->subtask != nullptr);
                at_subtask = at_subtask->decomposition->subtask;
            } else
            {
                break;
            }
            subtask_depth-=1;
        }
    }

    local_parent->initialize_special_parents(nullptr);

    subtask_depth = init_subtask_depth;

    string init_str = local_parent->to_string__one_line();
    cout << "HERE: " << init_str << endl;

    vector<pair<BittreeNode*, vector<int> > > leaf_internals_and_bit_ids;

    vector<BittreeNode*> path;
    (*fout) << "LOCAL_PARENT: " << endl;
    (*fout) << local_parent->slim_tree_to_string(0) << endl << endl;

    local_parent->calc_subtree_sums();
    BittreeNode * canvas = copy_bittree(nullptr, Name("canvas"), local_parent);

    //BOZHE GOSPODE POMOGNI MI.

    vector<SequenceOfPrimitiveRules> initial_rules;
    initial_rules.emplace_back(stay);
    initial_rules.emplace_back(move_right);
//    initial_rules.emplace_back(move_left);
    initial_rules.emplace_back(move_up);
//    initial_rules.emplace_back(move_down);
    initial_rules.emplace_back(move_front);
//    initial_rules.emplace_back(move_back);
    initial_rules.emplace_back(youngest_second_nephew);
    initial_rules.emplace_back(move_front, move_up);
    initial_rules.emplace_back(move_front, move_right);
    initial_rules.emplace_back(move_up, move_right);
    initial_rules.emplace_back(move_front, move_up, move_right);
    initial_rules.emplace_back(move_front, move_up, move_up);
    initial_rules.emplace_back(move_up, move_up, move_right);
    initial_rules.emplace_back(move_up, move_right, move_right);

    initial_rules.emplace_back(youngest_second_nephew, move_right);
    initial_rules.emplace_back(youngest_second_nephew, move_up);
    initial_rules.emplace_back(youngest_second_nephew, move_front);
    initial_rules.emplace_back(youngest_second_nephew, move_front, move_up);
    initial_rules.emplace_back(youngest_second_nephew, move_front, move_right);
    initial_rules.emplace_back(youngest_second_nephew, move_up, move_right);
    initial_rules.emplace_back(youngest_second_nephew, move_front, move_up, move_right);
    initial_rules.emplace_back(youngest_second_nephew, move_front, move_up, move_up);
    initial_rules.emplace_back(youngest_second_nephew, move_up, move_up, move_right);
    initial_rules.emplace_back(youngest_second_nephew, move_up, move_right, move_right);

    vector<SequenceOfPrimitiveRules> all_rules;
    all_rules.push_back(minus_one);
    for(int i = 0;i<initial_rules.size();i++)
    {
        all_rules.emplace_back(initial_rules[i]);
//        all_rules.emplace_back(SequenceOfPrimitiveRules(plus_one, initial_rules[i]));
//        for(int j = 0;j<initial_rules.size();j++)
//        {
//            all_rules.emplace_back(SequenceOfPrimitiveRules(initial_rules[i], SequenceOfPrimitiveRules(plus_one, initial_rules[j])));
//            all_rules.emplace_back(SequenceOfPrimitiveRules(plus_one, all_rules[all_rules.size() - 1]));
//        }
    }

//    all_rules.emplace_back(SequenceOfPrimitiveRules(move_front, plus_one), SequenceOfPrimitiveRules(move_up, plus_one, move_up));
//    all_rules.emplace_back(SequenceOfPrimitiveRules(move_front, plus_one), SequenceOfPrimitiveRules(move_up, plus_one, move_right));
//    all_rules.emplace_back(SequenceOfPrimitiveRules(move_up, plus_one), SequenceOfPrimitiveRules(move_up, plus_one, move_up));
//    all_rules.emplace_back(SequenceOfPrimitiveRules(move_up, plus_one), SequenceOfPrimitiveRules(move_up, plus_one, move_right));
//    all_rules.emplace_back(SequenceOfPrimitiveRules(move_up, plus_one), SequenceOfPrimitiveRules(move_right, plus_one, move_right));
//    all_rules.emplace_back(SequenceOfPrimitiveRules(move_right, plus_one), SequenceOfPrimitiveRules(move_right, plus_one, move_right));

    vector<int> new_path;
    BehaviorToProgram local_behaviors = BehaviorToProgram();
    local_parent->populate_programs(
            &all_rules, new CanvasAndBittreeProgram(canvas), -1, &new_path, &local_behaviors, max_automaton_rule_cost);
    assert(new_path.size() == 0);
    if(local_parent->node_type != leaf_node) {
        local_parent->populate_programs(
                &all_rules, new CanvasAndBittreeProgram(canvas), 0, &new_path, &local_behaviors,
                max_automaton_rule_cost);
    }
    (*fout) << local_parent->to_string__one_line() << " :: " << endl;
    for (
            CanvasAndBittreeProgram* at_behavior = local_behaviors.init_iter__all();
            !local_behaviors.end_iter__all(); at_behavior = local_behaviors.get_next__all())
    {
        (*fout) << "\t" << at_behavior->canvas->to_string__one_line() << " | " << at_behavior->get_cost().get_cost() << "\t| " << at_behavior->AutomatonRule::to_string() << endl;
    }

    vector<MaskAndCostAndInstantiatedModules*> ret;
//    for (CanvasAndBittreeProgram* at_behavior = local_behaviors.init_iter__all(); !local_behaviors.end_iter__all(); at_behavior = local_behaviors.get_next__all()) {
//        ret.my_push_back(new MaskAndCostAndInstantiatedModules(at_behavior));
//    }
    for (BehaviorToProgramIterator at_behavior = local_behaviors.init_iter__iterators(); !local_behaviors.end_iter__iterators(); at_behavior = local_behaviors.get_next__iterators()) {
        assert(at_behavior->second.size() >= 1);
        ret.push_back(new MaskAndCostAndInstantiatedModules((*(*at_behavior).second.begin()).get_pointer()));
        int prev = -1;
        for(multiset<Pointer<CanvasAndBittreeProgram> >::iterator it = at_behavior->second.begin(); it != at_behavior->second.end();it++)
        {
            assert(prev <= (*it).get_pointer()->get_cost().get_cost());
            prev = (*it).get_pointer()->get_cost().get_cost();
//            cout << (*it).get_pointer()->get_cost().to_string() << endxl;
        }
    }

    all_behaviors->plus_equals(&local_behaviors);

    return ret;
}

string BittreeTaskType::to_string__one_line(int subtask_depth)
{
    string ret = io->input->to_string__one_line();
    ret += io->output->to_string__one_line();
    int init_subtask_depth = subtask_depth;
    if(subtask_depth > 0) {
        BittreeTaskType* at_subtask = decomposition->subtask->solution;
        if(at_subtask == nullptr)
        {
            at_subtask = decomposition->subtask;
        }
        while(subtask_depth>0)
        {
            ret += " ";
            ret += at_subtask->io->output->to_string__one_line();

            if(at_subtask->decomposition != nullptr)
            {
                assert(at_subtask->decomposition->subtask != nullptr);
                at_subtask = at_subtask->decomposition->subtask;
            }
            subtask_depth-=1;
        }
    }

    if(solution != nullptr) {
        subtask_depth = init_subtask_depth;
        ret += "  -->  ";
        ret += solution->to_string__one_line(subtask_depth);
    }

//        ret += " -> ";
//        ret += solution->io->input->to_string__one_line();
//        ret += solution->io->output->to_string__one_line();

    return ret;
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
    if (node->bittree_node != nullptr) {
        BittreeNode *local_bittree = node->bittree_node;
        enter = true;
        if (local_bittree->node_type == internal_node ||
            (local_bittree->node_type == leaf_node && local_bittree->leaf_node_type == not_leaf_node)) {
            assert(local_bittree->delta == nullptr);
            assert(local_bittree->bit == nullptr);
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
    if (node->bittree_task_type != nullptr) {
        assert(!enter);
        enter = true;
        BittreeTaskType *local_bittree = node->bittree_task_type;
        assert(local_bittree->io != nullptr);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->io));
//        assert(local_bittree->io->output != nullptr);
//        bittree_programs.my_push_back(get_child_bittree_program(local_bittree->output));

        if (local_bittree->decomposition != nullptr) {
            bittree_programs.push_back(get_child_bittree_program(local_bittree->decomposition));
        }
//        if (local_bittree->delta != nullptr) {
//            bittree_programs.my_push_back(get_child_bittree_program(local_bittree->delta));
//        }
//        if (local_bittree->subtask != nullptr) {
//            bittree_programs.my_push_back(get_child_bittree_program(local_bittree->subtask));
//        }
//        if (local_bittree->solution != nullptr) {
//            bittree_programs.my_push_back(new BittreeProgram(local_bittree->solution, root, num_subtree_markers));
//        }
    }

    if(node->bittree_io_type != nullptr)
    {
        assert(!enter);
        enter = true;
        BittreeInputOutputType *local_bittree = node->bittree_io_type;
        assert(local_bittree->output != nullptr);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->output));
        assert(local_bittree->output != nullptr);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->output));
    }

    if(node->bittree_task_decomposition != nullptr)
    {
        assert(!enter);
        enter = true;
        BittreeTaskDecomposition *local_bittree = node->bittree_task_decomposition;
        assert (local_bittree->delta != nullptr);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->delta));
        assert(local_bittree->subtask != nullptr);
        bittree_programs.push_back(get_child_bittree_program(local_bittree->subtask));
    }

    if (node->bit_in_bittree != nullptr) {
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
        if (bittree_programs[i]->node != nullptr) {
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
                next_rec_programs.push_back(nullptr);
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
                next_rec_programs.push_back(nullptr);
            }
            else
            {
                next_rec_programs.push_back(nullptr);
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
        if(bittree_programs[i]!=nullptr && bittree_programs[i] != this)
        {
            bittree_programs[i]->extract_partial_functions(ret);
        }
        if(next_rec_programs[i]!=nullptr)
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

    is_root = _root == nullptr;

    if(is_root)
    {
        assert(node->bittree_task_type != nullptr);
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
    if(subtask!=nullptr)
    {
        subtask->append_bits(bits);
    }
    if(delta!=nullptr)
    {
        delta->append_bits(bits);
    }
}

void BittreeTaskDecomposition::append_bits_of_prefix_subtree(vector<BitInBittree *>& bits, int num_subtasks)
{
    assert(num_subtasks>=0);
    if(subtask!=nullptr)
    {
        subtask->append_bits_of_prefix_subtree(bits, num_subtasks);
    }
    if(delta!=nullptr)
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

    if(subtask != nullptr)
    {
        ret += tabs(next_tabs)+"subtask"+subtask->TreeNode::to_string()+open_bracket;
        ret += subtask->to_string(next_next_tabs);
        ret += close_bracket;
    }

    if(delta != nullptr)
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

    if(subtask != nullptr)
    {
        ret += tabs(next_tabs)+"subtask"+open_bracket;
        ret += subtask->bits_to_string(next_next_tabs);
        ret += close_bracket;
    }

    if(delta != nullptr)
    {
        ret += tabs(next_tabs)+"delta"+open_bracket;
        ret += delta->bits_to_string(next_next_tabs);
        ret += close_bracket;
    }
    return ret;
}

BittreeTaskDecomposition::BittreeTaskDecomposition(
        TreeNode *parent, Name name, BittreeInputOutputType *delta_to_copy, BittreeTaskType *_subtask): TreeNode(parent, name, this) {
    if(delta_to_copy != nullptr)
    {
        delta = new BittreeInputOutputType(this, Name("delta"), delta_to_copy, true);
    }
    if(_subtask != nullptr)
    {
        subtask = _subtask;
        subtask->push_back_parent(this);
        subtask->push_back_name(Name("subtask"));
    }
}

BittreeTaskDecomposition::BittreeTaskDecomposition(
        TreeNode *parent, Name name, BittreeTaskDecomposition *to_copy, bool all_new_bits): TreeNode(parent, name, this)
{
    if(to_copy != nullptr)
    {
        copied_from = to_copy;
        to_copy->copies.push_back(this);
        delta = new BittreeInputOutputType(this, Name("delta"), to_copy->delta, all_new_bits);
        subtask = new BittreeTaskType(this, Name("subtask"), to_copy->subtask, all_new_bits);
    }
}

BittreeTaskDecomposition::BittreeTaskDecomposition(TreeNode *parent, Name name, BittreeTaskDecomposition *to_copy,
                                                   bool all_new_bits, bool copy_all) : TreeNode(parent, name, this){

    if(to_copy != nullptr)
    {
        copied_from = to_copy;
        to_copy->copies.push_back(this);
        delta = new BittreeInputOutputType(this, Name("delta"), to_copy->delta, all_new_bits);
        subtask = new BittreeTaskType(this, Name("subtask"), to_copy->subtask, all_new_bits, copy_all);
    }
}

BittreeTaskDecomposition::BittreeTaskDecomposition(TreeNode *parent, Name name, BittreeTaskDecomposition *to_copy,
                                                   bool all_new_bits, bool copy_all, bool hard_all_new_bits) {
    if(to_copy != nullptr)
    {
        copied_from = to_copy;
        to_copy->copies.push_back(this);
        delta = new BittreeInputOutputType(this, Name("delta"), to_copy->delta, all_new_bits);
        subtask = new BittreeTaskType(this, Name("subtask"), to_copy->subtask, all_new_bits, copy_all, hard_all_new_bits);
    }
}

void AutomatonRule::push_back(PathAndSequenceOfRules new_path_and_rule) {
    cost.add_cost(new_path_and_rule.get_cost());
    code.push_back(new_path_and_rule);
}

CanvasAndBittreeProgram* CanvasAndBittreeProgram::produce(SequenceOfPrimitiveRules rule, vector<int> * path) {
    return new CanvasAndBittreeProgram(this, rule, path);
}

CanvasAndBittreeProgram::CanvasAndBittreeProgram(CanvasAndBittreeProgram *to_copy, SequenceOfPrimitiveRules rule, vector<int> *path) :
    AutomatonRule(to_copy)
{
    vector<int> copied_path;
    vector<int> second_copied_path;
    for(int i = 0;i<path->size();i++)
    {
        copied_path.push_back(path->at(i));
        second_copied_path.push_back(path->at(i));
    }

    canvas = new BittreeNode(
            nullptr, Name("next_canvas"), to_copy->canvas, true, false, true);
    BittreeNode *next_pointer_on_canvas = canvas->get_node(path);

    pair<BittreeNode *, BittreeNode*> ret = to_copy->canvas->get_node(path)->apply_rule(
            rule, canvas, next_pointer_on_canvas, &second_copied_path);

    canvas = ret.first;
    cout << "in CanvasAndBittreeProgram" << endl;
    for(int i = 0;i<path->size();i++)
    {
        cout << copied_path[i] <<" ";
    }
    cout << endl;
    if(canvas == nullptr)
    {
        cout << "nullptr *" << endl;
    }
    else
    {
        cout << canvas->to_string__one_line() << endl;
    }
    cout << "out CanvasAndBittreeProgram" << endl;
    cout << endl;

    push_back(PathAndSequenceOfRules(copied_path, rule));

}

string CanvasAndBittreeProgram::to_string() {
    string ret;
    ret += AutomatonRule::to_string();
    ret += ";\n";
    ret += canvas->slim_tree_to_string(0);
    return ret;
}

AutomatonRuleCost CanvasAndBittreeProgram::get_cost() const{
    return cost;
}

SequenceOfPrimitiveRules::SequenceOfPrimitiveRules(PrimitiveRule rule, PrimitiveRule rule1) {
    push_back(rule);
    push_back(rule1);
}

SequenceOfPrimitiveRules::SequenceOfPrimitiveRules(PrimitiveRule rule, SequenceOfPrimitiveRules suffix) {
    push_back(rule);
    for(int i = 0;i<suffix.size();i++)
    {
        push_back(suffix[i]);
    }
}

SequenceOfPrimitiveRules::SequenceOfPrimitiveRules(PrimitiveRule rule, PrimitiveRule rule1, PrimitiveRule rule2) {
    push_back(rule);
    push_back(rule1);
    push_back(rule2);
}

SequenceOfPrimitiveRules::SequenceOfPrimitiveRules(PrimitiveRule rule) {
    push_back(rule);
}

SequenceOfPrimitiveRules::SequenceOfPrimitiveRules(PrimitiveRule rule, PrimitiveRule rule1, PrimitiveRule rule2,
                                                   PrimitiveRule rule3) {
    push_back(rule);
    push_back(rule1);
    push_back(rule2);
    push_back(rule3);
}

SequenceOfPrimitiveRules::SequenceOfPrimitiveRules(SequenceOfPrimitiveRules prefix, SequenceOfPrimitiveRules suffix) {
    for(int i = 0;i<prefix.size();i++)
    {
        push_back(prefix[i]);
    }
    for(int i = 0;i<suffix.size();i++)
    {
        push_back(suffix[i]);
    }
}
