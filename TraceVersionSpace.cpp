//
// Created by Kliment Serafimov on 1/19/20.
//

#include "TraceVersionSpace.h"

TraceOperation::TraceOperation(TraceOperationType type, TraceNode *operand, Bitvector _subdomain_mask) {
    assert(type == compact_poset_operation);

    operation_type = type;

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

    add_operand(operand);
    if (get_output() != NULL) {
        bool found_union;
        TraceOperation* best_union_alternative = NULL;
        TraceNode *at_operand = operand;
        int count = 0;
        do {
            found_union = false;
            if (at_operand->is_result_from.size() == 1) {
                if (at_operand->is_result_from[0] != NULL) {

                    TraceNode *operand_pre_operand = at_operand->init_find_origin(
                            at_operand->is_result_from[0]->operands, 0);
                    TraceOperation *skip_operation_pre_operand =
                            new TraceOperation(type, operand_pre_operand, subdomain_mask);
                    if (skip_operation_pre_operand->get_output() != NULL) {
                        TraceOperation *local_union_alternative =
                                new TraceOperation(
                                        trace_union_operation, operand,
                                        skip_operation_pre_operand->get_output());
                        TraceNode *union_output = local_union_alternative->get_output();
                        if (union_output->trace_state.num_missing_bits ==
                            get_output()->trace_state.num_missing_bits) {
                            count += 1;
//                            if (count >= 2) {
//                                cout << "HERE count = " << count << endl;
//                                cout << best_union_alternative->get_output()->string__of__path_from_root_to_this() << endl;
//                                cout << union_output->string__of__path_from_root_to_this() << endl;
//                            }
                            found_union = true;
                            delete best_union_alternative;
                            best_union_alternative = local_union_alternative;
                            at_operand = operand_pre_operand;


//                            can_be_turned_into_union = true;
//                            cout << endl;
//                            cout << get_output()->string__of__path_from_root_to_this() << endl;
//                            cout << skip_operation_pre_operand->get_output()->string__of__path_from_root_to_this() << endl;
//                            cout << union_output->string__of__path_from_root_to_this() << endl;
//                            cout << endl;

                        } else {
                            delete local_union_alternative;
                            delete skip_operation_pre_operand;
                        }
                    } else {
                        delete skip_operation_pre_operand;
                    }
                }
            } else {
                assert(false);
            }
        } while (found_union);

        if (best_union_alternative != NULL) {
            can_be_turned_into_union = true;
            union_alternative = best_union_alternative;
        }
    }

//    TraceNode *operand_pre_operand = operand->find_origin(operand->is_result_from[0]->operands);
//    TraceOperation *skip_operation_pre_operand =
//            new TraceOperation(type, operand_pre_operand, subdomain_mask);
//    if(skip_operation_pre_operand->get_output() != NULL) {
//        union_alternative =
//                new TraceOperation(
//                        trace_union_operation, operand, skip_operation_pre_operand->get_output());
//        TraceNode *union_output = union_alternative->get_output();
//        if (union_output->trace_state.num_missing_bits == get_output()->trace_state.num_missing_bits) {
//            can_be_turned_into_union = true;
//            cout << endl;
//            cout << get_output()->string__of__path_from_root_to_this() << endl;
//            cout << skip_operation_pre_operand->get_output()->string__of__path_from_root_to_this() << endl;
//            cout << union_output->string__of__path_from_root_to_this() << endl;
//            cout << endl;
////                assert(false);
//        } else {
//            delete union_alternative;
//            delete skip_operation_pre_operand;
//        }
//    }
//    else
//    {
//        delete skip_operation_pre_operand;
//    }

//    compact_poset->soft_delete_redundant_edges();
}

