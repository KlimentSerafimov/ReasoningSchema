//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "CompactPoset.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>

using namespace std;

CompactPoset::CompactPoset(int _num_inputs)
{
    num_inputs = _num_inputs;
    generalization_mask = (1<<(1<<num_inputs))-1;
    input_mask = (1<<(1<<num_inputs))-1;
    push_back_new_node__and__get_id(CompactPosetNode(PartialFunction(num_inputs, 0, 0)));
}

CompactPoset::CompactPoset(int _num_inputs, int _generalization_mask, int _input_mask)
{
    num_inputs = _num_inputs;
    generalization_mask = _generalization_mask;
    input_mask = _input_mask;
    push_back_new_node__and__get_id(CompactPosetNode(PartialFunction(num_inputs, 0, 0)));
}

CompactPoset::CompactPoset(int _num_inputs, int _generalization_mask, int _input_mask, vector<MetaExample> &meta_examples)
{
    num_inputs = _num_inputs;
    generalization_mask = _generalization_mask;
    input_mask = _input_mask;

    assert(generalization_mask == input_mask);

    int subdomain_mask = input_mask;

    push_back_new_node__and__get_id(CompactPosetNode(PartialFunction(num_inputs, 0, 0)));

    vector<MetaExample> ret_meta_examples;
    for (int id = 0; id < meta_examples.size(); id++) {
        MetaExample local_insert_meta_example = meta_examples[id].get_application_of_subdomain(subdomain_mask);
        if (!local_insert_meta_example.fully_constrained()) {
            if(!insert(local_insert_meta_example))
            {
                hard_pop();
            }
            else
            {
                ret_meta_examples.push_back(meta_examples[id]);
            }
        }
        else
        {
            ret_meta_examples.push_back(meta_examples[id]);
        }
    }
    meta_examples = ret_meta_examples;
}

int CompactPoset::push_back_new_node__and__get_id(CompactPosetNode decision_tree) {
    int ret = (int) nodes.size();
    local_delta.new_nodes.push_back(ret);
    assert(!decision_tree.is_empty());
    nodes.push_back(decision_tree);
    nodes[ret].id_in_compact_poset = ret;
    is_union_of.push_back(vector<int>());
    is_contained_in.push_back(vector<int>());
    meta_edges.push_back(vector<int>());
    reverse_meta_edges.push_back(vector<int>());
    open_visited.push_back(-1);
    closed_visited.push_back(-1);
    return ret;
}

void CompactPoset::push_back_meta_edge(int dominator, int dominated, int meta_edge_id)
{
    meta_edges[dominator].push_back(dominated);
    reverse_meta_edges[dominated].push_back(dominator);
    edge_to_delta_stack_id[make_pair(dominator, dominated)] = meta_edge_id;
    num_meta_edges+=1;
}

void CompactPoset::re_insert_meta_edge(int meta_edge_id)
{
    int dominator = delta_stack[meta_edge_id].new_meta_edge.first;
    int dominated = delta_stack[meta_edge_id].new_meta_edge.second;
    meta_edges[dominator].insert(delta_stack[meta_edge_id].soft_delete_iterators.second, dominated);
    reverse_meta_edges[dominated].insert(delta_stack[meta_edge_id].soft_delete_iterators.first, dominator);
    edge_to_delta_stack_id[make_pair(dominator, dominated)] = meta_edge_id;
    num_meta_edges+=1;
}

void CompactPoset::pop_meta_edge(int dominator, int dominated)
{
    assert(dominated == meta_edges[dominator].back());
    assert(reverse_meta_edges[dominated].back() == dominator);
    meta_edges[dominator].pop_back();
    reverse_meta_edges[dominated].pop_back();
    edge_to_delta_stack_id.erase(make_pair(dominator, dominated));
    num_meta_edges -= 1;
}

pair<vector<int>::iterator, vector<int>::iterator> CompactPoset::erase_meta_edge(int dominator, int dominated)
{
    vector<int>::iterator dominated_it = find(meta_edges[dominator].begin(), meta_edges[dominator].end(), dominated);
    vector<int>::iterator dominator_it = find(reverse_meta_edges[dominated].begin(), reverse_meta_edges[dominated].end(), dominator);

    meta_edges[dominator].erase(dominated_it);
    reverse_meta_edges[dominated].erase(dominator_it);
    edge_to_delta_stack_id.erase(make_pair(dominator, dominated));
    num_meta_edges-=1;

    return make_pair(dominator_it, dominated_it);
}

void CompactPoset::create_union_edge(int container, int contained) {
    is_union_of[container].push_back(contained);
    is_contained_in[contained].push_back(container);
    local_delta.new_union_edges.push_back(make_pair(container, contained));
}

void CompactPoset::make_union_node(int new_union_node) {
    if(nodes[new_union_node].node_type == base_node)
    {
        local_delta.new_union_nodes.push_back(new_union_node);
        nodes[new_union_node].node_type = union_node;
    }
}


CompactPosetNode get_intersection(CompactPosetNode* first, CompactPosetNode* second)
{
    CompactPosetNode copy_first = CompactPosetNode(first->copy());
    copy_first.apply_operation(intersection, second);
    return copy_first;
}

bool is_intersection_empty(CompactPosetNode* first, CompactPosetNode* second)
{
    CompactPosetNode copy_first = CompactPosetNode(first->copy());
    copy_first.apply_operation(intersection, second);
    bool ret = copy_first.is_empty();
    copy_first.my_delete();
    return ret;
}

bool are_function_sets_the_same(DecisionTree* first, DecisionTree* second)
{
    CompactPosetNode copy_first = CompactPosetNode(first->copy());
    copy_first.apply_operation(difference, new CompactPosetNode(second));
    bool first_without_second_is_empty = copy_first.is_empty();
    copy_first.my_delete();

    CompactPosetNode copy_second = CompactPosetNode(second->copy());
    copy_second.apply_operation(difference, new CompactPosetNode(first));
    bool second_without_first_is_empty = copy_second.is_empty();
    copy_second.my_delete();

    return first_without_second_is_empty && second_without_first_is_empty;
}

CompactPosetNode get_difference(CompactPosetNode* first, CompactPosetNode* second)
{
    CompactPosetNode copy_first = CompactPosetNode(first->copy());
    copy_first.apply_operation(difference, second);
    return copy_first;
}

