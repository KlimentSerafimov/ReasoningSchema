//
// Created by Kliment Serafimov on 8/10/20.
//

#ifndef COMPACTPOSET_CANVASANDBITTREEPROGRAM_H
#define COMPACTPOSET_CANVASANDBITTREEPROGRAM_H

#include <string>
#include <vector>
#include "BittreeNode.h"
#include "AutomatonRuleCost.h"

using namespace std;

class PathAndRule
{
    vector<int> path;
    Rule rule;
    AutomatonRuleCost cost;
public:
    PathAndRule() = default;
    PathAndRule(vector<int> _path, Rule _rule)
    {
        path = _path;
        rule = _rule;
        cost = AutomatonRuleCost(rule_cost[rule]*(int)path.size());
    }
    string to_string()
    {
        string ret = rule_names[rule] + " ";
        for(int i = 0;i<path.size();i++)
        {
            ret += std::to_string(path[i]) + " ";
        }
        return ret;
    }
    Rule get_rule() const {
        return rule;
    }
    AutomatonRuleCost get_cost()
    {
        return cost;
    }
};


class AutomatonRule
{
protected:
    vector<PathAndRule> code;
    AutomatonRuleCost cost;
public:
    void push_back(PathAndRule new_path_and_rule);
    AutomatonRule() = default;
    AutomatonRule(AutomatonRule * to_copy)
    {
        code = to_copy->code;
        cost = to_copy->cost;
    }
    virtual string to_string()
    {
        string ret;
        ret += "code = ";
        for(int i = 0;i<code.size();i++)
        {
            ret += code[i].to_string() + " ";
        }
        return ret;
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
    CanvasAndBittreeProgram(CanvasAndBittreeProgram* to_copy, Rule rule, vector<int> * path);
    BittreeNode* get_canvas()
    {
        return canvas;
    }

    CanvasAndBittreeProgram * produce(Rule rule, vector<int> * path);

    string to_string() override;

    AutomatonRuleCost get_cost() const;
};


#endif //COMPACTPOSET_CANVASANDBITTREEPROGRAM_H
