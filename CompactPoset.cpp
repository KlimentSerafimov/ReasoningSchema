//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "CompactPoset.h"
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

int CompactPoset::push_back_new_node__and__get_id(CompactPosetNode decision_tree, vector<int> to_union_nodes) {
    int ret = (int) nodes.size();
    local_delta.new_nodes.push_back(ret);
    nodes.push_back(decision_tree);
    nodes[ret].id_in_compact_poset = ret;
    is_union_of.push_back(vector<int>());
    is_contained_in.push_back(vector<int>());
    for(int i = 0;i<to_union_nodes.size();i++)
    {
        create_union_edge(ret, to_union_nodes[i]);
    }
    meta_edges.push_back(vector<int>());
    reverse_meta_edges.push_back(vector<int>());
    open_visited.push_back(-1);
    closed_visited.push_back(-1);
    return ret;
}

int CompactPoset::push_back_new_node__and__get_id(CompactPosetNode decision_tree) {
    return push_back_new_node__and__get_id(decision_tree, vector<int>());
}

CompactPoset::CompactPoset(int _num_inputs)
{
    num_inputs = _num_inputs;
    push_back_new_node__and__get_id(CompactPosetNode(PartialFunction(num_inputs, 0, 0)));
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
    cout << first->get_string_of_union_of_partial_functions(1, num_inputs);
    cout << "second: " << endl;
    cout << second->get_string_of_union_of_partial_functions(1, num_inputs);
    CompactPosetNode copy_first = CompactPosetNode(first->copy());
    copy_first.apply_operation(operation_type, second);
    cout << "result: " << endl;
    cout << copy_first.get_string_of_union_of_partial_functions(1, num_inputs);
}


int CompactPoset::split_dominated_base(
        CompactPosetNode *dominated, int id__dominator_base, vector<int> ids__dominated_base) {

    int id__dominated = -1;

    CompactPosetNode remaining_dominated;
    if(ids__dominated_base.size() >= 2)
    {
        id__dominated = push_back_new_node__and__get_id(dominated->copy());
        make_union_node(id__dominated);
        remaining_dominated = nodes[id__dominated].copy();
    }
    else
    {
        remaining_dominated = dominated->copy();
    }

    vector<int> to_union_dominated_parts;

    for(int i = 0;i<ids__dominated_base.size();i++)
    {
        assert(!remaining_dominated.is_empty());
        int id__dominated_base = ids__dominated_base[i];
        CompactPosetNode base_diff_dominated = get_difference(&nodes[id__dominated_base], &remaining_dominated);
        CompactPosetNode dominated_diff_base = get_difference(&remaining_dominated, &nodes[id__dominated_base]);

        if(base_diff_dominated.is_empty() && dominated_diff_base.is_empty())
        {
            base_diff_dominated.my_delete();
            dominated_diff_base.my_delete();
            to_union_dominated_parts.push_back(id__dominated_base);
        }
        else if(base_diff_dominated.is_empty() && !dominated_diff_base.is_empty())
        {
            base_diff_dominated.my_delete();
            dominated_diff_base.my_delete();
            //no need to create new node because base_intersection_dominated == base;
            to_union_dominated_parts.push_back(id__dominated_base);
        }
        else if(!base_diff_dominated.is_empty() && dominated_diff_base.is_empty())
        {
            dominated_diff_base.my_delete();
            // base is bigger than dominated
            // create new node of remaining dominated
            CompactPosetNode base_intersection_dominated = get_intersection(&nodes[id__dominated_base], &remaining_dominated);
            int id__last_dominated = push_back_new_node__and__get_id(base_intersection_dominated);
            if(id__dominated == -1) {
                id__dominated = id__last_dominated;
            }
            nodes[id__last_dominated].applied_operations.push_back(
                    AppliedOperation(intersection, id__dominated_base, id__dominated));
            int id__base_diff_dominated = push_back_new_node__and__get_id(base_diff_dominated);
            nodes[id__base_diff_dominated].applied_operations.push_back(
                    AppliedOperation(difference, id__dominated_base, id__last_dominated));

            create_union_edge(id__dominated_base, id__last_dominated);
            create_union_edge(id__dominated_base, id__base_diff_dominated);
            make_union_node(id__dominated_base);

            to_union_dominated_parts.push_back(id__last_dominated);
        }
        else if (!base_diff_dominated.is_empty() && !dominated_diff_base.is_empty())
        {
            dominated_diff_base.my_delete();
            assert(id__dominated != -1);
            CompactPosetNode base_intersection_dominated = get_intersection(&nodes[id__dominated_base], &remaining_dominated);

            //need to create two new nodes for base and union them under original
            int id__base_intersection_dominated = push_back_new_node__and__get_id(base_intersection_dominated);
            nodes[id__base_intersection_dominated].applied_operations.push_back(
                    AppliedOperation(intersection, id__dominated_base, id__dominated));
            int id__base_diff_dominated = push_back_new_node__and__get_id(base_diff_dominated);
            nodes[id__base_diff_dominated].applied_operations.push_back(
                    AppliedOperation(difference, id__dominated_base, id__dominated));

            //update id__dominator_base is_union_of
            create_union_edge(id__dominated_base, id__base_intersection_dominated);
            create_union_edge(id__dominated_base, id__base_diff_dominated);
            make_union_node(id__dominated_base);
            //undo split

            //need to add id__dominated_base to to_union_dominated_parts;
            to_union_dominated_parts.push_back(id__base_intersection_dominated);
        }
        else
        {
            assert(false);
        }
        //update remaining_dominated
        remaining_dominated.apply_operation(difference, &nodes[id__dominated_base]);
    }
    assert(to_union_dominated_parts.size() == ids__dominated_base.size());
    assert(remaining_dominated.is_empty());

    if(ids__dominated_base.size() >= 2)
    {
        for(int i = 0;i<to_union_dominated_parts.size();i++)
        {
            create_union_edge(id__dominated, to_union_dominated_parts[i]);
        }
    }
    else{
        assert(to_union_dominated_parts.size() == 1);
        if(id__dominated != -1) {
            assert(id__dominated == to_union_dominated_parts[0]);
        } else{
            id__dominated = to_union_dominated_parts[0];
        }
    }

    assert(id__dominated != -1);

    remaining_dominated.my_delete();

    return id__dominated;
}

