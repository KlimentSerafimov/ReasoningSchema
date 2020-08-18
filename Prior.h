//
// Created by Kliment Serafimov on 8/15/20.
//

#ifndef COMPACTPOSET_PRIOR_H
#define COMPACTPOSET_PRIOR_H

#include "vector"

using namespace std;

class MaskAndCostAndInstantiatedModules;
class MaskAndNextPrior;

class MaskBucket : public vector<MaskAndCostAndInstantiatedModules*>
{

};

class MaskBuckets;

class Prior : public vector<vector<MaskAndNextPrior*> > {
public:
    Prior(MaskBuckets* masks);
    void initialize(Prior* next_prior);
};


class MaskBuckets : public vector<MaskBucket>
{
public:
    MaskBuckets(Prior* prior);
    MaskBuckets() = default;
};

#endif //COMPACTPOSET_PRIOR_H