void CompactPoset::print_operation(OperationType operation_type, CompactPosetNode* first, CompactPosetNode* second)
{
    cout << "operation_name = " << operation_name[operation_type] << endl;
    cout << "first: " << endl;
    cout << first->get_string_of_union_of_partial_functions(1);
    cout << "second: " << endl;
    cout << second->get_string_of_union_of_partial_functions(1);
    CompactPosetNode copy_first = CompactPosetNode(first->copy());
    copy_first.apply_operation(operation_type, second);
    cout << "result: " << endl;
    cout << copy_first.get_string_of_union_of_partial_functions(1);
}

int CompactPoset::multi_split_and_union(vector<int> ids__base_node_cover, CompactPosetNode *target_set) {

    int id__ret = -1;

    CompactPosetNode remaining_target;
    if(ids__base_node_cover.size() >= 2)
    {
        id__ret = push_back_new_node__and__get_id(target_set->copy());
        make_union_node(id__ret);
        remaining_target = nodes[id__ret].copy();
    }
    else
    {
        assert(ids__base_node_cover.size() == 1);
        remaining_target = target_set->copy();
    }

    vector<int> to_union_target_parts;

    for(int i = 0;i<ids__base_node_cover.size();i++)
    {
        assert(!remaining_target.is_empty());
        int id__target_base = ids__base_node_cover[i];
        CompactPosetNode target_diff_base = get_difference(&remaining_target, &nodes[id__target_base]);
        CompactPosetNode base_diff_target = get_difference(&nodes[id__target_base], &remaining_target);

        bool base_diff_target__is_empty = base_diff_target.is_empty();
        bool target_diff_base__is_empty = target_diff_base.is_empty();

        if(base_diff_target__is_empty && target_diff_base__is_empty)
        {
            base_diff_target.my_delete();
            target_diff_base.my_delete();
            to_union_target_parts.push_back(id__target_base);
        }
        else if(base_diff_target__is_empty && !target_diff_base__is_empty)
        {
            base_diff_target.my_delete();
            target_diff_base.my_delete();
            //no need to create new node because base_intersection_target == base;
            to_union_target_parts.push_back(id__target_base);
        }
        else if(!base_diff_target__is_empty && target_diff_base__is_empty)
        {
            target_diff_base.my_delete();
            // base is bigger than target_set
            // create new node of remaining target_set
            CompactPosetNode base_intersection_target = get_intersection(&nodes[id__target_base], &remaining_target);
            int id__last_target = push_back_new_node__and__get_id(base_intersection_target);
            if(id__ret == -1) {
                id__ret = id__last_target;
            }
            nodes[id__last_target].applied_operations.push_back(
                    AppliedOperation(intersection, id__target_base, id__ret));
            int id__base_diff_target = push_back_new_node__and__get_id(base_diff_target);
            nodes[id__base_diff_target].applied_operations.push_back(
                    AppliedOperation(difference, id__target_base, id__last_target));

            create_union_edge(id__target_base, id__last_target);
            create_union_edge(id__target_base, id__base_diff_target);
            make_union_node(id__target_base);

            to_union_target_parts.push_back(id__last_target);
        }
        else if (!base_diff_target__is_empty && !target_diff_base__is_empty)
        {
            target_diff_base.my_delete();
            assert(id__ret != -1);
            CompactPosetNode base_intersection_target = get_intersection(&nodes[id__target_base], &remaining_target);

            //need to create two new nodes for base and union them under original
            int id__base_intersection_target = push_back_new_node__and__get_id(base_intersection_target);
            nodes[id__base_intersection_target].applied_operations.push_back(
                    AppliedOperation(intersection, id__target_base, id__ret));
            int id__base_diff_target = push_back_new_node__and__get_id(base_diff_target);
            nodes[id__base_diff_target].applied_operations.push_back(
                    AppliedOperation(difference, id__target_base, id__ret));

            //update id__dominator_base is_union_of
            create_union_edge(id__target_base, id__base_intersection_target);
            create_union_edge(id__target_base, id__base_diff_target);
            make_union_node(id__target_base);
            //undo split

            //need to add id__target_base to to_union_target_parts;
            to_union_target_parts.push_back(id__base_intersection_target);
        }
        else
        {
            assert(false);
        }
        //update remaining_target
        remaining_target.apply_operation(difference, &nodes[id__target_base]);
    }
    assert(to_union_target_parts.size() == ids__base_node_cover.size());
    assert(remaining_target.is_empty());

    if(ids__base_node_cover.size() >= 2)
    {
        for(int i = 0;i<to_union_target_parts.size();i++)
        {
            create_union_edge(id__ret, to_union_target_parts[i]);
        }
    }
    else{
        assert(to_union_target_parts.size() == 1);
        if(id__ret != -1) {
            assert(id__ret == to_union_target_parts[0]);
        } else{
            id__ret = to_union_target_parts[0];
        }
    }

    assert(id__ret != -1);

    remaining_target.my_delete();

    return id__ret;
}

vector<int> CompactPoset::get__set_cover(CompactPosetNode *target_set)
{
    vector<int> ret;
    for(int i = 0;i<nodes.size();i++)
    {
        if(nodes[i].node_type == base_node)
        {
            if(!is_intersection_empty(&nodes[i], target_set))
            {
                ret.push_back(i);
            }
        }
    }
    return ret;
}

bool CompactPoset::insert(MetaExample meta_example)
{
    assert(((generalization_mask | input_mask) & meta_example.generalization.partition) == meta_example.generalization.partition);
    assert((input_mask & meta_example.partial_function.partition) == meta_example.partial_function.partition);

    meta_example.partial_function.total_function &= meta_example.generalization.partition;
    meta_example.generalization.total_function &= meta_example.generalization.partition;

    pair<int, pair<int, int> > to_insert =
            make_pair(
                    meta_example.generalization.total_function,
                    make_pair(meta_example.partial_function.partition, meta_example.generalization.partition));

    if(uniques.find(to_insert) != uniques.end())
    {
        delta_stack[uniques[to_insert]].equivalent_meta_edge_ids.push_back(delta_stack.size());

        local_delta = DeltaCompactPoset();
        local_delta.not_unique = true;
        local_delta.meta_example = meta_example;
        delta_stack.push_back(local_delta);

        return true;
    }
    else
    {
        uniques[to_insert] = (int) delta_stack.size();
    }

    local_delta = DeltaCompactPoset();
    local_delta.meta_example = meta_example;
    local_delta.equivalent_meta_edge_ids.push_back(delta_stack.size());

    CompactPosetNode dominator = CompactPosetNode(meta_example.generalization);
    CompactPosetNode dominated = CompactPosetNode(meta_example.partial_function, difference, &dominator);

    vector<int> dominator_set_cover = get__set_cover(&dominator);

    int dominator_node = multi_split_and_union(dominator_set_cover, &dominator);

    dominator.my_delete();

    vector<int> dominated_set_cover = get__set_cover(&dominated);

    int dominated_node = multi_split_and_union(dominated_set_cover, &dominated);

    dominated.my_delete();

    push_back_meta_edge(dominator_node, dominated_node, (int) delta_stack.size());

    local_delta.new_meta_edge = make_pair(dominator_node, dominated_node);
    delta_stack.push_back(local_delta);

    bool is_meta_example_consistent = is_valid_node(dominator_node);

    return is_meta_example_consistent;
}