TraceNode* TraceOperation::get_output(){
    if(output == NULL && !has_empty_output)
    {
        bool enter = false;
        if(operation_type == compact_poset_operation) {
            assert(!enter);
            enter = true;
            assert(operands.size() == 1);
            output = new TraceNode(this,
                                   get_meta_examples_after_query(
                                           subdomain_mask, compact_poset, operands[0]->trace_state.meta_examples, false,
                                           false));
        }
        if(operation_type == trace_union_operation)
        {
            assert(!enter);
            enter = true;
            assert(operands.size() == 2);

            assert(operands[0]->trace_state.meta_examples.size() == operands[1]->trace_state.meta_examples.size());

            vector<MetaExample> meta_examples;
            for(int i = 0;i<operands[0]->trace_state.meta_examples.size(); i++)
            {
                assert(
                        operands[0]->trace_state.meta_examples[i].generalization ==
                        operands[1]->trace_state.meta_examples[i].generalization);
                meta_examples.push_back(
                        MetaExample(
                                operands[0]->trace_state.meta_examples[i].partial_function.get_composition(
                                        operands[1]->trace_state.meta_examples[i].partial_function),
                                operands[0]->trace_state.meta_examples[i].generalization,
                                i
                                ));
            }

            output = new TraceNode(this, meta_examples);
        }
    }
    return output;
}

void TraceOperation::add_operand(TraceNode* operand) {
    operands.push_back(operand);
    operand->is_operand_for.push_back(this);
}

TraceOperation::TraceOperation(TraceOperationType type, TraceNode *first_operand, TraceNode *second_operand) {
    assert(type == trace_union_operation);
    operation_type = type;
    add_operand(first_operand);
    add_operand(second_operand);
}

string TraceOperation::to_string() {
    if(operation_type == compact_poset_operation)
    {
        return subdomain_mask.to_string() + " " + std::to_string(get_output()->trace_state.num_missing_bits);
    }
    else
    {
        assert(operation_type == trace_union_operation);
        return "union " + std::to_string(get_output()->trace_state.num_missing_bits);
    }

}

