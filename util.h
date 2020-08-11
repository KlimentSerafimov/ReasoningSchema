//
// Created by Kliment Serafimov on 2019-11-18.
//

#ifndef COMPACTPOSET_UTIL_H
#define COMPACTPOSET_UTIL_H

#include <string>
#include "Bitvector.h"
#include <vector>

using namespace std;

string tabs(int num_tabs);

int get_bit_of_int(int bitvector, int idx);

int get_bit(Bitvector bitvector, int idx);

string bitvector_to_str(int bitvector, int n);

string bitvector_to_str(Bitvector bitvector, int n);

int string_to_bitvector(string str);

string vector_of_vector_of_int_to_string(vector<vector<int> > to_print, string title);

unsigned int num_trailing_zeroes(Bitvector v);

enum NodeType {leaf_node, internal_node};

enum Rule {inherit_from_parent, stay, move_right, move_left, move_to_last_copy, copy_right, copy_left, move_up, move_down};
static const string rule_names[20] =
        {"inherit_from_parent", "stay", "move_right", "move_left", "move_to_last_copy", "copy_right", "copy_left", "move_up", "move_down"};
static const int rule_cost[20] = {0, 1, 2, 2, 100, 100, 100, 2, 2};


#endif //COMPACTPOSET_UTIL_H
