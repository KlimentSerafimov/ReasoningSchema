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
    Prior();
    Prior(MaskBuckets* masks);
    void push_back_next_subdomains_to_masks(MaskBuckets* masks, MaskBucket next_subdomains);
    void initialize(Prior* next_prior);
    void push_back_new_bucket_with_enumerated_masks(int function_size, int min_mask_size, int max_mask_size, MaskBuckets* masks_of_task_id);
    void push_back_prior_suffix(Prior* suffix);
    void remove_duplicates();
};


class MaskBuckets : public vector<MaskBucket>
{
public:
    MaskBuckets(Prior* prior);
    MaskBuckets() = default;
    void remove_duplicates();
};

#endif //COMPACTPOSET_PRIOR_H
