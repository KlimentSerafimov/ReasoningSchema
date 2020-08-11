//
// Created by Kliment Serafimov on 8/10/20.
//

#ifndef COMPACTPOSET_CANVASANDBITTREEPROGRAM_H
#define COMPACTPOSET_CANVASANDBITTREEPROGRAM_H

#include <string>
#include <vector>
#include "BittreeNode.h"

using namespace std;

class PathAndRule
{
    vector<int> path;
    Rule rule;
public:
    PathAndRule() = default;
    PathAndRule(vector<int> _path, Rule _rule)
    {
        path = _path;
        rule = _rule;
    }
    string to_string()
    {
        string ret = rule_names[rule] + " ";
        for(int i = 0;i<path.size();i++)
        {
            ret += std::to_string(path[i]);
        }
        return ret;
    }
    Rule get_rule()
    {
        return rule;
    }
};

class AutomatonRule
{
protected:
    vector<PathAndRule> code;
public:
    void push_back(PathAndRule new_path_and_rule);
    AutomatonRule() = default;
    AutomatonRule(AutomatonRule * to_copy)
    {
        code = to_copy->code;
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

    int get_cost();
};


#endif //COMPACTPOSET_CANVASANDBITTREEPROGRAM_H
