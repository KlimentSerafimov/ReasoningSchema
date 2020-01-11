//
// Created by Kliment Serafimov on 2019-11-18.
//

#ifndef COMPACTPOSET_UTIL_H
#define COMPACTPOSET_UTIL_H

#include <string>

using namespace std;

string tabs(int num_tabs);

int get_bit(int bitvector, int idx);

string bitvector_to_str(int bitvector, int n);

int string_to_bitvector(string str);

string vector_of_vector_of_int_to_string(vector<vector<int> > to_print, string title);

unsigned int num_trailing_zeroes(unsigned int v);

enum NodeType {leaf_node, internal_node};

#endif //COMPACTPOSET_UTIL_H