bool CompactPoset:: append(MetaExample meta_example) {

    local_delta = DeltaCompactPoset();
    local_delta.meta_example = meta_example;
    CompactPosetNode dominator = CompactPosetNode(meta_example.total_function);
    CompactPosetNode dominated = CompactPosetNode(meta_example.partial_function, difference, &dominator);


//    cout << "dominated: " << endl;
//    cout << dominated.to_string(num_inputs) <<endl;

    int id__dominator_base = -1;
    vector<int> ids__dominated_base;

    bool dominator_and_dominated_are_same = false;
    int dominator_and_dominated_are_same__at_id = -1;

    for(int i = 0;i<nodes.size();i++)
    {
        if(nodes[i].node_type == base_node)
        {
            bool is_dominator = false;
            if(!is_intersection_empty(&nodes[i], &dominator))
            {
                assert(id__dominator_base == -1);
                id__dominator_base = i;
                is_dominator = true;
            }

            if (!is_intersection_empty(&nodes[i], &dominated))
            {
                if (is_dominator)
                {
                    dominator_and_dominated_are_same = true;
                    dominator_and_dominated_are_same__at_id = (int) ids__dominated_base.size();
                }
                ids__dominated_base.push_back(i);
            }
        }
    }
    assert(id__dominator_base != -1);
    assert(!ids__dominated_base.empty());

    CompactPosetNode base_diff_dominator = get_difference(&nodes[id__dominator_base], &dominator);
    CompactPosetNode dominator_diff_base = get_difference(&dominator, &nodes[id__dominator_base]);

    if(base_diff_dominator.is_empty() && dominator_diff_base.is_empty())
    {
        assert(!dominator_and_dominated_are_same);
        base_diff_dominator.my_delete();
        dominator_diff_base.my_delete();

        //base and dominator are the same
        //no need to create new nodes for dominator
    }
    else if (!base_diff_dominator.is_empty() && dominator_diff_base.is_empty())
    {
        dominator_diff_base.my_delete();
        CompactPosetNode base_intersection_dominator = get_intersection(&nodes[id__dominator_base], &dominator);

        assert(!base_intersection_dominator.is_empty());

        //expected case when dominator is part of a leaf in the poset
        int id__base_intersection_dominator = push_back_new_node__and__get_id(base_intersection_dominator);
        nodes[id__base_intersection_dominator].applied_operations.push_back(
                AppliedOperation(intersection, nodes[id__dominator_base].id_in_compact_poset, id__base_intersection_dominator));

        int id__base_diff_dominator = push_back_new_node__and__get_id(base_diff_dominator);
        nodes[id__base_diff_dominator].applied_operations.push_back(
                AppliedOperation(difference, nodes[id__dominator_base].id_in_compact_poset, id__base_intersection_dominator));

        create_union_edge(id__dominator_base, id__base_intersection_dominator);
        create_union_edge(id__dominator_base, id__base_diff_dominator);
        make_union_node(id__dominator_base);

        //need to undo split

        if(dominator_and_dominated_are_same)
        {
//            cout << "dominator_and_dominated_are_same" <<endl;
            ids__dominated_base[dominator_and_dominated_are_same__at_id] = id__base_diff_dominator;
        }

        id__dominator_base = id__base_intersection_dominator;
    }
    else if (!dominator_diff_base.is_empty())
    {
        // not handled yet. This is when dominator is a set with more than one element
        assert(false);
    }

    int id__dominated = split_dominated_base(&dominated, id__dominator_base, ids__dominated_base);

    dominator.my_delete();
    dominated.my_delete();

    meta_edges[id__dominator_base].push_back(id__dominated);
    reverse_meta_edges[id__dominated].push_back(id__dominator_base);
    num_meta_edges+=1;
    local_delta.new_meta_edge = make_pair(id__dominator_base, id__dominated);
    delta_stack.push_back(local_delta);

    bool is_meta_example_consistent = is_valid_node(id__dominator_base);

    if(!is_meta_example_consistent)
    {
        int max_chain = get_loop_from_node(id__dominator_base);
        assert(max_chain != -1);
//        cout << "max_chain = " << max_chain << endl;
//        if(max_chain == 4) {
//            print();
//            assert(false);
//        }
    }
    else {
        meta_edges[id__dominator_base].pop_back();
        reverse_meta_edges[id__dominated].pop_back();
        num_meta_edges-=1;

        mark_dominated_init(id__dominator_base);

        meta_edges[id__dominator_base].push_back(id__dominated);
        reverse_meta_edges[id__dominated].push_back(id__dominator_base);
        num_meta_edges+=1;

        if (!there_are_new_dominated__init(id__dominator_base)) {
//        cout << "ID DOMINATOR BASE: " << id__dominator_base << endl;
//        cout << "ID DOMINATED BASE: " << id__dominated << endl;
//        print();
//        assert(false);
            pop();
            local_delta = DeltaCompactPoset();
            local_delta.was_poped = true;
            local_delta.meta_example = meta_example;
            delta_stack.push_back(local_delta);
        }
    }

    return is_meta_example_consistent;
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
    cout << endl;
}

