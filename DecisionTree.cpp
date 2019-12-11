//
// Created by Kliment Serafimov on 2019-11-20.
//

#include "util.h"
#include "DecisionTree.h"
#include <vector>
#include <queue>
#include <iostream>
#include <stack>

using namespace std;

Node::Node()
{

}

vector<pair<int, int> > get__idxs_and_branches(PartialFunction partial_function)
{
    vector<pair<int, int> > idxs_and_branches;

    for(int i = 0; i<partial_function.function_size;i++)
    {
        if(get_bit(partial_function.partition, i))
        {
            idxs_and_branches.emplace_back(i, get_bit(partial_function.total_function, i));
        }
    }

    return idxs_and_branches;
}

#define MAX_NUM_NODES 10000000

static int global_num_decision_tree_nodes = 0;
vector<int> empty_slots;
Node all_nodes[MAX_NUM_NODES];

Node* get_new_node()
{
    assert(global_num_decision_tree_nodes < MAX_NUM_NODES);
    if(empty_slots.empty()) {
        all_nodes[global_num_decision_tree_nodes] = Node(global_num_decision_tree_nodes);
        return &all_nodes[global_num_decision_tree_nodes++];
    }
    else
    {
        int new_id = empty_slots.back();
        empty_slots.pop_back();
        all_nodes[new_id] = Node(new_id);
        return &all_nodes[new_id];
    }
}

int get__global_num_decision_tree_nodes() {
    return global_num_decision_tree_nodes;
}

int get__empty_slots_count() {
    return empty_slots.size();
}


void DecisionTree::init(PartialFunction partial_function)
{

    vector<pair<int, int> > idxs_and_branches = get__idxs_and_branches(partial_function);

    root = get_new_node();

    Node *at = root;
    for (int i = 0; i < idxs_and_branches.size(); i++) {
        int idx = idxs_and_branches[i].first;
        int branch = idxs_and_branches[i].second;

        at->node_type = internal_node;
        at->idx = idx;
        at->branches[branch] = get_new_node();

        at->branches[1 - branch] = get_new_node();
        at->branches[1 - branch]->node_type = leaf_node;
        at->branches[1 - branch]->value = false;

        at = at->branches[branch];
    }
    at->node_type = leaf_node;
    at->value = true;
}

DecisionTree::DecisionTree(PartialFunction partial_function)
{
    init(partial_function);
}

DecisionTree::DecisionTree(PartialFunction partial_function, OperationType operation_type, DecisionTree *other) {
    init(partial_function);
    apply_operation(operation_type, other);
}

DecisionTree::DecisionTree() = default;


void DecisionTree::apply_operation(OperationType operation_type, DecisionTree *other) {
    map<int, int> idx_to_branch;
    root->apply_operation(operation_type, other->root, &idx_to_branch);
    assert(idx_to_branch.empty());
}

string DecisionTree::to_string(int num_inputs) {
    return root->to_string(0, num_inputs);
}


string DecisionTree::to_string(int num_tabs, int num_inputs) {
    return root->to_string(num_tabs, num_inputs);
}

DecisionTree DecisionTree::copy() {
    queue<Node*> my_nodes;
    queue<Node*> new_nodes;

    assert(root->node_type == internal_node || root->node_type == leaf_node);
    my_nodes.push(root);
    Node* new_root = get_new_node();
    new_nodes.push(new_root);

    while(!my_nodes.empty())
    {
        Node* my_node = my_nodes.front();
        Node* new_node = new_nodes.front();

        my_nodes.pop();
        new_nodes.pop();

        assert(my_node->node_type == internal_node || my_node->node_type == leaf_node);

        if(my_node->node_type == internal_node)
        {
            new_node->node_type = internal_node;
            new_node->idx = my_node->idx;
            new_node->branches[0] = get_new_node();
            new_node->branches[1] = get_new_node();
            assert(my_node->branches[0]->node_type == internal_node || my_node->branches[0]->node_type == leaf_node);
            assert(my_node->branches[1]->node_type == internal_node || my_node->branches[1]->node_type == leaf_node);
            my_nodes.push(my_node->branches[0]);
            my_nodes.push(my_node->branches[1]);
            new_nodes.push(new_node->branches[0]);
            new_nodes.push(new_node->branches[1]);
        }
        else
        {
            assert(my_node->node_type == leaf_node);
            new_node->node_type = leaf_node;
            new_node->value = my_node->value;
        }
    }

    DecisionTree ret;
    ret.root = new_root;
    return ret;
}

bool DecisionTree::is_empty() {
    return root->is_empty();
}

