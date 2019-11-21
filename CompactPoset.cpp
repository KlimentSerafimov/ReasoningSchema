//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "CompactPoset.h"
#include <string>
#include <iostream>

using namespace std;

int CompactPoset::push_back_new_node__and__get_id(CompactPosetNode decision_tree, vector<int> to_union_nodes) {
    int ret = (int) nodes.size();
    nodes.push_back(decision_tree);
    nodes[ret].id_in_compact_poset = ret;
    is_union_of.push_back(vector<int>());
    is_contained_in.push_back(vector<int>());
    for(int i = 0;i<to_union_nodes.size();i++)
    {
        create_union_edge(ret, to_union_nodes[i]);
    }
    meta_edges.push_back(vector<int>());
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
    copy_first.apply_operation(difference, second);
    cout << "result: " << endl;
    cout << copy_first.get_string_of_union_of_partial_functions(1, num_inputs);
}


void CompactPoset::split_nodes__and__insert_meta_edge_in_poset(
        CompactPosetNode *dominated, int id__dominator_base, vector<int> ids__dominated_base) {


    int id__dominated = -1;

    CompactPosetNode remaining_dominated;
    if(ids__dominated_base.size() >= 2)
    {
        id__dominated = push_back_new_node__and__get_id(dominated->copy());
        nodes[id__dominated].node_type = union_node;
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
            int id__last_dominated = -1;
            if(i>=1)
            {
                CompactPosetNode base_intersection_dominated = get_intersection(
                        &nodes[id__dominated_base], &remaining_dominated);
                id__last_dominated = push_back_new_node__and__get_id(base_intersection_dominated);
                nodes[id__last_dominated].applied_operations.push_back(
                        AppliedOperation(intersection, id__dominated_base, id__dominated));
                int id__base_diff_dominated = push_back_new_node__and__get_id(base_diff_dominated);
                nodes[id__base_diff_dominated].applied_operations.push_back(
                        AppliedOperation(difference, id__dominated_base, id__last_dominated));

                create_union_edge(id__dominated_base, id__last_dominated);
                create_union_edge(id__dominated_base, id__base_diff_dominated);
                nodes[id__dominated_base].node_type = union_node;
            }
            else
            {
                id__last_dominated = id__dominated_base;
            }
            to_union_dominated_parts.push_back(id__last_dominated);
        }
        else if (!base_diff_dominated.is_empty() && !dominated_diff_base.is_empty())
        {
            dominated_diff_base.my_delete();
            assert(id__dominated != -1);
            CompactPosetNode base_intersection_dominated = get_intersection(&nodes[id__dominated_base], dominated);

            //need to create two new nodes for base and union them under original
            int id__base_intersection_dominated = push_back_new_node__and__get_id(base_intersection_dominated.copy());
            nodes[id__base_intersection_dominated].applied_operations.push_back(
                    AppliedOperation(intersection, id__dominated_base, id__dominated));
            int id__base_diff_dominated = push_back_new_node__and__get_id(base_diff_dominated);
            nodes[id__base_diff_dominated].applied_operations.push_back(
                    AppliedOperation(difference, id__dominated_base, id__dominated));

            //update id__dominator_base is_union_of
            create_union_edge(id__dominated_base, id__base_intersection_dominated);
            create_union_edge(id__dominated_base, id__base_diff_dominated);
            nodes[id__dominated_base].node_type = union_node;

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

    if(ids__dominated_base.size() >= 2)
    {
        for(int i = 0;i<to_union_dominated_parts.size();i++)
        {
            create_union_edge(id__dominated, to_union_dominated_parts[i]);
        }
    }
    else{
        assert(to_union_dominated_parts.size() == 1);
        assert(id__dominated == -1);
        id__dominated = to_union_dominated_parts[0];
    }

    assert(id__dominated != -1);
    assert(remaining_dominated.is_empty());

    remaining_dominated.my_delete();

    meta_edges[id__dominator_base].push_back(id__dominated);
    dominator_stack.push_back(id__dominator_base);
}


bool CompactPoset:: append(MetaExample meta_example) {

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

        split_nodes__and__insert_meta_edge_in_poset(&dominated, id__dominator_base, ids__dominated_base);

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
        nodes[id__dominator_base].node_type = union_node;

        if(dominator_and_dominated_are_same)
        {
            ids__dominated_base[dominator_and_dominated_are_same__at_id] = id__base_diff_dominator;
        }

        id__dominator_base = id__base_intersection_dominator;

        split_nodes__and__insert_meta_edge_in_poset(&dominated, id__dominator_base, ids__dominated_base);

    }
    else if (!dominator_diff_base.is_empty())
    {
        // not handled yet. This is when dominator is a set with more than one element
        assert(false);
    }

    dominator.my_delete();
    dominated.my_delete();

//    cout << endl;
//    cout << "AFTER APPEND:" <<endl;
//    cout << to_string() << endl;

//    for(int i = 0;i<nodes.size();i++)
//    {
//        if(nodes[i].node_type == base_node) {
//            for (int j = i + 1; j < nodes.size(); j++) {
//                if (nodes[j].node_type == base_node)
//                {
//                    if(!get_intersection(&nodes[i], &nodes[j]).is_empty())
//                    {
//                        print_operation(intersection, &nodes[i], &nodes[j]);
//                        assert(false);
//                    }
//                }
//            }
//        }
//    }
//    cout << endl;

    return is_valid_node(id__dominator_base);

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

    return ret;
}

void CompactPoset::pop() {
    int last_dominator = dominator_stack.back();
    dominator_stack.pop_back();
    meta_edges[last_dominator].pop_back();
}

void CompactPoset::create_union_edge(int container, int contained) {
    is_union_of[container].push_back(contained);
    is_contained_in[contained].push_back(container);
}

void CompactPoset::print()
{
    cout << to_string() <<endl;
}