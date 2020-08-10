//
// Created by Kliment Serafimov on 1/19/20.
//

#include "TraceVersionSpace.h"

pair<TraceOperation*, bool> get_trace_operation(TraceOperationType type, TraceNode *operand, Bitvector _subdomain_mask, bool consider_union)
{
    for(int i = 0;i<operand->is_operand_for.size();i++)
    {
        if(operand->is_operand_for[i] != nullptr) {
            if (operand->is_operand_for[i]->subdomain_mask == _subdomain_mask) {
                return make_pair(operand->is_operand_for[i], false);
            }
        }
    }
    return make_pair(new TraceOperation(type, operand, _subdomain_mask, consider_union), true);

}

void TraceOperation::possible_refactor_into_union()
{
    assert(operation_type == compact_poset_operation);
    assert(operands.size() == 1);
    if (get_output() != nullptr)
    {
        bool found_union;
        TraceOperation* best_union_alternative = nullptr;
        pair<TraceOperation*, bool> best_skip_union_pre_operand; //make_pair(nullptr, false);
        best_skip_union_pre_operand.first = nullptr;
        best_skip_union_pre_operand.second = false;
        TraceNode *at_operand = operands[0];
        int count = 0;
        do {
            found_union = false;
            if (at_operand->is_result_from.size() == 1) {
                if (at_operand->is_result_from[0] != nullptr) {

                    TraceNode *operand_pre_operand = at_operand->init_find_origin(
                            0, 0);
                    pair<TraceOperation *, bool> skip_operation_pre_operand =
                            get_trace_operation(compact_poset_operation, operand_pre_operand, subdomain_mask, false);
//                            new TraceOperation(type, operand_pre_operand, subdomain_mask);
                    if (skip_operation_pre_operand.first->get_output() != nullptr) {
                        TraceOperation *local_union_alternative =
                                new TraceOperation(
                                        trace_union_operation, operands[0],
                                        skip_operation_pre_operand.first->get_output());
                        TraceNode *union_output = local_union_alternative->get_output();
                        if (union_output->trace_state.num_missing_bits ==
                            get_output()->trace_state.num_missing_bits){
                            count += 1;
                            found_union = true;
                            delete best_union_alternative;
                            best_union_alternative = local_union_alternative;
                            if(
                                    union_output->trace_state.num_missing_bits ==
                                    skip_operation_pre_operand.first->get_output()->trace_state.num_missing_bits)
                            {
//                                cout << "ONLY HEAD" << endl;
//
//                                cout << get_output()->string__of__path_from_root_to_this() << endl;
//                                cout << union_output->string__of__path_from_root_to_this() << endl;
//                                cout << skip_operation_pre_operand.first->get_output()->string__of__path_from_root_to_this() << endl;
//
//                                cout << endl;
                                if(best_skip_union_pre_operand.second)
                                {
                                    delete best_skip_union_pre_operand.first;
                                }
                                best_skip_union_pre_operand = skip_operation_pre_operand;
                            }
                            at_operand = operand_pre_operand;
                        } else {
                            delete local_union_alternative;
                            if(skip_operation_pre_operand.second) {
                                delete skip_operation_pre_operand.first;
                            }
                        }
                    } else {
                        if(skip_operation_pre_operand.second) {
                            delete skip_operation_pre_operand.first;
                        }
                    }
                }
            } else {
                assert(false);
            }
//            cout << "here" << endl;
        } while (found_union);

        if (best_union_alternative != nullptr) {
            can_be_turned_into_union = true;
            union_alternative = best_union_alternative;

            if(best_skip_union_pre_operand.first != nullptr)
            {
                can_replace_with_head_only = true;
                head_only_alternative = best_skip_union_pre_operand;
            }
        } else
        {
            assert(best_skip_union_pre_operand.first == nullptr);
        }
    }
}

TraceOperation::TraceOperation(TraceOperationType type, TraceNode *operand, Bitvector _subdomain_mask, bool consider_union) {
    assert(type == compact_poset_operation);

    for(int i = 0;i<operand->is_operand_for.size();i++)
    {
        assert(operand->is_operand_for[i]->subdomain_mask != _subdomain_mask);
    }

    operation_type = type;

    subdomain_mask = _subdomain_mask;

    vector<MetaExample> individually_consistent_meta_examples =
            get_meta_examples_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
                    subdomain_mask, operand->trace_state.meta_examples);

    compact_poset =
            new CompactPoset(
                    subdomain_mask.get_size(), subdomain_mask, subdomain_mask, individually_consistent_meta_examples);

