//
// Created by Kliment Serafimov on 8/18/20.
//

#ifndef COMPACTPOSETBittreeInputOutputType_H
#define COMPACTPOSETBittreeInputOutputType_H



#include "BitInBittree.h"
#include "Task.h"
#include "util.h"

class BittreeInputOutputType: public TreeNode
{
public:
    BittreeNode* input = nullptr;
    BittreeNode* output = nullptr;

    BittreeInputOutputType(TreeNode *_parent, Name name, NodeType input_node_type, NodeType output_node_type);

    BittreeInputOutputType(TreeNode *_parent, Name name, BittreeInputOutputType *to_copy, bool all_new_bits);

    void append_bits(vector<BitInBittree*>& bits)
    {
        input->append_bits(bits);
        output->append_bits(bits);
    }

    string to_string(int num_tabs)
    {
        string ret = "";
        int next_tabs = num_tabs;
        int next_next_tabs = next_tabs+1;
        string open_bracket = "\n"+tabs(next_tabs) + "{\n";
        string close_bracket = tabs(next_tabs) + "}\n";
        ret += tabs(num_tabs)+"input"+input->TreeNode::to_string()+open_bracket + input->to_string(next_next_tabs) + close_bracket;
        ret += tabs(num_tabs)+"output"+output->TreeNode::to_string()+open_bracket + output->to_string(next_next_tabs) + close_bracket;
        return ret;
    }

    string bits_to_string(int num_tabs)
    {
        string ret = "";

        int next_tabs = num_tabs;
        int next_next_tabs = next_tabs+1;
        string open_bracket = "{\n";
        string close_bracket = tabs(num_tabs) + "}\n";
        ret += tabs(next_tabs)+"input"+open_bracket + input->bits_to_string(next_next_tabs) + close_bracket;
        ret += tabs(next_tabs)+"output"+open_bracket + output->bits_to_string(next_next_tabs) + close_bracket;

        return ret;
    }

    void apply_delta(BittreeInputOutputType* delta)
    {
        input->apply_delta(delta->input);
        output->apply_delta(delta->output);
    }

    BittreeNode * add_input_child(NodeType child_type);

    BittreeNode * add_output_child(NodeType child_type);

    BittreeNode * add_output_child(NodeType child_type, BitInBittreeType bit_in_bittree_type);

    void solve(Task *task_name);

};


#endif //COMPACTPOSETBittreeInputOutputType_H
