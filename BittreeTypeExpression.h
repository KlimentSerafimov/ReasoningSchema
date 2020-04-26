//
// Created by Kliment Serafimov on 1/21/20.
//

#ifndef COMPACTPOSET_BITTREETYPEEXPRESSION_H
#define COMPACTPOSET_BITTREETYPEEXPRESSION_H

#include "TaskName.h"
#include "BittreeTaskType.h"

class BittreeTypeExpression
{
public:

    BittreeTaskType* base_task_type;
    BittreeInputOutputType* init_delta_task_type;
    BittreeInputOutputType* delta_task_type;

    BittreeTypeExpression(TaskName task_name);

    void build_input_type(int num_input_operands, int* init_input_sizes, int* delta_input_sizes);

    void build_input_type(int num_input_operands);

    //void build_input_type(int num_input_operands, const int init_input_operands[], int delta_input_operands[]);

    void build_output_type(int num_outputs, int* init_output_sizes, int* delta_output_sizes) const;

    void build_type(int num_input_operands, int init_output_size, int delta_output_size);

};


#endif //COMPACTPOSET_BITTREETYPEEXPRESSION_H
