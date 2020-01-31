//
// Created by Kliment Serafimov on 1/19/20.
//

#ifndef COMPACTPOSET_TRACEVERSIONSPACE_H
#define COMPACTPOSET_TRACEVERSIONSPACE_H

#include <vector>
#include "MetaExample.h"
#include "BittreeTaskType.h"
#include "CompactPoset.h"
#include <queue>

using namespace std;

class TaskType: public BittreeTaskType
{

};

class TraceState
{
public:
    vector<MetaExample> meta_examples;
    int num_missing_bits;
    TraceState() = default;
    TraceState(vector<MetaExample> _meta_examples)
    {
        meta_examples = _meta_examples;
        num_missing_bits = get_num_missing_bits(meta_examples);
    }
};

class SubdomainMask
{
//    TaskType task_type;
//    TaskType task_mask;
};

class TraceOperation;

class TraceNode
{
public:
    vector<TraceOperation*> is_result_from;
    TraceState trace_state;
    vector<TraceOperation*> is_operand_for;

    TraceNode(vector<MetaExample> meta_examples)
    {
        is_result_from.push_back(NULL);
        init(meta_examples);
    }

    TraceNode(TraceOperation* parent, vector<MetaExample> meta_examples)
    {
        is_result_from.push_back(parent);
        init(meta_examples);
    }

    void init(vector<MetaExample> meta_examples)
    {
        trace_state = TraceState(meta_examples);
    }

    void get_leafs(vector<TraceNode *>& ret_leafs);

    string string__of__root_to_this_path();
};

enum TraceOperationType{
    //init_trace_operation, end_trace_operation,
    compact_poset_operation, trace_union_operation, intersection_operation};

class TraceOperation
{
    TraceOperationType operation_type;

    //if compact_poset
    CompactPoset* compact_poset = NULL;


    void add_operand(TraceNode* operand);

    bool has_empty_output = false;
    TraceNode* output = NULL;

public:
    Bitvector subdomain_mask;

    vector<TraceNode*> operands;


    TraceOperation(TraceOperationType type, TraceNode *operand, Bitvector subdomain_mask);

    TraceNode* get_output();
};

class TraceVersionSpace
{
public:
    TraceNode* root;
    vector<TraceNode*> all_nodes;

    TraceVersionSpace(vector<MetaExample> _meta_examples, vector<Bitvector> masks)
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

        priority_queue<pair<int, pair<TraceNode*, int>>, vector<pair<int, pair<TraceNode*, int>>>, greater<pair<int, pair<TraceNode*, int>> > > fronteer;

        for(int i = 0;i<masks.size();i++) {
            fronteer.push(make_pair(masks[i].count(), make_pair(root, i)));
        }

        int num_iter = 0;

        int in_q_prev_max_widht = 0;

        int while_loop_count = 0;

        while(!fronteer.empty())
        {
            while_loop_count++;
            TraceNode* at = fronteer.top().second.first;
            int mask_id = fronteer.top().second.second;
            int prev_max_width = fronteer.top().first;
            fronteer.pop();
            if(in_q_prev_max_widht != prev_max_width)
            {
                cout << "prev_max_width " << prev_max_width << " @ |q| = " << num_iter << " |while| = " << while_loop_count << endl;
                in_q_prev_max_widht = prev_max_width;
            }

            TraceOperation* operation = new TraceOperation(compact_poset_operation, at, masks[mask_id]);
            TraceNode* output = operation->get_output();
            if(output != NULL) {
                for (int i = 0; i < masks.size(); i++) {
                    if (num_iter < 10000) {
                        fronteer.push(make_pair(max(prev_max_width, (int) masks[i].count()), make_pair(output, i)));
                        num_iter++;
                    }
                }
            }
            else {
                delete operation;
            }
        }

        vector<TraceNode*> leafs;
        root->get_leafs(leafs);

        assert(leafs.size() >= 1);

        cout << "traces" << endl;
        for(int i = 0;i<leafs.size(); i++)
        {
            cout << leafs[i]->string__of__root_to_this_path() << endl;
        }

    }


