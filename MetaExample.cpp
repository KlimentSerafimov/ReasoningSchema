//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "MetaExample.h"
#include "util.h"
#include <iostream>

MetaExample::MetaExample(
        int _function_size, Bitvector _total_function, Bitvector _partition) {
    partial_function = PartialFunction(_function_size, _total_function, _partition);
    generalization = PartialFunction(_function_size, _total_function, Bitvector(all_ones, _function_size));
}

MetaExample::MetaExample(int _function_size, Bitvector _total_function, Bitvector _partition, Bitvector generalization_partition) {

    partial_function = PartialFunction(_function_size, _total_function, _partition);
    generalization = PartialFunction(_function_size, _total_function, generalization_partition);
}


MetaExample::MetaExample(PartialFunction _partial_function){
    partial_function = _partial_function;
    generalization = PartialFunction(
            partial_function.function_size, partial_function.total_function, Bitvector(all_ones, partial_function.function_size));
}


int MetaExample::get_compact_partial_outputs()
{
    int compact_partial_outputs = 0;
    for(int i = 0, j = 0;i<partial_function.function_size; i++) {
        if (get_bit(partial_function.partition, i)) {
            compact_partial_outputs += (1 << j) * get_bit(partial_function.total_function, i);
            j++;
        }
    }
    return compact_partial_outputs;
}

int MetaExample::get_compact_hidden_outputs() {
    int compact_hidden_outputs = 0;
    for(int i = 0, j = 0;i<partial_function.function_size; i++) {
        if (!get_bit(partial_function.partition, i)) {
            compact_hidden_outputs += (1 << j) * get_bit(partial_function.total_function, i);
            j++;
        }
    }
    return compact_hidden_outputs;
}


string MetaExample::to_string() {
    return linear_string(0);
}

string MetaExample::linear_string(int tab)
{
    string ret;
    for(int i = 0;i<tab;i++)
    {
        ret+="\t";
    }
//    ret += "{";
//    ret += "(";
//    ret += "   ";
    ret += partial_function.to_string();
    ret += " -> ";
    ret += generalization.to_string();
//    ret += ")";
//    ret += "}";
    return ret;
}

void MetaExample::print()
{
    cout << to_string() << endl;
}

MetaExample::MetaExample(int _function_size, Bitvector _total_function, Bitvector _partition, Bitvector generalization_partition, int _idx) {
    partial_function = PartialFunction(_function_size, _total_function, _partition);
    generalization = PartialFunction(_function_size, _total_function, generalization_partition);
    idx = _idx;
}

MetaExample::MetaExample(int _function_size, Bitvector _total_function, Bitvector _partition, Bitvector generalization_partition, int _idx, bool _active) {
    partial_function = PartialFunction(_function_size, _total_function, _partition);
    generalization = PartialFunction(_function_size, _total_function, generalization_partition);
    idx = _idx;
    active = _active;
}

bool MetaExample::is_consistent_with(MetaExample other) {
    assert(generalization.partition == other.generalization.partition);

    if(other.generalization.is_contained_in(partial_function) &&
            generalization.is_contained_in(other.partial_function))
    {
        bool ret =
            (other.generalization.total_function & other.generalization.partition) ==
            (generalization.total_function & generalization.partition);
        return ret;
    }

    return true;
}

MetaExample::MetaExample(PartialFunction _partial_function, PartialFunction _generalization, int _idx) {
    assert(_generalization.is_contained_in(_partial_function));
    partial_function = _partial_function;
    generalization = _generalization;
    idx = _idx;
}

float MetaExample::cost(Bitvector mask)
{
    vector<vector<int> > bit_coordinates;
    for(int i = 0;i<partial_function.bits.size();i++)
    {
        if(get_bit(mask, i))
        {
            /*for(int j = 0;j<partial_function.bits[i]->names.size(); j++)
            {
                cout << partial_function.bits[i]->names[j].to_string() << " " << partial_function.bits[i]->names[j].id <<"  ";
            }
            cout << endl;*/
            NodeTemplate* at = partial_function.bits[i]->parents.back();
            vector<int> coord;
            while(at->names.back().is_child)
            {
                cout << at->names.back().id <<" ";
                coord.push_back(at->names.back().id);
                at = at->parents.back();
            }
            bit_coordinates.push_back(coord);
            cout << "-- ";

//            cout << partial_function.bits[i]->names.back().to_string() << " ";
//            cout << partial_function.bits[i]->parents.back()->names.back().to_string() << " ";
//            cout << partial_function.bits[i]->parents.back()->parents.back()->names.back().to_string() << " ";
//            cout << partial_function.bits[i]->parents.back()->parents.back()->parents.back()->names.back().to_string() << " ";
//            cout << partial_function.bits[i]->parents.back()->parents.back()->parents.back()->parents.back()->names.back().to_string() << " ";
//            cout << partial_function.bits[i]->parents.back()->parents.back()->parents.back()->parents.back()->parents.back()->names.back().to_string() << " ";
//            NodeTemplate* at = partial_function.bits[i]->parents.back()->parents.back()->parents.back()->parents.back()->parents.back();
//            while (at->names.back().to_string() == "subtask")
//            {
//                cout << at->parents.back()->names.back().to_string() << " ";
//                cout << at->parents.back()->parents.back()->names.back().to_string() << " ";
//                at = at->parents.back()->parents.back();
//            }
//            cout << " -- ";
//            ret += partial_function.bits[i]->names.back().id;
//            count++;
        }
    }

    int greatest_dist = 0;
    for(int i = 0;i<bit_coordinates.size();i++)
    {
        for(int j = i+1;j<bit_coordinates.size();j++)
        {
            int sum = 0;
            for(int k = 0;k<min(bit_coordinates[i].size(), bit_coordinates[j].size());k++)
            {
                sum += abs(bit_coordinates[i][k] - bit_coordinates[j][k]);
            }
            greatest_dist = max(greatest_dist, sum);
        }
    }

    cout << endl;
    cout << mask.to_string() <<" "<< greatest_dist << endl;
    return (float)greatest_dist;
}

vector<Bitvector> MetaExample::get_masks(int min_mask_size, int max_mask_size)
{

    vector<Bitvector> ret;

    vector<pair<float, Bitvector> > ret_with_cost;

    if(get_function_size() == 0)
    {
        return ret;
    }
    cout << "MASKS: " << endl;
    for(int i = min_mask_size;i<=max_mask_size;i++)
    {
        Bitvector local_mask(all_zeroes, get_function_size());
        for(int j = 0;j<i;j++)
        {
            local_mask.set(j);
        }
        do{
            ret_with_cost.push_back(make_pair(cost(local_mask), local_mask));
            ret.push_back(local_mask);
            //cout << bitvector_to_str(local_mask, get_function_size()) << endl;
        }while(next_mask(local_mask, i));
    }

    sort(ret_with_cost.begin(), ret_with_cost.end());
    //reverse(ret_with_cost.begin(), ret_with_cost.end());

    ret.clear();

    for(int i = 0;i<ret_with_cost.size();i++)
    {
        cout << ret_with_cost[i].second.to_string() << " " << ret_with_cost[i].first;
        if (i < ret_with_cost.size())
        {
            cout << " include";
            ret.push_back(ret_with_cost[i].second);
        }
        cout << endl;

    }
    cout << endl;
    return ret;

}

int get_num_missing_bits(vector<MetaExample> meta_examples)
{
    int ret = 0;
    for(int i = 0; i<meta_examples.size();i++)
    {
        ret += meta_examples[i].generalization.partition.count() -
               meta_examples[i].partial_function.partition.count();
    }
    return ret;
}