pair<vector<int>, vector<int>> CompactPoset::get_cycle_of_meta_example_ids_and_meta_edge_ids()
{
    int dominator_node = delta_stack.back().new_meta_edge.first;
    int max_chain = get_loop_from_node(dominator_node);
    assert(max_chain != -1);

    vector<int> meta_example_ids;
    for(int i = 0;i<meta_edges_ids_in_cycle.size();i++)
    {
        meta_example_ids.push_back(delta_stack[meta_edges_ids_in_cycle[i]].meta_example.idx);
    }

    return make_pair(meta_example_ids, meta_edges_ids_in_cycle);
}

void CompactPoset::disjoint_assertion()
{
    for(int i = 0;i<nodes.size();i++)
    {
        if(nodes[i].node_type == base_node) {
            for (int j = i + 1; j < nodes.size(); j++) {
                if (nodes[j].node_type == base_node)
                {
                    if(!is_intersection_empty(&nodes[i], &nodes[j]))
                    {
                        print_operation(intersection, &nodes[i], &nodes[j]);
                        assert(false);
                    }
                }
            }
        }
    }
}

int CompactPoset::get_loop(int at, int count) {
    open_visited[at] = visited_mark;
    int ret = -1;
    if(nodes[at].node_type == union_node)
    {
        for(auto subnode_id : is_union_of[at])
        {
            if (open_visited[subnode_id] == visited_mark) {
                cycle_init_id = subnode_id;
                min_cycles[subnode_id] = 0;
                return 0;
            } else if (closed_visited[subnode_id] != visited_mark) {
                int val = get_loop(subnode_id, count);
                if(val != -1)
                {
                    if(ret != -1)
                    {
                        ret = min(ret, val);
                    }
                    else
                    {
                        ret = val;
                    }
                    break;
                }
            }
            else
            {
                int val = min_cycles[subnode_id];
                if(val != -1)
                {
                    if(ret != -1)
                    {
                        ret = min(ret, val);
                    }
                    else
                    {
                        ret = val;
                    }
                    break;
                }
            }
        }
    }
    for (auto next_id : meta_edges[at]) {
        if (open_visited[next_id] == visited_mark) {
            min_cycles[next_id] = 1;
            cycle_init_id = next_id;
            meta_edges_ids_in_cycle.push_back(edge_to_delta_stack_id[make_pair(at, next_id)]);
            return 1;
        } else if (closed_visited[next_id] != visited_mark) {
            int val = get_loop(next_id, count);
            if(val != -1)
            {
                if(open_visited[cycle_init_id] == visited_mark)
                {
                    meta_edges_ids_in_cycle.push_back(edge_to_delta_stack_id[make_pair(at, next_id)]);
                }
                val+=1;
                if(ret != -1)
                {
                    ret = min(ret, val);
                }
                else
                {
                    ret = val;
                }
                break;
            }
        }
        else
        {
            int val = min_cycles[next_id];
            if(val != -1)
            {
                val+=1;
                if(ret != -1)
                {
                    ret = min(ret, val);
                }
                else
                {
                    ret = val;
                }
                break;
            }
        }
    }

    //traverse_meta_edges_of_union_parent;
    set<int> local_visited;
    queue<int> union_parents;
    for(auto parent: is_contained_in[at])
    {
        union_parents.push(parent);
        local_visited.insert(parent);
    }
    while(!union_parents.empty())
    {
        int at_parent = union_parents.front();
        union_parents.pop();
        bool do_break = false;
        for(auto next_id: meta_edges[at_parent])
        {
            if (open_visited[next_id] == visited_mark) {
                min_cycles[next_id] = 1;
                cycle_init_id = next_id;
                meta_edges_ids_in_cycle.push_back(edge_to_delta_stack_id[make_pair(at_parent, next_id)]);
                return 1;
            } else if (closed_visited[next_id] != visited_mark) {
                int val = get_loop(next_id, count);
                if(val != -1)
                {
                    if(open_visited[cycle_init_id] == visited_mark)
                    {
                        meta_edges_ids_in_cycle.push_back(edge_to_delta_stack_id[make_pair(at_parent, next_id)]);
                    }
                    val+=1;
                    if(ret != -1)
                    {
                        ret = min(ret, val);
                    }
                    else
                    {
                        ret = val;
                    }
                    do_break = true;
                    break;
                }
            }
            else
            {
                int val = min_cycles[next_id];
                if(val != -1)
                {
                    assert(open_visited[cycle_init_id] == -1);
                    val+=1;
                    if(ret != -1)
                    {
                        ret = min(ret, val);
                    }
                    else
                    {
                        ret = val;
                    }
                    do_break = true;
                    break;
                }
            }
        }
        if(do_break)
        {
            break;
        }
        for(auto parent_of_parent: is_contained_in[at_parent])
        {
            if(local_visited.find(parent_of_parent) == local_visited.end())
            {
                union_parents.push(parent_of_parent);
                local_visited.insert(parent_of_parent);
            }
        }
    }

    open_visited[at] = -1;
    closed_visited[at] = visited_mark;
    min_cycles[at] = ret;
    return ret;
}

int CompactPoset::get_loop_from_node(int origin) {
    visited_mark += 1;
    min_cycles = vector<int>(nodes.size(), -1);
    cycle_init_id = -1;
    meta_edges_ids_in_cycle.clear();
    return get_loop(origin, 0);
}

