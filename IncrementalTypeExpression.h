//
// Created by Kliment Serafimov on 1/21/20.
//

#ifndef COMPACTPOSET_INCREMENTALTYPEEXPRESSION_H
#define COMPACTPOSET_INCREMENTALTYPEEXPRESSION_H

#include "Task.h"
#include "BittreeTaskType.h"


class IncrementalTypeExpression
{
public:

    BittreeTaskType* base_task_type;
    BittreeInputOutputType* init_delta_task_type;
    BittreeInputOutputType* delta_task_type;

    vector<BittreeInputOutputType*> deltas;

    // type_expression(type) = type + delta_type

    //sum: base = . . -> _
    //sum: delta = _ _ -> _

    //base + delta = (. . -> _) + (_ _ -> _) = (_ _ -> __)
    //base + delta + delta = (_ _ -> __) + (_ _ -> _) = (__ __ -> ___)

    IncrementalTypeExpression() = default;

    IncrementalTypeExpression(Task *task_name);

    void build_input_type(int num_input_operands, int* init_input_sizes, int* delta_input_sizes);

    void build_input_type(int num_input_operands);

    //void build_input_type(int num_input_operands, const int init_input_operands[], int delta_input_operands[]);

    void build_output_type(int num_outputs, int* init_output_sizes, int* delta_output_sizes) const;

    void build_type__add_leafs(int num_input_operands, int init_output_size, int delta_output_size);

};


#endif //COMPACTPOSET_INCREMENTALTYPEEXPRESSION_H
