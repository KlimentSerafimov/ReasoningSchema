//
// Created by Kliment Serafimov on 8/10/20.
//

#ifndef COMPACTPOSET_CANVASANDBITTREEPROGRAM_H
#define COMPACTPOSET_CANVASANDBITTREEPROGRAM_H

#include <string>
#include <vector>
#include "BittreeNode.h"
#include "AutomatonRuleCost.h"
#include "PrimitiveRule.h"

using namespace std;

class PrimitiveRuleContext
{
public:
    int child_id = -1;
    int grandchild_id = -1;
    int greatgrandchild_id = -1;
    PrimitiveRuleContext() = default;
    PrimitiveRuleContext(int _child_id, int _grandchild_id, int _greatgrandchild_id)
    {
        child_id = _child_id;
        grandchild_id = _grandchild_id;
        greatgrandchild_id = _greatgrandchild_id;
    }
};

class SequenceOfPrimitiveRules: public vector<PrimitiveRule> {
public:
    SequenceOfPrimitiveRules() = default;
    SequenceOfPrimitiveRules(PrimitiveRule rule);
    SequenceOfPrimitiveRules(PrimitiveRule rule, PrimitiveRule rule1);
    SequenceOfPrimitiveRules(PrimitiveRule rule, PrimitiveRule rule1, PrimitiveRule rule2);
    string to_string()
    {
        string ret;
        for(int i = 0;i<size();i++)
        {
            ret += rule_names[at(i)] + " ";
        }
        return ret;
    }
    static AutomatonRuleCost get_cost()
    {
        return 1;
    }
};

class PathAndSequenceOfRules
{
    vector<int> path;
    SequenceOfPrimitiveRules sequence_of_rules;
    AutomatonRuleCost cost;
public:
    PathAndSequenceOfRules() = default;
    PathAndSequenceOfRules(vector<int> _path, SequenceOfPrimitiveRules _sequence_of_rules)
    {
        path = _path;
        sequence_of_rules = _sequence_of_rules;
        cost.add_cost(AutomatonRuleCost(_sequence_of_rules.get_cost().get_cost() << (int)_path.size()));
    }
    string to_string()
    {
        string ret = "[";
        if(sequence_of_rules.size()>=2)
        {
            ret += "(";
            for(int i = 0;i<sequence_of_rules.size();i++)
            {
                if(i!= 0)
                {
                    ret += " ";
                }
                ret += rule_names[sequence_of_rules[i]];
            }
            ret += ")";
        }
        else
        {
            assert(sequence_of_rules.size() == 1);
            ret += rule_names[sequence_of_rules[0]] ;
        }
        for(int i = 0;i<path.size();i++)
        {
            ret += " ";
            ret += std::to_string(path[i]);
        }
        ret += "]";
        return ret;
    }
    AutomatonRuleCost get_cost()
    {
        return cost;
    }
};


class AutomatonRule
{
protected:
    vector<PathAndSequenceOfRules> code;
    AutomatonRuleCost cost;
public:
    void push_back(PathAndSequenceOfRules new_path_and_rule);
    AutomatonRule() = default;
    AutomatonRule(AutomatonRule * to_copy)
    {
        code = to_copy->code;
        cost = to_copy->cost;
    }
    virtual string to_string()
    {
        string ret;
        for(int i = 0;i<code.size();i++)
        {
            ret += code[i].to_string() + " ";
        }
        return ret;
    }
    bool operator < (const AutomatonRule& other) const
    {
        return cost < other.cost;
    }
};

class CanvasAndBittreeProgram : public AutomatonRule
{
public:
    BittreeNode* canvas;
    CanvasAndBittreeProgram(BittreeNode* _canvas)
    {
        canvas = _canvas;
    }
    CanvasAndBittreeProgram(CanvasAndBittreeProgram* to_copy, SequenceOfPrimitiveRules rule, vector<int> * path);
    BittreeNode* get_canvas()
    {
        return canvas;
    }

    CanvasAndBittreeProgram * produce(SequenceOfPrimitiveRules rule, vector<int> * path);

    string to_string() override;

    AutomatonRuleCost get_cost() const;
};


#endif //COMPACTPOSET_CANVASANDBITTREEPROGRAM_H