bool CompactPoset::visit(int at) {
    open_visited[at] = visited_mark;
    if(nodes[at].node_type == union_node)
    {
        for(auto subnode_id : is_union_of[at])
        {
            if (open_visited[subnode_id] == visited_mark) {
                return false;
            } else if (closed_visited[subnode_id] != visited_mark) {
                if (!visit(subnode_id)) {
                    return false;
                }
            }
        }
    }
    for (auto next_id : meta_edges[at]) {
        if (open_visited[next_id] == visited_mark) {
            return false;
        } else if (closed_visited[next_id] != visited_mark) {
            if (!visit(next_id)) {
                return false;
            }
        }
    }
    //traverse_meta_edges_of_union_parent;
    set<int> local_visited;
    queue<int> union_parents;
    for(auto parent: is_contained_in[at])
    {
        union_parents.push(parent);
        local_visited.insert(parent);
    }
    while(!union_parents.empty())
    {
        int at_parent = union_parents.front();
        union_parents.pop();
        for(auto next_id: meta_edges[at_parent])
        {
            if (open_visited[next_id] == visited_mark) {
                return false;
            } else if (closed_visited[next_id] != visited_mark) {
                if (!visit(next_id)) {
                    return false;
                }
            }
        }
        for(auto parent_of_parent: is_contained_in[at_parent])
        {
            if(local_visited.find(parent_of_parent) == local_visited.end())
            {
                union_parents.push(parent_of_parent);
                local_visited.insert(parent_of_parent);
            }
        }
    }

    open_visited[at] = -1;
    closed_visited[at] = visited_mark;
    return true;
}

bool CompactPoset::is_valid_node(int origin) {
    visited_mark += 1;
    return visit(origin);
}

bool CompactPoset::is_valid() {
    visited_mark += 1;
    for(int i = 0;i<nodes.size();i++)
    {
        if(closed_visited[i] != visited_mark)
        {
            if (!visit(i)) {
                return false;
            }
        }
    }
    return true;
}

string CompactPoset::meta_examples_to_string()
{
    string ret;
//    ret += "meta_example_stack: \n";
    for(int i = 0;i<delta_stack.size();i++)
    {

        ret += delta_stack[i].meta_example.linear_string(0);
        if(delta_stack[i].not_unique)
        {
            ret += "   not_unique";
        }
        if(delta_stack[i].removed_after_the_fact)
        {
            ret += "   redundant";
        }
        if(delta_stack[i].is_existant())
        {
            ret += "   necessary";
        }
        ret += "\n";
    }
    return ret;
}

string CompactPoset::to_string() {
    string ret;
    ret += "nodes:\n";
    for(int i = 0;i<nodes.size();i++)
    {
        ret += nodes[i].to_string(i, num_inputs);
    }

    ret += vector_of_vector_of_int_to_string(meta_edges, "meta_edges");
    ret += vector_of_vector_of_int_to_string(is_union_of, "is_union_of");

    ret += meta_examples_to_string();

    return ret;
}

void CompactPoset::erase_from_uniques(int id)
{


    MetaExample meta_example = delta_stack[id].meta_example;
    pair<int, pair<int, int> > to_erase =
            make_pair(
                    meta_example.generalization.total_function,
                    make_pair(meta_example.partial_function.partition, meta_example.generalization.partition));

    if(delta_stack[id].removed_from_uniques)
    {
        assert(uniques.find(to_erase) == uniques.end());
        return;
    }
    if(uniques.find(to_erase) == uniques.end())
    {
        assert(delta_stack[id].not_unique);
        return ;
    }

    if(uniques[to_erase] == id)
    {
        uniques.erase(to_erase);
    }
    else
    {
        delta_stack[uniques[to_erase]].equivalent_meta_edge_ids.erase(
                find(
                        delta_stack[uniques[to_erase]].equivalent_meta_edge_ids.begin(),
                        delta_stack[uniques[to_erase]].equivalent_meta_edge_ids.end(),
                        id));
    }
    delta_stack[id].removed_from_uniques = true;
}

void CompactPoset::hard_pop()
{
    int id = delta_stack.size()-1;
    DeltaCompactPoset delta = delta_stack.back();
    int dominator = delta.new_meta_edge.first;
    int dominated = delta.new_meta_edge.second;

    erase_from_uniques(id);

    if(delta.is_existant())
    {
        pop_meta_edge(dominator, dominated);
    }

    for (int i = 0; i < (int) delta.new_union_nodes.size(); i++) {
        int union_node = delta.new_union_nodes[i];
        nodes[union_node].node_type = base_node;
    }

    for (int i = (int) delta.new_union_edges.size() - 1; i >= 0; i--) {
        int container = delta.new_union_edges[i].first;
        int contained = delta.new_union_edges[i].second;

        assert(is_union_of[container].back() == contained);
        is_union_of[container].pop_back();
        if (is_union_of[container].empty()) {
            assert(nodes[container].node_type == base_node);
        }
        assert(is_contained_in[contained].back() == container);
        is_contained_in[contained].pop_back();
    }

    for (int i = 0; i < (int) delta.new_union_nodes.size(); i++) {
        int union_node = delta.new_union_nodes[i];
        assert(is_union_of[union_node].size() == 0);
    }
    for (int i = (int) delta.new_nodes.size() - 1; i >= 0; i--) {
        int new_node = delta.new_nodes[i];
        assert(new_node == nodes.size() - 1);
        assert(is_union_of[new_node].empty());
        assert(is_contained_in[new_node].empty());
        assert(meta_edges[new_node].empty());
        assert(reverse_meta_edges[new_node].empty());
        nodes[new_node].my_delete();
        nodes.pop_back();
        is_union_of.pop_back();
        is_contained_in.pop_back();
        meta_edges.pop_back();
        reverse_meta_edges.pop_back();
    }

    delta_stack.pop_back();
}


void CompactPoset::soft_pop() {
    int id_to_delete = delta_stack.size()-1;
    while(!delta_stack[id_to_delete].is_existant() && id_to_delete>=0)
    {
        id_to_delete--;
    }
    if(id_to_delete>=0)
    {
        soft_delete(id_to_delete);
    }
}


void CompactPoset::soft_delete(int id) {
    erase_from_uniques(id);
    if(!delta_stack[id].is_existant())
    {
        return;
    }
    int dominator = delta_stack[id].new_meta_edge.first;
    int dominated = delta_stack[id].new_meta_edge.second;
    delta_stack[id].soft_delete_iterators = erase_meta_edge(dominator, dominated);
    delta_stack[id].soft_deleted = true;
}

