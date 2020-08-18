//
// Created by Kliment Serafimov on 8/18/20.
//

#ifndef COMPACTPOSET_BEHAVIORTOPROGRAM_H
#define COMPACTPOSET_BEHAVIORTOPROGRAM_H

#include <map>
#include "Bitvector.h"

class CanvasAndBittreeProgram;

typedef map<Bitvector, vector<CanvasAndBittreeProgram*> >::iterator BehaviorToProgramIterator;

class BehaviorToProgram: public map<Bitvector, vector<CanvasAndBittreeProgram*> > {
public:
    void push_back(CanvasAndBittreeProgram* new_canvas_and_bittree_program);
    bool in_iter__all = false;
    bool in_iter__iterators = false;
    BehaviorToProgramIterator at_iterator;
    int at_program_id;
    CanvasAndBittreeProgram* init_iter__all();
    bool end_iter__all();
    CanvasAndBittreeProgram* get_next__all();

    BehaviorToProgramIterator init_iter__iterators();
    bool end_iter__iterators();
    BehaviorToProgramIterator get_next__iterators();

    void plus_equals(BehaviorToProgram* other);

};


#endif //COMPACTPOSET_BEHAVIORTOPROGRAM_H
