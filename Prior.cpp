//
// Created by Kliment Serafimov on 8/15/20.
//

#include "Prior.h"
#include "MaskAndCostAndInstantiatedModules.h"

MaskBuckets::MaskBuckets(Prior *prior)
{
    assert(size() == 0);
    assert(prior != nullptr);

    for(int i = 0;i<prior->size();i++)
    {
        push_back(MaskBucket());
        for(int j = 0;j<prior->at(i).size();j++)
        {
            at(i).push_back(prior->at(i)[j]->mask);
        }
    }
}

void MaskBuckets::remove_duplicates()
{
    MaskBuckets ret_masks_of_task_id;
    set<Bitvector> masks;
    for(int i = 0;i<size();i++)
    {
        ret_masks_of_task_id.push_back(MaskBucket());
        for(int j = 0;j<at(i).size();j++)
        {
            if(masks.find(*at(i)[j]) == masks.end())
            {
                ret_masks_of_task_id[i].push_back(at(i)[j]);
                masks.insert((Bitvector)*at(i)[j]);
            }
        }
    }
    (*this) = ret_masks_of_task_id;
}

Prior::Prior(MaskBuckets *masks) {

    assert(size() == 0);
    assert(masks != nullptr);

    for(int i = 0;i<masks->size();i++)
    {
        push_back(vector<MaskAndNextPrior*>());
        for(int j = 0;j<masks->at(i).size();j++)
        {
            at(i).push_back(new MaskAndNextPrior(masks->at(i)[j], nullptr));
        }
    }
}

void Prior::remove_duplicates()
{
    Prior ret_prior;
    set<Bitvector> masks;
    for(int i = 0;i<size();i++)
    {
        ret_prior.push_back(vector<MaskAndNextPrior*>());
        for(int j = 0;j<at(i).size();j++)
        {
            if(masks.find(*at(i)[j]->mask) == masks.end())
            {
                ret_prior[i].push_back(at(i)[j]);
                masks.insert((Bitvector)*at(i)[j]->mask);
            }
        }
    }
    (*this) = ret_prior;
}


void Prior::initialize(Prior *next_prior)
{
    for(int i = 0;i<size();i++)
    {
        for(int j = 0;j<at(i).size();j++)
        {
            at(i)[j]->next_prior = next_prior;
        }
    }
}

static bool next_mask(Bitvector &v)// current permutation of bits
{
    int count = 0;
    for(int i = 0;i<v.get_size();i++)
    {
        if(v.get_bit(i))
        {
            if(i == v.get_size() - 1)
            {
                return false;
            }
            else
            {
                v.set(i, 0);
                if(!v.get_bit(i+1))
                {
                    v.set(i+1);
                    for(int j = 0;j<count;j++)
                    {
                        v.set(j);
                    }
                    break;
                }
                else
                {
                    count++;
                }
            }
        }
    }
    return true;
}

void append_to_masks(int function_size, int min_mask_size, int max_mask_size, MaskBuckets *ret) {

    if (function_size == 0) {
        return;
    }

    vector<Pointer<MaskAndCostAndInstantiatedModules>> ret_with_cost;

    if (min_mask_size < 0) {
        min_mask_size = function_size + min_mask_size;
        min_mask_size = max(min_mask_size, 2);
    }
    if (max_mask_size < 0) {
        max_mask_size = function_size + max_mask_size;
        max_mask_size = max(max_mask_size, 3);
    }

    min_mask_size = min(min_mask_size, function_size);
    max_mask_size = min(max_mask_size, function_size);


    for (int i = min_mask_size; i <= max_mask_size; i++) {
        Bitvector local_mask(all_zeroes, function_size);
        for (int j = 0; j < i; j++) {
            local_mask.set(j);
        }
        do {
            ret_with_cost.emplace_back(
                    new MaskAndCostAndInstantiatedModules(
                            new MaskAndCost(i, local_mask)));
        } while (next_mask(local_mask));
    }

    sort(ret_with_cost.begin(), ret_with_cost.end());

    MaskBucket ret_bucket;

    for (int i = 0; i < ret_with_cost.size(); i++) {
        ret_bucket.push_back(ret_with_cost[i].get_pointer());
    }

    ret->push_back(ret_bucket);
    ret_bucket.clear();
}

void Prior::push_back_next_subdomains_to_masks(MaskBuckets *masks, MaskBucket next_subdomains) {
    bool more_buckets = false;
    if(more_buckets) {
        AutomatonRuleCost prev_cost = -1;
        MaskBucket local_subdomains;
        vector<MaskAndNextPrior*> local_subdomains_with_prior;
        for (int i = 0; i < next_subdomains.size(); i++) {
            if (prev_cost != next_subdomains[i]->get_cost() && prev_cost != -1) {
                masks->push_back(local_subdomains);
                local_subdomains.clear();
            }
            local_subdomains.push_back(next_subdomains[i]);
            local_subdomains_with_prior.push_back(new MaskAndNextPrior(next_subdomains[i], nullptr));
            prev_cost = next_subdomains[i]->get_cost();
        }
        masks->push_back(local_subdomains);
        local_subdomains.clear();
    }
    else
    {
        masks->push_back(next_subdomains);
        vector<MaskAndNextPrior*> local_subdomains_with_prior;
        for(int i = 0;i<next_subdomains.size();i++)
        {
            local_subdomains_with_prior.push_back(new MaskAndNextPrior(next_subdomains[i], nullptr));
        }
    }
}

void Prior::push_back_new_bucket_with_enumerated_masks(int function_size, int min_mask_size, int max_mask_size, MaskBuckets* masks_of_task_id) {
    append_to_masks(function_size, min_mask_size, max_mask_size, masks_of_task_id);
}

void Prior::push_back_prior_suffix(Prior* suffix) {
    for(int i = 0;i<(int)suffix->size();i++)
    {
        push_back(suffix->at(i));
    }
}

Prior::Prior() : vector<vector<MaskAndNextPrior *>>() {

}
