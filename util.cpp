//
// Created by Kliment Serafimov on 2019-11-19.
//


#include "util.h"
#include <vector>

string tabs(int num_tabs) {
    string ret;
    for(int i = 0;i<num_tabs;i++)
    {
        ret += "\t";
    }
    return ret;
}

string bitvector_to_str(int bitvector, int n)
{
    string ret = "";
    for(int i = n-1;i>=0;i--)
    {
        if(get_bit(bitvector, i))
        {
            ret+="1";
        }
        else
        {
            ret+="0";
        }
    }
    return ret;
}

int get_bit(int bitvector, int idx) {
    return (bitvector & (1<<idx)) != 0;
}

string vector_of_vector_of_int_to_string(vector<vector<int> > to_print, string title)
{
    string ret;
    ret += title + ": \n";
    for(int i = 0;i<to_print.size();i++)
    {
        ret += tabs(1) + "i = " + std::to_string(i) + ": ";
        for(int j = 0;j<to_print[i].size();j++)
        {
            ret += std::to_string(to_print[i][j]) + " ";
        }
        ret += "\n";
    }
    return ret;
}

int string_to_bitvector(string str)
{
    int ret = 0;
    int power = 1;
    for(int i = (int) str.size()-1;i>=0;i--)
    {
        ret+=(str[i]-'0')*power;
        power*=2;
    }
    return ret;
}