bool Node::is_empty()
{
    if(node_type == leaf_node)
    {
        if(value == true)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if(branches[0]->node_type == leaf_node && branches[1]->node_type == leaf_node)
        {
            if (branches[0]->value == branches[1]->value)
            {
                node_type = leaf_node;
                value = branches[0]->value;
                branches[0]->my_delete();
                branches[1]->my_delete();
                return is_empty();
            }
        }
        return branches[0]->is_empty() && branches[1]->is_empty();
    }
}

DecisionTree::DecisionTree(Node *_root) {
    root = _root;
}

Node* DecisionTree::get_root() {
    return root;
}

void Node::apply_operation(OperationType operation_type, Node *other, map<int, int> *idx_to_branch) {
    if(node_type == internal_node)
    {
        (*idx_to_branch)[idx] = 0;
        branches[0]->apply_operation(operation_type, other, idx_to_branch);
        (*idx_to_branch)[idx] = 1;
        branches[1]->apply_operation(operation_type, other, idx_to_branch);
        idx_to_branch->erase(idx);

        if(branches[0]->node_type == leaf_node && branches[1]->node_type == leaf_node)
        {
            if(branches[0]->value == branches[1]->value)
            {
                node_type = leaf_node;
                value = branches[0]->value;
                branches[0]->my_delete();
                branches[1]->my_delete();
            }
        }
    }
    else
    {
        assert(node_type == leaf_node);
        if((value == true && operation_type != my_union) || (value == false && operation_type == my_union))
        {
            vector<Node*> other_nodes;
            vector<Node*> mine_nodes;
            other_nodes.push_back(other);
            mine_nodes.push_back(this);
            stack<vector<Node*> > new_layers_of_nodes;
            while(other_nodes.size() != 0)
            {
                vector<Node*> next_other_nodes;
                vector<Node*> next_mine_nodes;
                assert(other_nodes.size() == mine_nodes.size());
                for(int i = 0;i<other_nodes.size();i++) {
                    Node* at_other = other_nodes[i];
                    Node* at_mine = mine_nodes[i];
                    if (at_other->node_type == internal_node)
                    {
                        if (idx_to_branch->find(at_other->idx) == idx_to_branch->end())
                        {
                            //need to split
                            at_mine->node_type = internal_node;
                            at_mine->idx = at_other->idx;
                            at_mine->branches[0] = get_new_node();
                            at_mine->branches[1] = get_new_node();
                            next_mine_nodes.push_back(at_mine->branches[0]);
                            next_other_nodes.push_back(at_other->branches[0]);
                            next_mine_nodes.push_back(at_mine->branches[1]);
                            next_other_nodes.push_back(at_other->branches[1]);
                        }
                        else
                        {
                            next_mine_nodes.push_back(at_mine);
                            next_other_nodes.push_back(at_other->branches[(*idx_to_branch)[at_other->idx]]);
                        }
                    }
                    else
                    {
                        assert(at_other->node_type == leaf_node);
                        at_mine->node_type = leaf_node;
                        if(at_other->value == true)
                        {
                            if(operation_type == intersection || operation_type == my_union)
                            {
                                at_mine->value = true;
                            }
                            else
                            {
                                assert(operation_type == difference);
                                at_mine->value = false;
                            }
                        }
                        else
                        {
                            if(operation_type == intersection || operation_type == my_union)
                            {
                                at_mine->value = false;
                            }
                            else
                            {
                                assert(operation_type == difference);
                                at_mine->value = true;
                            }
                        }
                    }
                }
                new_layers_of_nodes.push(mine_nodes);
                other_nodes = next_other_nodes;
                mine_nodes = next_mine_nodes;
                assert(other_nodes.size() == mine_nodes.size());
            }
        }
        else
        {
            //do nothing;
        }
    }
}

static string int_to_bool_string[2] = {"false", "true"};

string Node::to_string(int num_tabs, int num_inputs) {
    string ret;
    if(node_type == leaf_node) {
        ret += tabs(num_tabs)+"value = " + int_to_bool_string[value];
        assert(value == 0 || value == 1);
    }
    else
    {
        assert(node_type == internal_node);
        ret += tabs(num_tabs)+"if(f(" + bitvector_to_str(idx, num_inputs) + ")){\n";
        ret += branches[1]->to_string(num_tabs+1, num_inputs);
        ret += "\n";
        ret += tabs(num_tabs) + "} else{\n";
        ret += branches[0]->to_string(num_tabs+1, num_inputs);
        ret += "\n";
        ret += tabs(num_tabs) + "}";
    }
    return ret;
}