//    prune_globally_inconsistent_meta_examples(
//            operand->trace_state.meta_examples, subdomain_mask, compact_poset);

    if(individually_consistent_meta_examples.size() == 0)
    {
        has_empty_output = true;
        return;
    }

    add_operand(operand);


    if(consider_union)
        possible_refactor_into_union();


//    compact_poset->soft_delete_redundant_edges();
}

TraceNode* TraceOperation::get_output(){
    if(output == nullptr && !has_empty_output)
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
                                operands[0]->trace_state.meta_examples[i].generalization, i));
            }

            output = new TraceNode(this, meta_examples);
        }
        assert(enter);
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
//                assert(j >= (int)operands[i]->is_operand_for.size() - 2);
                assert(!enter);
                enter = true;
                operands[i]->is_operand_for.erase(operands[i]->is_operand_for.begin()+j);
//                break;
            }
        }
        assert(enter);
    }
    operands.clear();
    if(compact_poset != nullptr)
    {
        compact_poset->clear();
    }
    delete output;
}

void TraceNode::get_leafs(vector<TraceNode *>& ret_leafs) {
    if(!visited(vis_leafs))
    {
        visit(vis_leafs);
    }
    else
    {
        return;
    }
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

TraceNode * TraceNode::init_find_origin(int parent_id, int depth)
{
    if(origin_per_is_result_from[parent_id] == nullptr) {
        vector<TraceNode *> operands = is_result_from[parent_id]->operands;
        memset_visited(vis_origin, depth);
        origin_per_is_result_from[parent_id] = find_origin(operands, depth);
        return origin_per_is_result_from[parent_id];
    }
    else
    {
        return origin_per_is_result_from[parent_id];
    }
}

TraceNode* TraceNode::find_origin(vector<TraceNode*> operands, int depth)
{
    if(operands.size() == 1)
    {
        return operands[0];
    }
    TraceNode* origin = nullptr;
    for(int i = 0;i<operands.size();i++)
    {
        TraceNode* at = operands[i];
        while(at != nullptr)
        {
            at->mark(depth);
            assert(at->num_markers[depth] <= operands.size());
            if(at->num_markers[depth] == operands.size())
            {
                return at;
            }
            assert(at->is_result_from.size() == 1);
            if(at->is_result_from[0] != nullptr) {
                assert(at->is_result_from[0]->get_output() == at);
                if (at->is_result_from[0]->operands.size() == 1) {
//                where_enter = "First";
//                cout << "A " << i << endl;
                    at = at->is_result_from[0]->operands[0];
                } else {
//                where_enter = "Second";
//                cout << "B " << i << endl;
                    at = at->init_find_origin(0, depth + 1);
                }
            }
            else
            {
                at = nullptr;
            }
        }
    }
    assert(false);
    return origin;
}

string TraceNode::string_from_origin_to_operands(int parent_id, int depth)
{
    vector<TraceNode*> operands = is_result_from[parent_id]->operands;
    TraceNode* origin = init_find_origin(parent_id, depth);
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
                at = operation->operands[0];
            }
            else
            {
                base =
                        at->string_from_origin_to_operands(0, depth+1) + " -> " +
                        operation->to_string();
                at = at->init_find_origin(0, depth + 1);
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
    if(operation != nullptr) {
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
            TraceNode* origin = at->init_find_origin(0, 0);
            ret += origin->string__of__path_from_root_to_this() + " -> ";
            ret += at->string_from_origin_to_operands(0, 0);
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

TraceNode::TraceNode(TraceOperation *parent, vector<MetaExample> meta_examples)
{
    is_result_from.push_back(parent);
    origin_per_is_result_from.push_back(nullptr);
//    origin_per_is_result_from[origin_per_is_result_from.size()-1] = init_find_origin(0, 0);
    init(meta_examples);
}


TraceVersionSpace::TraceVersionSpace(vector<MetaExample> _meta_examples, vector<MaskAndCost> masks)
{
    cout << endl;
    cout << "meta_examples" << endl;
    for(int i = 0;i<_meta_examples.size();i++)
    {
        cout << _meta_examples[i].to_string() << endl;
    }
    cout << endl;

    root = new TraceNode(_meta_examples);

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
    int in_q_prev_remaining_bits = 0;

    int while_loop_count = 0;
    int remaining_while_loop_count = 15000;
    int beam_size = 2500;

    map<pair<TraceNode*, TraceNode*>, TraceOperation*> unions;

    while(!fronteer.empty())
    {
        while_loop_count++;
        remaining_while_loop_count--;

        auto at_pointer = fronteer.begin();
        HeuristicScoreAndSolution at = (*at_pointer);
        fronteer.erase(at_pointer);

        if(
                while_loop_count % 1000 == 0
//              in_q_prev_remaining_bits != at.remaining_bits
//              in_q_prev_max_widht != at.max_width
//              || in_q_prev_sum_widths != at.sum_width
        )
        {
            cout << "max_width " << at.max_width << " sum_width " << at.sum_width << " remaining_bits " << at.remaining_bits <<  " @ |num_inserts| = " << num_inserts << " |while| = " << while_loop_count << " |size| = " << fronteer.size() << endl;
            in_q_prev_max_widht = at.max_width;
            in_q_prev_sum_widths =  at.sum_width;
            in_q_prev_remaining_bits = at.remaining_bits;
        }

        pair<TraceOperation*, bool> operation_with_bool =
                get_trace_operation(compact_poset_operation, at.at, masks[at.next_mask_id], true);
        TraceOperation* operation = operation_with_bool.first;
        bool is_new = operation_with_bool.second;
        bool skip = false;
        if(operation->can_replace_with_head_only)
        {
            assert(is_new);
            pair<TraceOperation*, bool> new_operation = operation->head_only_alternative;
            delete operation->union_alternative;
            if(is_new)
            {
                delete operation;
            }
            operation = new_operation.first;
            skip = !new_operation.second;
        }
        else if(operation->can_be_turned_into_union)
        {
            assert(is_new);
            TraceOperation* new_operation = operation->union_alternative;
            if(is_new)
            {
                delete operation;
            }
            assert(new_operation->operands.size() == 2);
            pair<TraceNode*, TraceNode*> operands = make_pair(
                    new_operation->operands[0], new_operation->operands[1]);
            if(unions.find(operands) != unions.end())
            {
                operation = unions[operands];
//                cout << "resolve redundant:" << endl;
//                cout << operation->get_output()->string__of__path_from_root_to_this() << endl;
//                cout << new_operation->get_output()->string__of__path_from_root_to_this() << endl;
                if(is_new)
                {
                    delete new_operation;
                }
                skip = true;
//                assert(false);
            }
            else
            {
                operation = new_operation;
                unions[operands] = operation;
                pair<TraceNode*, TraceNode*> reverse_operands = make_pair(
                        new_operation->operands[1], new_operation->operands[0]);
                unions[reverse_operands] = operation;
            }
        }
        if(!skip)
        {
            TraceNode *output = operation->get_output();
            if(output == nullptr)
            {
                skip = true;
            }
            else
            {
                if(output->trace_state.num_missing_bits == 0)
                {
                    skip = true;
                }
            }
        }
        if(!skip) {
            TraceNode *output = operation->get_output();
            if (output != nullptr) {
                for (int i = 0; i < masks.size(); i++) {
                    fronteer.insert(
                            HeuristicScoreAndSolution(
                                    max(at.max_width, (int) masks[i].count()),
                                    at.sum_width + masks[i].count(),
                                    output->trace_state.num_missing_bits,
                                    output, i));
                    num_inserts++;
                    while (fronteer.size() > min(beam_size, remaining_while_loop_count)) {
                        auto fronteer_end = fronteer.end();
                        fronteer_end--;
                        fronteer.erase(fronteer_end);
                    }
                }
            } else {
                if (is_new) {
                    delete operation;
                }
            }
        }
    }

    vector<TraceNode*> leafs;
    memset_visited(vis_leafs);
    root->get_leafs(leafs);

    assert(leafs.size() >= 1);

    cout << "traces " << leafs.size() << endl;
    for(int i = 0;i<leafs.size(); i++)
    {
        cout << "i = " << i << " :: " <<leafs[i]->string__of__path_from_root_to_this() << endl;
    }

}