//
// Created by Kliment Serafimov on 8/18/20.
//

#ifndef COMPACTPOSET_BEHAVIORTOPROGRAM_H
#define COMPACTPOSET_BEHAVIORTOPROGRAM_H

#include <map>
#include <set>
#include "Bitvector.h"
#include "Pointer.h"

class CanvasAndBittreeProgram;

typedef map<Bitvector, multiset<Pointer<CanvasAndBittreeProgram> > > BehaviorToProgramMap;
typedef BehaviorToProgramMap::iterator BehaviorToProgramIterator;

class BehaviorToProgram: public BehaviorToProgramMap {
public:
    void my_push_back(CanvasAndBittreeProgram* new_canvas_and_bittree_program);
    bool in_iter__all = false;
    bool in_iter__iterators = false;
    BehaviorToProgramIterator at_iterator;
    multiset<Pointer<CanvasAndBittreeProgram> >::iterator at_set_iterator;
    CanvasAndBittreeProgram* init_iter__all();
    bool end_iter__all();
    CanvasAndBittreeProgram* get_next__all();

    BehaviorToProgramIterator init_iter__iterators();
    bool end_iter__iterators();
    BehaviorToProgramIterator get_next__iterators();

    void plus_equals(BehaviorToProgram* other);

};


#endif //COMPACTPOSET_BEHAVIORTOPROGRAM_H
