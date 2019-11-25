//
// Created by Kliment Serafimov on 2019-11-25.
//

#include "LanguagesOverBooleanFunctions.h"
#include "util.h"
#include <iostream>

#define MAX_NUM_PROGRAM_NODES 400000

static int global_num_program_nodes = 0;
//vector<int> empty_slots;
ProgramNode all_program_nodes[MAX_NUM_PROGRAM_NODES];

ProgramNode* get_new_program_node(Primitive primitive)
{
    if(global_num_program_nodes>=MAX_NUM_PROGRAM_NODES)
    {
        cout << "expand MAX_NUM_PROGRAM_NODES" <<endl;
        assert(false);
    }
    all_program_nodes[global_num_program_nodes] = ProgramNode(primitive);
    return &all_program_nodes[global_num_program_nodes++];
}

void LanguagesOverBooleanFunctions::append_inputs()
{
    for(int i=0;i<num_inputs;i++)
    {
        primitives_by_num_inputs[0].push_back(Primitive(i, "x"+std::to_string(i)));
    }
}

LanguagesOverBooleanFunctions::LanguagesOverBooleanFunctions(int _num_inputs, int language_id) {

    num_inputs = _num_inputs;
    primitives_by_num_inputs = vector<vector<Primitive> >(3, vector<Primitive>());

    cout << "language = " << language_id << " :: ";

    if(language_id == 0) {
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 0, -1), "0"));
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 15, -1), "1"));
        append_inputs();
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 1, -1), "nor"));
    }
    else if(language_id == 1)
    {
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 0, -1), "0"));
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 15, -1), "1"));
        append_inputs();
        primitives_by_num_inputs[1].push_back(Primitive(PartialFunction(2, 1, -1), "not"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 8, -1), "and"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 14, -1), "or"));
    }
    else if(language_id == 2)
    {
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 0, -1), "0"));
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 15, -1), "1"));
        append_inputs();
        primitives_by_num_inputs[1].push_back(Primitive(PartialFunction(2, 1, -1), "not"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 14, -1), "or"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 8, -1), "and"));
    }
    else if(language_id == 3)
    {
        append_inputs();
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 0, -1), "0"));
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 15, -1), "1"));
        primitives_by_num_inputs[1].push_back(Primitive(PartialFunction(2, 1, -1), "not"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 8, -1), "and"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 14, -1), "or"));
    }
    else if(language_id == 4)
    {
        append_inputs();
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 0, -1), "0"));
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 15, -1), "1"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 6, -1), "xor"));
    }
    else if(language_id == 5)
    {
        append_inputs();
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 0, -1), "0"));
        primitives_by_num_inputs[0].push_back(Primitive(PartialFunction(2, 15, -1), "1"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 9, -1), "xnor"));
    }
    if(language_id == 6) {
        append_inputs();
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 1, -1), "nor"));
    }
    else if(language_id == 7)
    {
        append_inputs();
        primitives_by_num_inputs[1].push_back(Primitive(PartialFunction(2, 1, -1), "not"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 8, -1), "and"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, 14, -1), "or"));
    }
    else if(language_id >= 8)
    {
        append_inputs();
        primitives_by_num_inputs[1].push_back(Primitive(PartialFunction(2, 1, -1), "not"));
        primitives_by_num_inputs[2].push_back(Primitive(PartialFunction(2, language_id-8, -1), "custom"));
    }
}

