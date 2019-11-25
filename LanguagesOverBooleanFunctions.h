//
// Created by Kliment Serafimov on 2019-11-25.
//

#ifndef COMPACTPOSET_LANGUAGESOVERBOOLEANFUNCTIONS_H
#define COMPACTPOSET_LANGUAGESOVERBOOLEANFUNCTIONS_H

#include <vector>
#include <set>
#include "MetaExample.h"

using namespace std;

enum PrimitiveType {input_node, operator_node};

class Primitive
{
public:
    PrimitiveType primitive_type;
    string name;
    //if operator_node
    PartialFunction partial_function;
    //if input_node
    int input_node_id;

    Primitive() = default;
    Primitive(PartialFunction _partial_function, string _name);
    Primitive(int _input_node_id, string _name);

    int execute(int x);
};

class ProgramNode
{
public:

    Primitive primitive;
    vector<ProgramNode*> inputs;

    int executed = false;
    PartialFunction partial_function;

    ProgramNode() = default;
    ProgramNode(Primitive _primitive);

    string to_string(int num_tabs);
    void print();

    bool is_constant();

    int execute(int x);
};

class LanguagesOverBooleanFunctions {

    int num_inputs;
    vector<vector<Primitive> > primitives_by_num_inputs;

    vector<vector<ProgramNode*> > program_roots_by_size;

    set<int> boolean_functions;
    vector<PartialFunction> ordering_over_boolean_functions;

    void append_inputs();

public:
    LanguagesOverBooleanFunctions(int num_inputs, int language_id);

    void enumerate();

    MetaExample get_meta_example(PartialFunction partial_function);

};

int get_total_function(int num_inputs, ProgramNode* program_node);


#endif //COMPACTPOSET_LANGUAGESOVERBOOLEANFUNCTIONS_H
