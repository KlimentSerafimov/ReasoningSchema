//
// Created by Kliment Serafimov on 8/15/20.
//

#ifndef COMPACTPOSET_PRIOR_H
#define COMPACTPOSET_PRIOR_H

#include "vector"

using namespace std;

class MaskAndCostAndInstantiatedModules;

class Prior : public vector<vector<MaskAndCostAndInstantiatedModules*> > {};


#endif //COMPACTPOSET_PRIOR_H