//    void main() {
//
//        calc_function_size();
//
//        local_time = time(nullptr);
//
//        if (parent_pointer == nullptr) {
//            init_time = local_time;
//            root_pointer = this;
//            module_id = 0;
//        } else {
//            init_time = parent_pointer->init_time;
//            root_pointer = parent_pointer->root_pointer;
//            module_id = parent_pointer->module_id + 1;
//        }
//
//        calc_masks(set_init_mask_size);
//
//        init_num_missing_bits = get_num_missing_bits(meta_examples);
//
//        if(init_num_missing_bits >= 1) {
//
//            bool possible_candidate_found = false;
//
//            for (int mask_id = 0; mask_id < masks.size(); mask_id++)
//            {
//                HeuristicScore heuristic;
//
//                cout << "working on mask = " << bitvector_to_str(masks[mask_id], function_size) << " time: "
//                     << (double) time(nullptr) - local_time << " ";
//
//                if(skip_mask(masks[mask_id]))
//                {
//                    cout << "skip" << endl;
//                    assert(parent_pointer != nullptr);
//                    heuristic = parent_pointer->heuristic_score_by_mask_id[mask_id];
//                }
//                else
//                {
//                    cout << "calc" << endl;
//                    Module local_module = Module(this);
//
//                    calc_module(masks[mask_id], &local_module);
//
//                    local_module.compact_poset->clear();
//
//                    heuristic = local_module.heuristic_score = calculate_heuristic(&local_module);
//                }
//
//                if (heuristic.defined) {
//                    possible_candidate_found = true;
//                }
//
//                mask_ids_by_heuristic.push_back(make_pair(heuristic, mask_id));
//                heuristic_score_by_mask_id.push_back(heuristic);
//
////            cout << "heuristic = " << fixed << setprecision(4) << heuristic.to_string() << endl;
//            }
//
//            sort(mask_ids_by_heuristic.begin(), mask_ids_by_heuristic.end());
//
//            cout << "PRINT mask_ids_by_heuristic" << endl;
//            for (int local_id = 0; local_id < mask_ids_by_heuristic.size(); local_id++) {
//                int mask_id = mask_ids_by_heuristic[local_id].second;
//                assert(masks[mask_id].get_size() == function_size);
//                cout << bitvector_to_str(masks[mask_id], function_size) << fixed << setprecision(6)
//                     << " heuristic_measure = " << mask_ids_by_heuristic[local_id].first.to_string() << endl;
//                if(!mask_ids_by_heuristic[local_id].first.defined)
//                {
//                    cout << "..." << endl;
//                    break;
//                }
//            }
//
//            assert(mask_ids_by_heuristic.size() >= 1);
//
//            HeuristicScore max_num_consistent_meta_examples = mask_ids_by_heuristic[0].first;
//
//            if (max_num_consistent_meta_examples.defined) {
//
//                if (parent_pointer == nullptr) {
//                    fout << "missing_bits " << init_num_missing_bits << endl << std::flush;
//                }
//
//                best_module = Module(this);
//
//                int best_mask_id = mask_ids_by_heuristic[0].second;
//
//                calc_module(masks[best_mask_id], (&best_module));
//
//                best_module.heuristic_score = calculate_heuristic((&best_module));
//
//                fout << best_module.print_module_sketch(time(nullptr) - init_time) << std::flush;
//
//                next = new MinimalFactoringSchema(best_module.meta_examples_after_query, this);
//
//                if(parent_pointer == nullptr)
//                {
//
//                    MinimalFactoringSchema* last = this;
//                    while(last->next != nullptr)
//                    {
//                        last = last->next;
//                    }
//
//                    fout << last->parent_pointer->best_module.covered_to_string(root_pointer->meta_examples) << endl;
//
//                    vector<pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > > all_traces;
//                    for(int i = 0;i<meta_examples.size();i++) {
//                        pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace =
//                                record_trace_of_query(
//                                        meta_examples[i].partial_function);
//                        assert(trace.second.first.size() + 1 == trace.second.second.size());
//                        all_traces.push_back(trace);
//                    }
//                    for(int i = 0;i<all_traces.size();i++)
//                    {
//                        pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
//                        assert(trace.second.first.size()+1 == trace.second.second.size());
//                        fout << "all    ";
//                        for(int j = 0;j<trace.first.size(); j++)
//                        {
//                            fout << trace.first[j].to_string() <<" ";
//                        }
//                        fout << endl;
//                    }
//                    fout << endl;
//                    for(int i = 0;i<all_traces.size();i++)
//                    {
//                        pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
//                        assert(trace.second.first.size()+1 == trace.second.second.size());
//                        fout << "active ";
//                        for(int j = 0;j<trace.second.second.size(); j++)
//                        {
//                            fout << trace.second.second[j].to_string() << " " ;
//                        }
//                        fout << endl;
//                    }
//                    fout << endl;
//                    for(int i = 0;i<all_traces.size();i++)
//                    {
//                        pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
//                        assert(trace.second.first.size()+1 == trace.second.second.size());
//                        fout << "masks  ";
//                        for(int j = 0;j<trace.second.first.size(); j++)
//                        {
//                            fout << std::to_string(trace.second.first[j]) << " " ;
//                        }
//                        fout << endl;
//                    }
//                    fout << endl;
//                    for(int i = 0;i<all_traces.size();i++)
//                    {
//                        pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
//                        assert(trace.second.first.size()+1 == trace.second.second.size());
//                        fout << "all    ";
//                        for(int j = 0;j<trace.first.size(); j++)
//                        {
//                            fout << trace.first[j].to_string() <<" ";
//                        }
//                        fout << endl;
//                        fout << "active ";
//                        for(int j = 0;j<trace.second.second.size(); j++)
//                        {
//                            fout << trace.second.second[j].to_string() << " " ;
//                        }
//                        fout << endl;
//                        fout << "masks  ";
//                        for(int j = 0;j<trace.second.first.size(); j++)
//                        {
//                            fout << std::to_string(trace.second.first[j]) << " " ;
//                        }
//                        fout << endl;
//                    }
//                    fout << endl;
//                    get_necessary_meta_examples(true);
//                }
//            } else
//            {
//                assert(false);
//            }
//        }
//    }


};



#endif //COMPACTPOSET_TRACEVERSIONSPACE_H