void LanguagesOverBooleanFunctions::enumerate() {
    //size = 0
    program_roots_by_size.push_back(vector<ProgramNode*>());

    //size = 1
    program_roots_by_size.push_back(vector<ProgramNode*>());


    for(int i = 0;i<primitives_by_num_inputs[0].size();i++)
    {
        ProgramNode *new_program = get_new_program_node(primitives_by_num_inputs[0][i]);
        int total_function = get_total_function(num_inputs, new_program);

        if(boolean_functions.find(total_function) == boolean_functions.end())
        {
            boolean_functions.insert(total_function);
            ordering_over_boolean_functions.push_back(PartialFunction(num_inputs, total_function, -1));
            program_roots_by_size[1].push_back(new_program);
//                                new_program->print();
        }
    }

    int max_size = 26;
    //size >= 2
    for(int size = 2; size<=max_size; size++)
    {
        if(ordering_over_boolean_functions.size() >= (1<<(1<<num_inputs)))
        {
            break;
        }
//        cout << size <<endl;
        program_roots_by_size.push_back(vector<ProgramNode*>());
        for(int local_num_inputs = 1; local_num_inputs < primitives_by_num_inputs.size(); local_num_inputs++)
        {
            assert(local_num_inputs <= 2);
            if(local_num_inputs == 1)
            {
                for(int i = 0;i<primitives_by_num_inputs[local_num_inputs].size();i++)
                {
                    for(int j = 0;j<program_roots_by_size[size-1].size();j++)
                    {
                        if(program_roots_by_size[size-1][j]->is_constant())
                        {
                            continue;
                        }

                        ProgramNode *new_program = get_new_program_node(primitives_by_num_inputs[local_num_inputs][i]);
                        new_program->inputs.push_back(program_roots_by_size[size-1][j]);
                        int total_function = get_total_function(num_inputs, new_program);

                        if(boolean_functions.find(total_function) == boolean_functions.end())
                        {
                            boolean_functions.insert(total_function);
                            ordering_over_boolean_functions.push_back(PartialFunction(num_inputs, total_function, -1));
                            program_roots_by_size[size].push_back(new_program);
//                                new_program->print();
                        }
                    }
                }
            }
            else
            {
                for(int i = 0;i<primitives_by_num_inputs[local_num_inputs].size();i++)
                {
                    int sum_of_size_of_inputs = size-1;
                    for(int left_input_size = 1; left_input_size<sum_of_size_of_inputs;left_input_size++)
                    {
                        int right_input_size = sum_of_size_of_inputs - left_input_size;
//                        cout << left_input_size <<" "<< right_input_size << endl;
                        for(int j0 = 0;j0<program_roots_by_size[left_input_size].size();j0++)
                        {
                            for(int j1 = 0;j1<program_roots_by_size[right_input_size].size();j1++)
                            {
                                if(program_roots_by_size[left_input_size][j0]->is_constant() &&
                                        program_roots_by_size[right_input_size][j1]->is_constant())
                                {
                                    continue;
                                }

                                ProgramNode *new_program = get_new_program_node(primitives_by_num_inputs[local_num_inputs][i]);
                                new_program->inputs.push_back(program_roots_by_size[left_input_size][j0]);
                                new_program->inputs.push_back(program_roots_by_size[right_input_size][j1]);

                                int total_function = get_total_function(num_inputs, new_program);
//                               todo: update_ordering_over_boolean_functions(size, total_function);
                                
                                if(boolean_functions.find(total_function) == boolean_functions.end())
                                {
                                    boolean_functions.insert(total_function);
                                    ordering_over_boolean_functions.push_back(PartialFunction(num_inputs, total_function, -1));
                                    program_roots_by_size[size].push_back(new_program);
//                                new_program->print();
                                }

                            }
                        }
                    }
                }
            }
        }
    }

    int count = 0;
    for(int size = 0; size <= max_size; size++)
    {
//        cout << "size = " << size <<endl;
        for (int i = 0; i < program_roots_by_size[size].size(); i++)
        {
            count+=1;
            int total_function = 0;
//            cout << "count = " << count << endl;
//            program_roots_by_size[size][i]->print();
            for (int x = 0; x < (1 << num_inputs); x++) {
                int result = program_roots_by_size[size][i]->execute(x);
                total_function+=(1<<x)*result;
//                cout << bitvector_to_str(x, 2) << " " << result << endl;
            }
            cout << total_function <<" ";
//            cout << endl;
        }
    }
    cout << endl;
}

MetaExample LanguagesOverBooleanFunctions::get_meta_example(PartialFunction partial_function) {
    for(int i = 0;i<ordering_over_boolean_functions.size();i++)
    {
        PartialFunction boolean_function = ordering_over_boolean_functions[i];
        if(boolean_function.is_generalization_of(partial_function))
        {
            return MetaExample(boolean_function.num_inputs, boolean_function.total_function, partial_function.partition);
        }
    }
    assert(false);
}

int get_total_function(int num_inputs, ProgramNode* program_node)
{
    int total_function = 0;
//    program_node->print();
    for (int x = 0; x < (1 << num_inputs); x++) {
        int result = program_node->execute(x);
        total_function+=(1<<x)*result;
//        cout << bitvector_to_str(x, 2) << " " << result << endl;
    }
    return total_function;
}

Primitive::Primitive(PartialFunction _partial_function, string _name) {
    partial_function = _partial_function;
    primitive_type = operator_node;
    name = _name;
}

Primitive::Primitive(int _input_node_id, string _name) {
    input_node_id = _input_node_id;
    primitive_type = input_node;
    name = _name;
}

int Primitive::execute(int x) {
    if(primitive_type == input_node)
    {
        return get_bit(x, input_node_id);
    }
    else
    {
        return get_bit(partial_function.total_function, x);
    }
}

ProgramNode::ProgramNode(Primitive _primitive) {
    primitive = _primitive;
}

string ProgramNode::to_string(int num_tabs) {
    string ret;
    ret += tabs(num_tabs) + primitive.name + "(";
    if(inputs.size() >= 1)
    {
        ret+="\n";
    }
    for(int i = 0;i<inputs.size();i++)
    {
        ret += inputs[i]->to_string(num_tabs+1);
        ret += "\n";
    }
    if(inputs.size()>=1)
    {
        ret+=tabs(num_tabs);
    }
    ret+=")";
    return ret;
}

void ProgramNode::print() {
    cout << to_string(0) <<endl;
}

bool ProgramNode::is_constant() {
    return primitive.primitive_type == operator_node && inputs.size() == 0;
}

int ProgramNode::execute(int x) {
    if(inputs.size()>=1) {
        vector<int> results;
        for (int i = 0; i < inputs.size(); i++) {
            results.push_back(inputs[i]->execute(x));
        }
        int root_x = 0;
        for (int i = 0; i < results.size(); i++) {
            root_x += (1 << i) * results[i];
        }
        return primitive.execute(root_x);
    } else
    {
        return primitive.execute(x);
    }
}