int CompactPoset::soft_delete_meta_example_with_idx(int meta_example_idx)
{
    int delta_stack_id = -1;
    for(int i = (int) delta_stack.size()-1;i>=0;i--)
    {
        if(delta_stack[i].meta_example.idx == meta_example_idx)
        {
            assert(delta_stack_id == -1);
            delta_stack_id = i;
        }
    }
    assert(delta_stack_id != -1);
    soft_delete(delta_stack_id);
    return delta_stack_id;
}


void CompactPoset::re_insert(int id) {
    assert(delta_stack[id].soft_deleted);

    MetaExample meta_example = delta_stack[id].meta_example;
    pair<int, pair<int, int> > to_re_insert =
            make_pair(
                    meta_example.generalization.total_function,
                    make_pair(meta_example.partial_function.partition, meta_example.generalization.partition));

    if(uniques.find(to_re_insert) == uniques.end())
    {
        assert(!delta_stack[id].not_unique);
        uniques[to_re_insert] = id;
    }
    else
    {
        assert(delta_stack[id].not_unique);
        delta_stack[uniques[to_re_insert]].equivalent_meta_edge_ids.push_back(id);
    }

    delta_stack[id].removed_from_uniques = false;

    re_insert_meta_edge(id);

    delta_stack[id].soft_deleted = false;
}

void CompactPoset::print()
{
    cout << to_string() <<endl;
}

int CompactPoset::get_num_nodes() {
    return (int) nodes.size();
}

void CompactPoset::mark_dominated_init(int origin)
{
    visited_mark += 1;
//    cout << "mark dominated = ";
    mark_dominated(origin, true);
//    cout << endl;
}

bool CompactPoset::there_are_new_dominated__init(int origin) {
    visited_mark += 1;

//    cout << "mark second dominated = ";
    bool ret =  there_are_new_dominated(origin, true);
//    cout << endl;
    return ret;
}

bool CompactPoset::there_are_new_dominated(int at, bool is_origin) {
    open_visited[at] = visited_mark;
    if(nodes[at].node_type == union_node && !is_origin)
    {
        for(auto subnode_id : is_union_of[at])
        {
            if (open_visited[subnode_id] == visited_mark) {
                assert(false);
            } else if (closed_visited[subnode_id] != visited_mark) {
//                cout << subnode_id <<" "<< closed_visited[subnode_id] <<" | ";
                if (there_are_new_dominated(subnode_id, false)) {
                    return true;
                }
            }
        }
    }
    for (auto next_id : meta_edges[at]) {
        if (open_visited[next_id] == visited_mark) {
            assert(false);
        } else if (closed_visited[next_id] != visited_mark) {
//            cout << next_id <<" "<< closed_visited[next_id] <<" | ";
            if(nodes[next_id].node_type == base_node && closed_visited[next_id] != visited_mark-1)
            {
                if(nodes[next_id].node_type == base_node)
                {
//                    cout << "dominated new: " << next_id << endl;
                }
                return true;
            }
            if (there_are_new_dominated(next_id, false)) {
                return true;
            }
        }
    }

    set<int> local_visited;
    queue<int> union_parents;
    for(auto parent: is_contained_in[at])
    {
        union_parents.push(parent);
        local_visited.insert(parent);
    }
    while(!union_parents.empty())
    {
        int at_parent = union_parents.front();
        union_parents.pop();
        for(auto next_id: meta_edges[at_parent])
        {
            if (open_visited[next_id] == visited_mark) {
                assert(false);
            } else if (closed_visited[next_id] != visited_mark) {
//                cout << next_id <<" " << closed_visited[next_id] <<" | ";
                if(nodes[next_id].node_type == base_node && closed_visited[next_id] != visited_mark-1)
                {
                    if(nodes[next_id].node_type == base_node)
                    {
//                    cout << "dominated new: " << next_id << endl;
                    }
                    return true;
                }
                if (there_are_new_dominated(next_id, false)) {
                    return true;
                }
            }
        }
        for(auto parent_of_parent: is_contained_in[at_parent])
        {
            if(local_visited.find(parent_of_parent) == local_visited.end())
            {
                union_parents.push(parent_of_parent);
                local_visited.insert(parent_of_parent);
            }
        }
    }


    open_visited[at] = -1;
    if(closed_visited[at] == visited_mark-1)
    {
        closed_visited[at] = visited_mark;
        if(nodes[at].node_type == base_node)
        {
//            cout << "dominated old: " << at << endl;
        }
        return false;
    }
    else
    {
        closed_visited[at] = visited_mark;
        if(nodes[at].node_type == base_node)
        {
//            cout << "dominated new: " << at << endl;
        }
        return nodes[at].node_type == base_node;
    }
}

void CompactPoset::mark_dominated(int at, bool is_origin) {
    open_visited[at] = visited_mark;
    if(nodes[at].node_type == union_node && !is_origin)
    {
        for(auto subnode_id : is_union_of[at])
        {
            if (open_visited[subnode_id] == visited_mark) {
                return ;
            } else if (closed_visited[subnode_id] != visited_mark) {

//                cout << subnode_id <<" "<<visited_mark <<" | ";
                nodes[subnode_id].dominated_mark = visited_mark;
                mark_dominated(subnode_id, false);
            }
        }
    }
    for (auto next_id : meta_edges[at]) {
        if (open_visited[next_id] == visited_mark) {
            return ;
        } else if (closed_visited[next_id] != visited_mark) {

//            cout << next_id <<" "<<visited_mark <<" | ";
            nodes[next_id].dominated_mark = visited_mark;
            mark_dominated(next_id, false);
        }
    }

    //traverse_meta_edges_of_union_parent;
    set<int> local_visited;
    queue<int> union_parents;
    for(auto parent: is_contained_in[at])
    {
        union_parents.push(parent);
        local_visited.insert(parent);
    }
    while(!union_parents.empty())
    {
        int at_parent = union_parents.front();
        union_parents.pop();
        for(auto next_id: meta_edges[at_parent])
        {
            if (open_visited[next_id] == visited_mark) {
                return ;
            } else if (closed_visited[next_id] != visited_mark) {

//                cout << next_id <<" "<<visited_mark <<" | ";
                nodes[next_id].dominated_mark = visited_mark;
                mark_dominated(next_id, false);
            }
        }
        for(auto parent_of_parent: is_contained_in[at_parent])
        {
            if(local_visited.find(parent_of_parent) == local_visited.end())
            {
                union_parents.push(parent_of_parent);
                local_visited.insert(parent_of_parent);
            }
        }
    }

    open_visited[at] = -1;
    closed_visited[at] = visited_mark;

//    if(nodes[at].node_type == base_node)
//    {
//        cout << "dominated: " << at << endl;
//    }
    return;
}

