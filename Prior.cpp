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
