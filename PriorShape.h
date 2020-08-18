//
// Created by Kliment Serafimov on 8/17/20.
//

#ifndef COMPACTPOSET_PRIORSHAPE_H
#define COMPACTPOSET_PRIORSHAPE_H

class PriorShape {

};


#endif //COMPACTPOSET_PRIORSHAPE_H


// What do I need to describe a prior?
// node represents a sequence of buckets.
// each subdomain in a bucket has a pointer to a node for the next iteration
//
// for example simplest set up is node -> node, where each subdomain leads to the node itself
// what I really need is a prior topology that evolves over time