int CompactPoset::get_num_meta_edges() {
    return num_meta_edges;
}

int CompactPoset::get_num_inserts() {
    return (int) delta_stack.size();
}

bool CompactPoset::soft_delete_redundant_edges()
{
    if(removed_edges.size() >= 1)
    {
        return false;
    }

    bool removed_meta_edge = false;

    int count_necessary_meta_edges = 0;

    for(int i = delta_stack.size()-1;i>=0;i--)
    {
        if(delta_stack[i].is_existant())
        {
            int dominator = delta_stack[i].new_meta_edge.first;

            soft_delete(i);

            mark_dominated_init(dominator);

            re_insert(i);

            if (!there_are_new_dominated__init(dominator))
            {
                soft_delete(i);

                delta_stack[i].removed_after_the_fact = true;

                removed_edges.push_back(i);
            }
            else
            {
                count_necessary_meta_edges++;
            }

        }
    }

    assert(count_necessary_meta_edges == num_meta_edges);

    return removed_meta_edge;
}

CompactPoset::CompactPoset(CompactPoset *poset) {
    num_inputs = poset->num_inputs;
    push_back_new_node__and__get_id(CompactPosetNode(PartialFunction(num_inputs, 0, 0)));

    vector<MetaExample> meta_examples = poset->get_meta_examples();
    for(int i = 0; i < meta_examples.size();i++)
    {
        insert(meta_examples[i]);
    }
}

int CompactPoset::get_num_inputs() {
    return num_inputs;
}

vector<MetaExample> CompactPoset::get_meta_examples() {
    vector<MetaExample> ret;
    for(int i = 0; i < delta_stack.size();i++)
    {
        ret.push_back(delta_stack[i].meta_example);
    }
    return ret;
}

vector<int> CompactPoset::get_existant_meta_example_ids()
{
    vector<int> ret;
    for(int i = 0; i < delta_stack.size();i++)
    {
        if(delta_stack[i].is_existant())
        {
            ret.push_back(delta_stack[i].meta_example.idx);
        }
    }
    return ret;
}

void CompactPoset::get_existant_meta_examples_and_equivalent_ids(
        vector<MetaExample> &ret_meta_examples, vector<vector<int> > &ret_equivalent_ids)
{
    assert(ret_meta_examples.size() == 0 && ret_equivalent_ids.size() == 0);

//    cout << "source:" << endl;
    for(int i = 0; i < delta_stack.size();i++)
    {
        if(delta_stack[i].is_existant())
        {
            ret_meta_examples.push_back(delta_stack[i].meta_example);
            ret_equivalent_ids.push_back(vector<int>());
            for(int j = 0; j<delta_stack[i].equivalent_meta_edge_ids.size();j++)
            {
                int meta_edge_id = delta_stack[i].equivalent_meta_edge_ids[j];
                ret_equivalent_ids[ret_equivalent_ids.size()-1].push_back(delta_stack[meta_edge_id].meta_example.idx);
//                cout << delta_stack[meta_edge_id].meta_example.idx <<" ";
            }
//            cout << endl;
        }
    }
}


bool CompactPoset::empty()
{
    return delta_stack.empty();
}

void CompactPoset::add_edges_back() {
    for(int i = (int) removed_edges.size()-1;i>=0;i--)
    {
        re_insert(removed_edges[i]);
    }
    removed_edges.clear();
}

void CompactPoset::clear()
{
    while(!empty())
    {
        hard_pop();
    }
    assert(nodes.size() == 1);
    nodes[0].my_delete();
}

DecisionTree* CompactPoset::get_downstream_union(int at)
{
    assert(open_visited[at] != visited_mark);
    open_visited[at] = visited_mark;
    if(closed_visited[at] != visited_mark) {
        if (nodes[at].node_type == union_node) {
            assert(is_union_of[at].size() >= 1);
            nodes[at].downstream_union = get_downstream_union(is_union_of[at][0])->copy();
            nodes[at].downstream_union.apply_operation(my_union, &nodes[is_union_of[at][0]]);
            for (int i = 1; i < is_union_of[at].size(); i++) {
                nodes[at].downstream_union.apply_operation(my_union, get_downstream_union(is_union_of[at][i]));
                nodes[at].downstream_union.apply_operation(my_union, &nodes[is_union_of[at][i]]);
            }
        } else if (meta_edges[at].size() >= 1)
        {
            nodes[at].downstream_union = get_downstream_union(meta_edges[at][0])->copy();
            nodes[at].downstream_union.apply_operation(my_union, &nodes[meta_edges[at][0]]);
            for (int i = 1; i < meta_edges[at].size(); i++) {
                nodes[at].downstream_union.apply_operation(my_union, get_downstream_union(meta_edges[at][i]));
                nodes[at].downstream_union.apply_operation(my_union, &nodes[meta_edges[at][i]]);
            }
        }
        else if (nodes[at].node_type == base_node)
        {
            assert(meta_edges[at].size() == 0);
            Node* empty_node = get_new_node();
            empty_node->node_type = leaf_node;
            empty_node->value = false;
            nodes[at].downstream_union = DecisionTree(0, empty_node);
        }
    }

    closed_visited[at] = visited_mark;
    open_visited[at] = -1;
//    cout << "--" << endl;
//    cout << "node:" << endl;
//    cout << nodes[at].to_string(at, num_inputs) <<endl;
//    cout << "downstream_union:" <<endl;
//    cout << nodes[at].downstream_union.get_string_of_union_of_partial_functions(1, num_inputs) << endl;
//    cout << "--" << endl;
    return &nodes[at].downstream_union;
}

void CompactPoset::calc_downstream_unions()
{
    visited_mark+=1;
    for(int i = 0;i<nodes.size();i++)
    {
        nodes[i].downstream_union.my_delete();
    }
    for(int i = 0;i<nodes.size();i++)
    {
        if(closed_visited[i] != visited_mark && nodes[i].node_type != inactive_node) {
            get_downstream_union(i);
        }
    }
}

