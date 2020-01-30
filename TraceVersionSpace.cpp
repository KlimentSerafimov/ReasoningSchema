//
// Created by Kliment Serafimov on 1/19/20.
//

#include "TraceVersionSpace.h"

TraceOperation::TraceOperation(TraceOperationType type, TraceNode *operand, Bitvector _subdomain_mask) {
    assert(type == compact_poset_operation);

    subdomain_mask = _subdomain_mask;

    vector<MetaExample> individually_consistent_meta_examples =
            get_meta_examples_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
                    subdomain_mask, operand->trace_state.meta_examples);

    compact_poset =
            new CompactPoset(
                    subdomain_mask.get_size(), subdomain_mask, subdomain_mask, individually_consistent_meta_examples);

    prune_globally_inconsistent_meta_examples(
            operand->trace_state.meta_examples, subdomain_mask, compact_poset);

    if(individually_consistent_meta_examples.size() == 0)
    {
        has_empty_output = true;
        return;
    }

    if(operand->is_result_from.size() == 1)
    {
        if(operand->is_result_from[0] != NULL)
        {
            TraceNode* pre_operand = operand->is_result_from[0]->output;

        }
    }
    else
    {
        assert(false);
    }

    add_operand(operand);

//    compact_poset->soft_delete_redundant_edges();
}

TraceNode* TraceOperation::get_output(){
    if(output == NULL && !has_empty_output)
    {
        assert(operands.size() == 1);
        output = new TraceNode(this,
                get_meta_examples_after_query(
                        subdomain_mask, compact_poset, operands[0]->trace_state.meta_examples, false, false));
    }
    return output;
}

void TraceOperation::add_operand(TraceNode* operand) {
    operands.push_back(operand);
    operand->is_operand_for.push_back(this);
}

void TraceNode::get_leafs(vector<TraceNode *>& ret_leafs) {
    if(is_operand_for.size() == 0)
    {
        ret_leafs.push_back(this);
    }
    else
    {
        for(int i = 0;i<is_operand_for.size();i++)
        {
            is_operand_for[i]->get_output()->get_leafs(ret_leafs);
        }
    }
}

string TraceNode::string__of__root_to_this_path() {
    vector<TraceOperation*> parents;
    TraceNode* at = this;
    TraceNode* root = NULL;
    while(at != NULL)
    {
        root = at;
        assert(at->is_result_from.size() == 1);
        if(at->is_result_from[0] != NULL) {
            parents.push_back(at->is_result_from[0]);
            assert(at->is_result_from[0]->get_output() == at);
            assert(at->is_result_from[0]->operands.size() == 1);
            at = at->is_result_from[0]->operands[0];
        }
        else
        {
            break;
        }
    }
    assert(root != NULL);

    reverse(parents.begin(), parents.end());
    string ret = std::to_string(root->trace_state.num_missing_bits) + " ";
    for(int i = 0; i<parents.size();i++)
    {
        if(i >= 1)
        {
            ret += " -> ";
        }

        ret += parents[i]->subdomain_mask.to_string() + " " + std::to_string(parents[i]->get_output()->trace_state.num_missing_bits);
    }
    return ret;
}
