//
// Created by Kliment Serafimov on 8/10/20.
//

#include "Mask.h"
#include "PartialFunction.h"

MaskAndCost::
MaskAndCost(CanvasAndBittreeProgram* _program): Mask(CanvasAsPartialFunction(_program->get_canvas()).total_function)
{
    program = _program;
    cost = program->get_cost();
}