int CompactPoset::get_loop(int at, int count) {
    open_visited[at] = visited_mark;
    int ret = -1;
    if(nodes[at].node_type == union_node)
    {
        for(auto subnode_id : is_union_of[at])
        {
            if (open_visited[subnode_id] == visited_mark) {
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
                }
            }
        }
    }
    for (auto next_id : meta_edges[at]) {
        if (open_visited[next_id] == visited_mark) {
            min_cycles[next_id] = 1;
            return 1;
        } else if (closed_visited[next_id] != visited_mark) {
            int val = get_loop(next_id, count);
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

string CompactPoset::to_string() {
    string ret;
    ret += "nodes:\n";
    for(int i = 0;i<nodes.size();i++)
    {
        ret += tabs(1) + "id = " + std::to_string(i) + "\n";
        ret += tabs(1) + "node_type = " + node_type_name[nodes[i].node_type] + "\n";
        ret += tabs(1) + "union_of_partial_functions = \n";
        ret += nodes[i].get_string_of_union_of_partial_functions(2, num_inputs);
        ret += tabs(1) + "applied_operations = \n";
        for(int j = 0;j<nodes[i].applied_operations.size();j++)
        {
            ret += tabs(2) + nodes[i].applied_operations[j].to_string() + "\n";
        }
        ret += "\n";
//        ret += tabs(1) + "decision_tree: \n";
//        ret += nodes[i].to_string(2, num_inputs) + "\n";
    }

    ret += vector_of_vector_of_int_to_string(meta_edges, "meta_edges");
    ret += vector_of_vector_of_int_to_string(is_union_of, "is_union_of");

    ret += "meta_example_stack: \n";
    for(int i = 0;i<delta_stack.size();i++)
    {

        ret += delta_stack[i].meta_example.linear_string(1);
        if(delta_stack[i].was_poped)
        {
            ret += " popped";
        }
        if(delta_stack[i].removed_after_the_fact)
        {
            ret += " removed_after_the_fact";
        }
        ret += "\n";
    }

    return ret;
}

void CompactPoset::pop()
{

    DeltaCompactPoset delta = delta_stack.back();
    delta_stack.pop_back();
    if(delta.was_poped)
    {
        return;
    }
    int dominator = delta.new_meta_edge.first;
    int dominated = delta.new_meta_edge.second;
    assert(dominated == meta_edges[dominator].back());
    meta_edges[dominator].pop_back();
    assert(reverse_meta_edges[dominated].back() == dominator);
    reverse_meta_edges[dominated].pop_back();
    num_meta_edges-=1;

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
}

void CompactPoset::create_union_edge(int container, int contained) {
    is_union_of[container].push_back(contained);
    is_contained_in[contained].push_back(container);
    local_delta.new_union_edges.push_back(make_pair(container, contained));
}

void CompactPoset::print()
{
    cout << to_string() <<endl;
}

int CompactPoset::get_num_nodes() {
    return (int) nodes.size();
}

void CompactPoset::make_union_node(int new_union_node) {
    if(nodes[new_union_node].node_type == base_node)
    {
        local_delta.new_union_nodes.push_back(new_union_node);
        nodes[new_union_node].node_type = union_node;
    }
    assert(nodes[new_union_node].node_type == union_node);
}

void CompactPoset::simple_pop() {
    DeltaCompactPoset delta = delta_stack.back();
    delta_stack.pop_back();
    if(delta.was_poped)
    {
        return;
    }
    int dominator = delta.new_meta_edge.first;
    int dominated = delta.new_meta_edge.second;
    assert(dominated == meta_edges[dominator].back());
    meta_edges[dominator].pop_back();
    assert(reverse_meta_edges[dominated].back() == dominator);
    reverse_meta_edges[dominated].pop_back();
    num_meta_edges-=1;
}

void CompactPoset::mark_dominated_init(int origin)
{
    visited_mark += 1;
    mark_dominated(origin);
}

bool CompactPoset::there_are_new_dominated__init(int origin) {
    visited_mark += 1;
    return there_are_new_dominated(origin);
}

bool CompactPoset::there_are_new_dominated(int at) {
    open_visited[at] = visited_mark;
    if(nodes[at].node_type == union_node)
    {
        for(auto subnode_id : is_union_of[at])
        {
            if (open_visited[subnode_id] == visited_mark) {
                assert(false);
            } else if (closed_visited[subnode_id] != visited_mark) {
                if (there_are_new_dominated(subnode_id)) {
                    return true;
                }
            }
        }
    }
    for (auto next_id : meta_edges[at]) {
        if (open_visited[next_id] == visited_mark) {
            assert(false);
        } else if (closed_visited[next_id] != visited_mark) {
            if(nodes[next_id].node_type == base_node && closed_visited[next_id] != visited_mark-1)
            {
                if(nodes[next_id].node_type == base_node)
                {
//                    cout << "dominated new: " << next_id << endl;
                }
                return true;
            }
            if (there_are_new_dominated(next_id)) {
                return true;
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

void CompactPoset::mark_dominated(int at) {
    open_visited[at] = visited_mark;
    if(nodes[at].node_type == union_node)
    {
        for(auto subnode_id : is_union_of[at])
        {
            if (open_visited[subnode_id] == visited_mark) {
                return ;
            } else if (closed_visited[subnode_id] != visited_mark) {
                mark_dominated(subnode_id);
            }
        }
    }
    for (auto next_id : meta_edges[at]) {
        if (open_visited[next_id] == visited_mark) {
            return ;
        } else if (closed_visited[next_id] != visited_mark) {
            mark_dominated(next_id);
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

bool CompactPoset::there_exist_redundant_meta_edge()
{
    bool removed_meta_edge = false;

    for(int i = delta_stack.size()-1;i>=0;i--)
    {
        if(!delta_stack[i].was_poped)
        {
            int dominator = delta_stack[i].new_meta_edge.first;
            int dominated = delta_stack[i].new_meta_edge.second;

            auto it_dominated = find(meta_edges[dominator].begin(), meta_edges[dominator].end(), dominated);
            auto it_dominator = find(reverse_meta_edges[dominated].begin(), reverse_meta_edges[dominated].end(), dominator);

            meta_edges[dominator].erase(it_dominated);
            reverse_meta_edges[dominated].erase(it_dominator);
            num_meta_edges-=1;

            mark_dominated_init(dominator);

            meta_edges[dominator].insert(it_dominated, dominated);
            reverse_meta_edges[dominated].insert(it_dominator, dominator);
            num_meta_edges+=1;

            if (!there_are_new_dominated__init(dominator))
            {
                removed_meta_edge = true;
                it_dominated = find(meta_edges[dominator].begin(), meta_edges[dominator].end(), dominated);
                it_dominator = find(reverse_meta_edges[dominated].begin(), reverse_meta_edges[dominated].end(), dominator);

                delta_stack[i].removed_after_the_fact = true;

                meta_edges[dominator].erase(it_dominated);
                reverse_meta_edges[dominated].erase(it_dominator);
                num_meta_edges-=1;

                removed_edges.push_back(make_pair(i, make_pair(it_dominated, it_dominator)));
            }

        }
    }
    return removed_meta_edge;
}

CompactPoset::CompactPoset(CompactPoset *poset) {
    num_inputs = poset->num_inputs;
    push_back_new_node__and__get_id(CompactPosetNode(PartialFunction(num_inputs, 0, 0)));

    vector<MetaExample> meta_examples = poset->get_meta_examples();
    for(int i = 0; i < meta_examples.size();i++)
    {
        append(meta_examples[i]);
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

bool CompactPoset::empty()
{
    return delta_stack.empty();
}

void CompactPoset::add_edges_back() {
    for(int i = removed_edges.size()-1;i>=0;i--)
    {
        int id = removed_edges[i].first;
        auto it_dominated = removed_edges[i].second.first;
        auto it_dominator = removed_edges[i].second.second;
        int dominator = delta_stack[id].new_meta_edge.first;
        int dominated = delta_stack[id].new_meta_edge.second;
        meta_edges[dominator].insert(it_dominated, dominated);
        reverse_meta_edges[dominated].insert(it_dominator, dominator);
        num_meta_edges+=1;
    }
    removed_edges.clear();
}

void CompactPoset::clear()
{
    add_edges_back();
    while(!empty())
    {
        pop();
    }
    for(int i = 0; i<nodes.size();i++)
    {
        nodes[i].my_delete();
        nodes.pop_back();
        is_union_of.pop_back();
        is_contained_in.pop_back();
        meta_edges.pop_back();
        reverse_meta_edges.pop_back();
    }
}