TraceOperation::~TraceOperation() {
    for(int i = 0;i<operands.size();i++)
    {
        bool enter = false;
        for(int j = operands[i]->is_operand_for.size()-1; j>=0;j--)
        {
            if(operands[i]->is_operand_for[j] == this)
            {
                assert(j >= (int)operands[i]->is_operand_for.size() - 2);
                enter = true;
                operands[i]->is_operand_for.erase(operands[i]->is_operand_for.begin()+j);
                break;
            }
        }
        assert(enter);
    }
    operands.clear();
    if(compact_poset != NULL)
    {
        compact_poset->clear();
    }
    delete output;
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

TraceNode* TraceNode::init_find_origin(vector<TraceNode*> operands, int depth)
{
    memset_visited(origin_type, depth);
    return find_origin(operands, depth);
}

TraceNode* TraceNode::find_origin(vector<TraceNode*> operands, int depth)
{
    if(operands.size() == 1)
    {
        return operands[0];
    }
    TraceNode* origin = NULL;
    for(int i = 0;i<operands.size();i++)
    {
        TraceNode* at = operands[i];
        while(at != NULL)
        {
            at->mark(depth);
            assert(at->num_markers <= operands.size());
            if(at->num_markers == operands.size())
            {
                return at;
            }
            assert(at->is_result_from.size() == 1);
            if(at->is_result_from[0] != NULL) {
                assert(at->is_result_from[0]->get_output() == at);
                if (at->is_result_from[0]->operands.size() == 1) {
//                where_enter = "First";
//                cout << "A " << i << endl;
                    at = at->is_result_from[0]->operands[0];
                } else {
//                where_enter = "Second";
//                cout << "B " << i << endl;
                    at = init_find_origin(at->is_result_from[0]->operands, depth + 1);
                }
            }
            else
            {
                at = NULL;
            }
        }
    }
    assert(false);
    return origin;
}

string TraceNode::string_from_origin_to_operands(vector<TraceNode*> operands, int depth)
{
    TraceNode* origin = init_find_origin(operands, depth);
    string ret = "{";

    for(int i = 0;i<operands.size();i++)
    {
        if(i >= 1)
        {
            ret += " | ";
        }
        TraceNode* at = operands[i];
        string local_string;
        while(at != origin)
        {
            assert(at->is_result_from.size() == 1);
            TraceOperation* operation = at->is_result_from[0];
            assert(operation->get_output() == at);
            TraceNode* local_origin;
            string base;
            if(operation->operands.size() == 1)
            {
                base = operation->to_string();

                at = at->is_result_from[0]->operands[0];
            }
            else
            {
                base =
                        string_from_origin_to_operands(operation->operands, depth+1) + " -> " + operation->to_string();
                at = init_find_origin(at->is_result_from[0]->operands, depth+1);
            }
            if(local_string == "")
            {
                local_string = base;
            }
            else
            {
                local_string = base + " -> " + local_string;
            }
        }
        ret += local_string;
    }

    ret += "}";
    return ret;
}

string TraceNode::string__of__path_from_root_to_this()
{
    string ret;
    TraceNode* at = this;
    assert(at->is_result_from.size() == 1);
    TraceOperation* operation = at->is_result_from[0];
    if(operation != NULL) {
//        parents.push_back(operation);
        assert(operation->get_output() == at);
        if(operation->operands.size() == 1)
        {
//            at = operation->operands[0];
            ret += operation->operands[0]->string__of__path_from_root_to_this();
        }
        else if(operation->operands.size() == 2)
        {
//            at = find_origin(operation->operands);
            TraceNode* origin = init_find_origin(operation->operands, 0);
            ret += origin->string__of__path_from_root_to_this() + " -> ";
            ret += string_from_origin_to_operands(operation->operands, 0);
        }
        else
        {
            assert(false);
        }
        ret += " -> " + operation->to_string();
    }
    else
    {
        TraceNode* root = at;
        ret = std::to_string(root->trace_state.num_missing_bits) + " ";
    }

    return ret;

}


TraceVersionSpace::TraceVersionSpace(vector<MetaExample> _meta_examples, vector<Bitvector> masks)
{
    cout << endl;
    cout << "meta_examples" << endl;
    for(int i = 0;i<_meta_examples.size();i++)
    {
        cout << _meta_examples[i].to_string() << endl;
    }
    cout << endl;

    root = new TraceNode(_meta_examples);
    all_nodes.push_back(root);
//
//        priority_queue<
//                pair<int, pair<TraceNode*, int>>, vector<pair<int, pair<TraceNode*, int>>>, greater<pair<int, pair<TraceNode*, int>> >
//                        > fronteer;

    multiset<HeuristicScoreAndSolution> fronteer;

    for(int i = 0;i<masks.size();i++) {
        fronteer.insert(HeuristicScoreAndSolution(masks[i].count(), masks[i].count(), root->trace_state.num_missing_bits, root, i));
    }

    int num_inserts = 0;

    int in_q_prev_max_widht = 0;
    int in_q_prev_sum_widths = 0;

    int while_loop_count = 0;
    int remaining_while_loop_count = 100000;

    while(!fronteer.empty())
    {
        while_loop_count++;
        remaining_while_loop_count--;

        auto at_pointer = fronteer.begin();
        HeuristicScoreAndSolution at = (*at_pointer);
        fronteer.erase(at_pointer);

        if(in_q_prev_max_widht != at.max_width || in_q_prev_sum_widths != at.sum_width)
        {
            cout << "max_width " << at.max_width << " sum_width " << at.sum_width <<  " @ |num_inserts| = " << num_inserts << " |while| = " << while_loop_count << " |size| = " << fronteer.size() << endl;
            in_q_prev_max_widht = at.max_width;
            in_q_prev_sum_widths =  at.sum_width;
        }

        TraceOperation* operation = new TraceOperation(compact_poset_operation, at.at, masks[at.next_mask_id]);
        if(operation->can_be_turned_into_union)
        {
            TraceOperation* new_operation = operation->union_alternative;
            delete operation;
            operation = new_operation;
        }
        TraceNode *output = operation->get_output();
        if (output != NULL) {
            for (int i = 0; i < masks.size(); i++) {
                fronteer.insert(
                        HeuristicScoreAndSolution(
                                max(at.max_width, (int) masks[i].count()),
                                at.sum_width + masks[i].count(),
                                output->trace_state.num_missing_bits,
                                output, i));
                num_inserts++;
                if (fronteer.size() > remaining_while_loop_count) {
                    auto fronteer_end = fronteer.end();
                    fronteer_end--;
                    fronteer.erase(fronteer_end);
                }
            }
        } else {
            delete operation;
        }
    }

    vector<TraceNode*> leafs;
    root->get_leafs(leafs);

    assert(leafs.size() >= 1);

    cout << "traces" << endl;
    for(int i = 0;i<leafs.size(); i++)
    {
        cout << leafs[i]->string__of__path_from_root_to_this() << endl;
    }

}