DecisionTree* CompactPoset::get_dominator_union(int at)
{
    assert(open_visited[at] != visited_mark);
    open_visited[at] = visited_mark;
    if(closed_visited[at] != visited_mark) {

        Node* empty_node = get_new_node();
        empty_node->node_type = leaf_node;
        empty_node->value = false;
        nodes[at].dominator_union = DecisionTree(0, empty_node);

        for(int i = 0; i< reverse_meta_edges[at].size();i++)
        {
            nodes[at].dominator_union.apply_operation(my_union, &nodes[reverse_meta_edges[at][i]]);
            nodes[at].dominator_union.apply_operation(my_union, get_dominator_union(reverse_meta_edges[at][i]));
        }

        for(int i = 0; i< is_contained_in[at].size();i++)
        {
            nodes[at].dominator_union.apply_operation(my_union, get_dominator_union(is_contained_in[at][i]));
        }

//        cout << "--" << endl;
//        cout << "node:" << endl;
//        cout << nodes[at].to_string(at, num_inputs) <<endl;
//        cout << "dominator_union:" <<endl;
//        cout << nodes[at].dominator_union.get_string_of_union_of_partial_functions(1, num_inputs) << endl;
//        cout << "--" << endl;

    }

    closed_visited[at] = visited_mark;
    open_visited[at] = -1;
    return &nodes[at].dominator_union;
}

void CompactPoset::calc_dominator_unions()
{
    visited_mark+=1;
    for(int i = 0;i<nodes.size();i++)
    {
        nodes[i].dominator_union.my_delete();
    }
    for(int i = 0;i<nodes.size();i++)
    {
        if(closed_visited[i] != visited_mark) {
            get_dominator_union(i);
        }
    }
}