void Node::get_union_of_partial_functions(
        int num_inputs, vector<PartialFunction> &union_of_partial_functions, map<int, int> *idx_to_branch) {

    if(node_type == internal_node)
    {
        (*idx_to_branch)[idx] = 0;
        branches[0]->get_union_of_partial_functions(num_inputs, union_of_partial_functions, idx_to_branch);
        (*idx_to_branch)[idx] = 1;
        branches[1]->get_union_of_partial_functions(num_inputs, union_of_partial_functions, idx_to_branch);
        idx_to_branch->erase(idx);
    }
    else
    {
        if(value == true) {
            int total_function = 0;
            int partition = 0;
            for (map<int, int>::iterator it = idx_to_branch->begin(); it != idx_to_branch->end(); it++) {
                int idx = (*it).first;
                int val = (*it).second;
                total_function += (1 << idx) * val;
                partition += (1 << idx);
            }
            union_of_partial_functions.push_back(PartialFunction(num_inputs, total_function, partition));
        }
    }
}

Node::Node(int _global_id) {
    global_id = _global_id;
}

void Node::my_delete() {
    if(node_type == internal_node)
    {
        branches[0]->my_delete();
        branches[1]->my_delete();
        all_nodes[global_num_decision_tree_nodes].branches[0] = NULL;
        all_nodes[global_num_decision_tree_nodes].branches[1] = NULL;
    }
    empty_slots.push_back(global_id);
}

bool Node::contains(PartialFunction partial_function) {
    if(node_type == leaf_node)
    {
        return value;
    }
    else
    {
        assert(node_type == internal_node);
        if(get_bit(partial_function.partition, idx))
        {
            contains_counter+=1;
//            cout << contains_counter << endl;
            return branches[get_bit(partial_function.total_function, idx)]->contains(partial_function);
        }
        else
        {
            return branches[0]->contains(partial_function) || branches[1]->contains(partial_function);
        }
    }
}

void Node::get_union_of_partial_functions_that_contain_partial_function(
        int num_inputs, PartialFunction partial_function, vector<PartialFunction> *union_of_partial_functions, map<int, int> *idx_to_branch) {
    if(node_type == internal_node)
    {
        if(!partial_function.has_output(idx)) {
            (*idx_to_branch)[idx] = 0;
            branches[0]->get_union_of_partial_functions_that_contain_partial_function(
                    num_inputs, partial_function,union_of_partial_functions, idx_to_branch);
            (*idx_to_branch)[idx] = 1;
            branches[1]->get_union_of_partial_functions_that_contain_partial_function(
                    num_inputs, partial_function, union_of_partial_functions, idx_to_branch);
            idx_to_branch->erase(idx);
        } else
        {
            int output = partial_function.get_output(idx);
            (*idx_to_branch)[idx] = output;
            branches[output]->get_union_of_partial_functions_that_contain_partial_function(
                    num_inputs, partial_function, union_of_partial_functions, idx_to_branch);
            idx_to_branch->erase(idx);
        }
    }
    else
    {
        if(value == true) {
            int total_function = 0;
            int partition = 0;
            for (map<int, int>::iterator it = idx_to_branch->begin(); it != idx_to_branch->end(); it++) {
                int idx = (*it).first;
                int val = (*it).second;
                total_function += (1 << idx) * val;
                partition += (1 << idx);
            }
            union_of_partial_functions->push_back(
                    PartialFunction(num_inputs, total_function, partition).get_composition(partial_function));
        }
    }
}


vector<PartialFunction> DecisionTree::get_union_of_partial_functions(int num_inputs)
{
    vector<PartialFunction> ret;
//    map<int, int> idx_to_branch;
//    root->get_union_of_partial_functions(num_inputs, ret, &idx_to_branch);
//    return ret;
    append_union_of_partial_functions_that_contain_partial_function(num_inputs, PartialFunction(num_inputs, 0, 0), &ret);
    return ret;
}


void DecisionTree::append_union_of_partial_functions_that_contain_partial_function(
        int num_inputs, PartialFunction partial_function, vector<PartialFunction> *ret) {
    map<int, int> idx_to_branch;
    root->get_union_of_partial_functions_that_contain_partial_function(num_inputs, partial_function, ret, &idx_to_branch);
}

string DecisionTree::get_string_of_union_of_partial_functions(int num_tabs, int num_inputs) {
    string ret;
    vector<PartialFunction> union_of_partial_functions = get_union_of_partial_functions(num_inputs);
    for(int i = 0; i<union_of_partial_functions.size();i++)
    {
        ret+=tabs(num_tabs)+union_of_partial_functions[i].to_string()+"\n";
    }
    return ret;
}

void DecisionTree::my_delete() {
    if(root != NULL) {
        root->my_delete();
    }
}

bool DecisionTree::contains(PartialFunction partial_function) {
    return root->contains(partial_function);
}