vector<PartialFunction> CompactPoset::query(PartialFunction partial_function)
{
//    cout << endl;
//    cout << partial_function.to_string() << endl;
    queue<int> q;
    int local_visited_mark = ++visited_mark;
    for(int i = 0;i<nodes.size();i++)
    {
        if(nodes[i].node_type != inactive_node && reverse_meta_edges[i].size() == 0 && is_contained_in[i].size() == 0)
        {
            nodes[i].open_visited_mark = local_visited_mark;
            nodes[i].num_incoming_meta_edges = 0;
            nodes[i].num_incoming_union_edges = 0;
            assert(nodes[i].num_incoming_meta_edges == reverse_meta_edges[i].size());
            assert(nodes[i].num_incoming_union_edges == is_contained_in[i].size());

//            if (!nodes[i].dominator_union.contains(partial_function))
            {
                nodes[i].closed_visited_mark = local_visited_mark;
                q.push(i);
            }
        }
    }

    int init_dominated_mark = visited_mark;

    vector<int> candidate_ids;
    while(!q.empty())
    {
        num_visited_nodes_during_query++;
        int at = q.front();
        q.pop();

        if(init_dominated_mark <= nodes[at].dominated_mark && nodes[at].dominated_mark <= visited_mark)
        {
//            assert(false);
            continue;
        }

//        cout <<"at " << at << endl;
        if(nodes[at].node_type == base_node)
        {
            assert(is_union_of[at].size() == 0);
            if(nodes[at].contains(partial_function))
            {
//                if(nodes[at].dominator_union.contains(partial_function))
//                {
//                    cout << "here at" << at << endl;
//                    cout << "init_dominated_mark " << init_dominated_mark <<endl;
//                    cout << "visited_mark " << visited_mark << endl;
//                    cout << "nodes[at].dominated_mark " << nodes[at].dominated_mark << endl;
////                    assert(false);
//                }
                assert(nodes[at].num_incoming_meta_edges == reverse_meta_edges[at].size());
                assert(nodes[at].num_incoming_union_edges == is_contained_in[at].size());

                mark_dominated_init(at);

                candidate_ids.push_back(at);

//                if(ret.size() >= 6)
//                {
//                    for(int i = 0;i<ret.size();i++) {
//                        cout << ret[i].to_string() << endl;
//                    }
//                    cout << nodes[at].get_string_of_union_of_partial_functions(0, num_inputs) << endl;
//                    assert(false);
//                }
//                while(!q.empty()){
//                    q.hard_pop();
//                }
            }
            else if(meta_edges[at].size() >= 1)
            {
//                if(nodes[at].downstream_union.contains(partial_function))
                {
                    for(int i = 0;i<meta_edges[at].size();i++){
                        int next = meta_edges[at][i];
                        if(nodes[next].closed_visited_mark != local_visited_mark) {
                            if(nodes[next].open_visited_mark != local_visited_mark)
                            {
                                nodes[next].num_incoming_meta_edges = 0;
                                nodes[next].num_incoming_union_edges = 0;
                                nodes[next].open_visited_mark = local_visited_mark;
                            }
                            nodes[next].num_incoming_meta_edges+=1;
                            if(nodes[next].num_incoming_meta_edges == reverse_meta_edges[next].size()
                               && nodes[next].num_incoming_union_edges == is_contained_in[next].size())
                            {
                                if(init_dominated_mark <= nodes[at].dominated_mark && nodes[at].dominated_mark <= visited_mark)
                                {
                                    assert(false);
                                    continue;
                                }
//                                if (!nodes[next].dominator_union.contains(partial_function))
                                {
                                    nodes[next].closed_visited_mark = local_visited_mark;
                                    q.push(next);
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(nodes[at].node_type == union_node)
        {
//            if(nodes[at].downstream_union.contains(partial_function))
            {
                for(int i = 0;i<is_union_of[at].size();i++)
                {
                    int next = is_union_of[at][i];
                    if(nodes[next].closed_visited_mark != local_visited_mark) {
                        if(nodes[next].open_visited_mark != local_visited_mark)
                        {
                            nodes[next].num_incoming_meta_edges = 0;
                            nodes[next].num_incoming_union_edges = 0;
                            nodes[next].open_visited_mark = local_visited_mark;
                        }
                        nodes[next].num_incoming_union_edges+=1;
                        if(nodes[next].num_incoming_meta_edges == reverse_meta_edges[next].size()
                            && nodes[next].num_incoming_union_edges == is_contained_in[next].size())
                        {
                            if(init_dominated_mark <= nodes[at].dominated_mark && nodes[at].dominated_mark <= visited_mark)
                            {
                                assert(false);
                                continue;
                            }
//                            if (!nodes[next].dominator_union.contains(partial_function))
                            {
                                nodes[next].closed_visited_mark = local_visited_mark;
                                q.push(next);
                            }
                        }
                    }
                }
                for(int i = 0;i<meta_edges[at].size();i++){
                    int next = meta_edges[at][i];
                    if(nodes[next].closed_visited_mark != local_visited_mark) {
                        if(nodes[next].open_visited_mark != local_visited_mark)
                        {
                            nodes[next].num_incoming_meta_edges = 0;
                            nodes[next].num_incoming_union_edges = 0;
                            nodes[next].open_visited_mark = local_visited_mark;
                        }
                        nodes[next].num_incoming_meta_edges+=1;
                        if(nodes[next].num_incoming_meta_edges == reverse_meta_edges[next].size()
                           && nodes[next].num_incoming_union_edges == is_contained_in[next].size())
                        {
                            if(init_dominated_mark <= nodes[at].dominated_mark && nodes[at].dominated_mark <= visited_mark)
                            {
                                assert(false);
                                continue;
                            }
//                                if (!nodes[next].dominator_union.contains(partial_function))
                            {
                                nodes[next].closed_visited_mark = local_visited_mark;
                                q.push(next);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            assert(false);
        }
    }

    vector<PartialFunction> ret;
//    cout << endl;
//    cout << "candidates" << endl;

//    cout << "RESULT IDS  = ";
    for(int i = 0;i<candidate_ids.size();i++)
    {
        int at = candidate_ids[i];

//        cout << "at = " << at << endl;
//        cout << "init_dominated_mark " << init_dominated_mark <<endl;
//        cout << "visited_mark " << visited_mark << endl;
//        cout << "nodes[at].dominated_mark " << nodes[at].dominated_mark << endl;

        if(init_dominated_mark <= nodes[at].dominated_mark && nodes[at].dominated_mark <= visited_mark)
        {
//            assert(false);
            continue;
        }
//        cout << at <<" ";

        nodes[at].append_union_of_partial_functions_that_contain_partial_function(num_inputs, partial_function, &ret);
    }
//    cout << endl;

//    for(int i = 0;i<candidate_ids.size();i++)
//    {
//        int at = candidate_ids[i];
//
//        if(nodes[at].dominator_union.contains(partial_function))
//        {
//            for(int j = 0;j<nodes.size();j++)
//            {
//                if(nodes[j].node_type == base_node)
//                {
//                    if(nodes[j].contains(partial_function))
//                    {
//                        cout << "custom search" << endl;
//                        cout << "j = " << j << endl;
//                        cout << "init_dominated_mark " << init_dominated_mark <<endl;
//                        cout << "visited_mark " << visited_mark << endl;
//                        cout << "nodes[at].dominated_mark " << nodes[j].dominated_mark << endl;
//                        cout << "nodes[j].dominator_union.contains(partial_function) = " << nodes[j].dominator_union.contains(partial_function) << endl;
//                    }
//                }
//            }
//
//            for(int j = 0;j<nodes.size();j++)
//            {
//                cout << "j = " << j << " dominator_union " << endl;
//                cout<<nodes[j].dominator_union.get_string_of_union_of_partial_functions(0, num_inputs) << endl;
//            }
//
//            print();
//
//            assert(false);
//        }
//    }

    set<pair<int, int> > unique_ret;
    for(int i = 0;i<ret.size();i++)
    {
        ret[i].partition &= generalization_mask;
        ret[i].partition |= partial_function.partition;
        ret[i].total_function &= ret[i].partition;
        unique_ret.insert(make_pair(ret[i].total_function, ret[i].partition));
    }

    ret.clear();

    for(auto element : unique_ret)
    {
        ret.push_back(PartialFunction(num_inputs, element.first, element.second));
    }

    assert(ret.size()>=1);
    return ret;
}

void CompactPoset::compare_nodes()
{
    calc_downstream_unions();
    calc_dominator_unions();

    for(int i = 0;i<nodes.size();i++)
    {
        if(nodes[i].node_type == base_node)
        {
            vector<int> to_unite;
            to_unite.push_back(i);
            for (int j = i+1; j < nodes.size(); j++)
            {
                if(nodes[j].node_type == base_node)
                {
                    bool is_domianted_same = are_function_sets_the_same(&nodes[i].downstream_union, &nodes[j].downstream_union);
                    bool is_dominator_same = are_function_sets_the_same(&nodes[i].dominator_union, &nodes[j].dominator_union);

                    if(is_domianted_same && is_dominator_same)
                    {
                        cout << i << " " << j << endl;
                        to_unite.push_back(j);
                    }
                }
            }

            if(to_unite.size() >= 2)
            {
                cout << "dominator:" << endl;
                for(int j = 0;j<to_unite.size();j++)
                {
                    cout << nodes[to_unite[j]].dominator_union.get_string_of_union_of_partial_functions(1) << endl;
                }
                cout << "dominated:" << endl;
                for(int j = 0;j<to_unite.size();j++)
                {
                    cout << nodes[to_unite[j]].downstream_union.get_string_of_union_of_partial_functions(1) << endl;
                }

//                CompactPosetNode new_base_node;
//                for(int j = 0;j<to_unite.size();j++)
//                {
//                    new_base_node.apply_operation(my_union, &nodes[to_unite[j]]);
//                    nodes[to_unite[j]].my_delete();
//                }
//                int id__new_base_node = push_back_new_node__and__get_id(new_base_node);

            }
        }
    }
}

vector<MetaExample> CompactPoset::get_existant_meta_examples() {
    vector<MetaExample> ret;

    for(int i = 0;i<delta_stack.size();i++)
    {
        if(delta_stack[i].is_existant())
        {
            ret.push_back(delta_stack[i].meta_example);
        }
    }
    return ret;
}

vector<MetaExample> CompactPoset::get_all_meta_examples_without_duplicates() {
    vector<MetaExample> ret;
    for(auto it = uniques.begin(); it!=uniques.end(); it++)
    {
        pair<int, pair<int, int> > element = (*it).first;
        ret.push_back(
                MetaExample(num_inputs, element.first, element.second.first, element.second.second)
        );
    }
    return ret;
}


