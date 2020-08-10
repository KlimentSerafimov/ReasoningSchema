//
// Created by Kliment Serafimov on 1/13/20.
//

#ifndef COMPACTPOSET_TASK_H
#define COMPACTPOSET_TASK_H

#include <string>
#include <cassert>

using namespace std;

class BittreeTypeExpression;
class BittreeInputOutputType;


class Task
{
public:

    string task_name;

    Task() = default;

    Task(string _task_name)
    {
        task_name = _task_name;
    }

    virtual string get_task_name()
    {
        return task_name;
    }

    virtual void generate_bittree_task_expression(BittreeTypeExpression * holder);

    virtual void solve(BittreeInputOutputType * holder);

};

class Task__sum : public Task
{
public:
    Task__sum() : Task("sum") {}
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__greater : public Task
{ public:
    Task__greater() : Task("greater") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__cumulative_binary_operator : public Task
{public:
    Task__cumulative_binary_operator() : Task("cumulative_binary_operator") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__bitwise_binary_operator : public Task
{public:
    Task__bitwise_binary_operator() : Task("bitwise_binary_operator") {}
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__one_shift_idx : public Task
{public:
    Task__one_shift_idx() : Task("one_shift_idx") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__multiply_by : public Task
{
    int multiply_by;
public:
    Task__multiply_by(int _multiply_by) : Task() {
        task_name = "multiply_by";
        multiply_by = _multiply_by;
    }
    string get_task_name() override
    {
        return Task::get_task_name() + "(mul=" + std::to_string(multiply_by) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__one_shift_idx__rev : public Task
{
    int param__init_size;
public:
    Task__one_shift_idx__rev(int _param__init_size) : Task("one_shift_idx__rev") { param__init_size = _param__init_size; }

    string get_task_name() override
    {
        return Task::get_task_name() + "(init_sz=" + std::to_string(param__init_size) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__count_unary : public Task
{public:
    Task__count_unary() : Task("count_unary") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__count_unary__rev : public Task
{
    int param__init_size;
public:
    Task__count_unary__rev(int _param__init_size) : Task("count_unary__rev") { param__init_size = _param__init_size ; }
    string get_task_name() override
    {
        return Task::get_task_name() + "(init_sz=" + std::to_string(param__init_size) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__unary_sum : public Task
{public:
    Task__unary_sum() : Task("unary_sum") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__least_set_bit : public Task
{public:
    Task__least_set_bit() : Task("least_set_bit") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__strech_of_0s : public Task
{public:
    Task__strech_of_0s() : Task("strech_of_0s") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__strech_of_0s_w_state : public Task
{public:
    Task__strech_of_0s_w_state() : Task("strech_of_0s_w_state") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__linear_and_or_expression : public Task
{public:
    Task__linear_and_or_expression() : Task("linear_and_or_expression") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__linear_and_or_nand_nor_expression : public Task
{public:
    Task__linear_and_or_nand_nor_expression() : Task("linear_and_or_nand_nor_expression") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__sort_bits : public Task
{public:
    Task__sort_bits() : Task("sort_bits") { }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__add : public Task
{
    int add_num;
public:
    Task__add(int _add_num) : Task("add") { add_num = _add_num; }
    string get_task_name() override
    {
        return Task::get_task_name() + "(num=" + std::to_string(add_num) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__gene_network : public Task
{
    int param__network;
public:
    Task__gene_network(int _param__network) : Task("gene_network") { param__network = _param__network; }
    string get_task_name() override
    {
        return Task::get_task_name() + "(net_id=" + std::to_string(param__network) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__biggest_square : public Task
{
    int param__w;
public:
    Task__biggest_square(int _param__w) : Task("biggest_square") { param__w = _param__w;}
    string get_task_name() override
    {
        return Task::get_task_name() + "(w=" + std::to_string(param__w) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__biggest_square_with_kernel : public Task
{
    int param__w;
    int tile_w;
    int tile_h;
public:
    Task__biggest_square_with_kernel(int _param__w, int _tile_w, int _tile_h) : Task("biggest_square_with_kernel") {
        assert(tile_w == 2 && tile_h == 1);
        param__w = _param__w;
        tile_w = _tile_w;
        tile_h = _tile_h;
    }
    string get_task_name() override
    {
        return Task::get_task_name() + "(w=" + std::to_string(param__w) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__biggest_square_w_corners_as_intermediate_state : public Task
{
    int param__w;
public:
    Task__biggest_square_w_corners_as_intermediate_state(int _param__w) : Task("biggest_square_w_inter_corners") { param__w = _param__w;}
    string get_task_name() override
    {
        return Task::get_task_name() + "(w=" + std::to_string(param__w) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__biggest_square_as_corners : public Task
{
    int param__w;
public:
    Task__biggest_square_as_corners(int _param__w) : Task("biggest_square_to_corners") { param__w = _param__w;}
    string get_task_name() override
    {
        return Task::get_task_name() + "(w=" + std::to_string(param__w) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__remove_points : public Task
{
    int param__w;
public:
    Task__remove_points(int _param__w) : Task("remove_points") { param__w = _param__w;}
    string get_task_name() override
    {
        return Task::get_task_name() + "(w=" + std::to_string(param__w) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__remove_points_and_peninsula : public Task
{
    int param__w;
public:
    Task__remove_points_and_peninsula(int _param__w) : Task("remove_points_and_peninsula") { param__w = _param__w;}
    string get_task_name() override
    {
        return Task::get_task_name() + "(w=" + std::to_string(param__w) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__game_of_life : public Task
{
    int param__w;
public:
    Task__game_of_life(int _param__w) : Task("game_of_life") { param__w = _param__w;}
    string get_task_name() override
    {
        return Task::get_task_name() + "(w=" + std::to_string(param__w) + ")";
    }
    void generate_bittree_task_expression(BittreeTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};



#endif //COMPACTPOSET_TASK_H


//FOSSILS:


/*
static const string str_task_name__sum = "sum";
static const string str_task_name__greater = "greater";
static const string str_task_name__cumulative_binary_operator = "cumulative_binary_operator";
static const string str_task_name__bitwise_binary_operator = "bitwise_binary_operator";
static const string str_task_name__one_shift_idx = "one_shift_idx";
static const string str_task_name__multiply_by = "multiply_by";
static const string str_task_name__one_shift_idx__reverse_subtask = "one_shift_idx__rev";
static const string str_task_name__count_unary = "count_unary";
static const string str_task_name__count_unary__reverse_subtask = "count_unary__rev";
static const string str_task_name__unary_sum = "unary_sum";
static const string str_task_name__least_set_bit = "least_set_bit";
static const string str_task_name__max_window_between_bits = "strech_of_0s";
static const string str_task_name__max_window_between_bits_with_state = "strech_of_0s_w_state";
static const string str_task_name__linear_and_or_expression = "linear_and_or_expression";
static const string str_task_name__linear_and_or_nand_nor_expression = "linear_and_or_nand_nor_expression";
static const string str_task_name__sort_bits = "sort_bits";
static const string str_task_name__add = "add";
static const string str_task_name__gene_network = "gene_network";
static const string str_task_name__biggest_square = "biggest_square";
static const string str_task_name__biggest_square_with_kernel = "biggest_square_with_kernel";
*/

//enum TaskNames {
//    sum,
//    greater,
//    cumulative_binary_operator,
//    bitwise_binary_operator,
//    one_shift_idx,
//    multiply_by,
//    one_shift_idx__rev,
//    count_unary,
//    count_unary__rev,
//    unary_sum,
//    least_set_bit,
//    strech_of_0s,
//    strech_of_0s_w_state,
//    linear_and_or_expression,
//    linear_and_or_nand_nor_expression,
//    sort_bits,
//    add,
//    gene_network,
//    biggest_square,
//    biggest_square_with_kernel
//    };

//enum TaskNames {
//sum
//greater
//cumulative_binary_operator
//bitwise_binary_operator
//one_shift_idx
//multiply_by
//one_shift_idx__rev
//count_unary
//count_unary__rev
//unary_sum
//least_set_bit
//strech_of_0s
//strech_of_0s_w_state
//linear_and_or_expression
//linear_and_or_nand_nor_expression
//sort_bits
//add
//gene_network
//biggest_square
//biggest_square_with_kernel
//};


/*bool do__sum = false;
bool do__greater = false;
bool do__cummulative_binary_operator = false;
bool do__bitwise_binary_operator = false;
bool do__one_shift_idx = false;
bool do__count_unary = false;
bool do__unary_sum = false;
bool do__least_set_bit = false;
bool do__max_window_between_bits = false;
bool do__max_window_between_bits_with_state = false;
bool do__linear_and_or_expression = false;
bool do__linear_and_or_nand_nor_expression = false;
bool do__sort_bits = false;

bool do__multiply_by = false;
int param__multiply_by = -1;

bool do__add = false;
int param__add_by = -1;

bool do__one_shift_idx__reverse_subtask = false;
bool do__count_unary__reverse_subtask = false;

int param__init_size = -1;

bool do__gene_network = false;
int param__network = -1;

bool do__biggest_square = false;
int param__w = -1;

bool do__biggest_square_with_kernel = false;

bool num_iter_defined = false;
int num_iter = -1;*/



/*Task(string _task_name) {

    assert(_task_name != str_task_name__multiply_by);
    assert(_task_name != str_task_name__one_shift_idx__reverse_subtask);
    assert(_task_name != str_task_name__count_unary__reverse_subtask);
    assert(_task_name != str_task_name__add);
    assert(_task_name != str_task_name__gene_network);
    assert(_task_name != str_task_name__biggest_square);
    assert(_task_name != str_task_name__biggest_square_with_kernel);

    task_name = _task_name;

    do__sum = task_name == str_task_name__sum;
    do__greater = task_name == str_task_name__greater;
    do__cummulative_binary_operator = task_name == str_task_name__cumulative_binary_operator;
    do__bitwise_binary_operator = task_name == str_task_name__bitwise_binary_operator;
    do__one_shift_idx = task_name == str_task_name__one_shift_idx;
    do__count_unary = task_name == str_task_name__count_unary;
    do__unary_sum = task_name == str_task_name__unary_sum;
    do__least_set_bit = task_name == str_task_name__least_set_bit;
    do__max_window_between_bits = task_name == str_task_name__max_window_between_bits;
    do__max_window_between_bits_with_state = task_name == str_task_name__max_window_between_bits_with_state;
    do__linear_and_or_expression = task_name == str_task_name__linear_and_or_expression;
    do__linear_and_or_nand_nor_expression = task_name == str_task_name__linear_and_or_nand_nor_expression;
    do__sort_bits = task_name == str_task_name__sort_bits;
}

Task(string _task_name, int _param) {
    assert(
            _task_name == str_task_name__multiply_by ||
            _task_name == str_task_name__one_shift_idx__reverse_subtask ||
            _task_name == str_task_name__count_unary__reverse_subtask ||
            _task_name == str_task_name__add ||
            _task_name == str_task_name__gene_network ||
            _task_name == str_task_name__biggest_square
    );
    task_name = _task_name;

    do__multiply_by = task_name == str_task_name__multiply_by;
    do__one_shift_idx__reverse_subtask = task_name == str_task_name__one_shift_idx__reverse_subtask;
    do__count_unary__reverse_subtask = task_name == str_task_name__count_unary__reverse_subtask;
    do__add = task_name == str_task_name__add;
    do__gene_network = task_name == str_task_name__gene_network;
    do__biggest_square = task_name == str_task_name__biggest_square;

    if(do__multiply_by)
    {
        param__multiply_by = _param;
    }
    else if(do__one_shift_idx__reverse_subtask || do__count_unary__reverse_subtask)
    {
        param__init_size = _param;
        num_iter_defined = true;
        num_iter = param__init_size;
    }
    else if(do__add)
    {
        param__add_by = _param;
    }
    else if(do__gene_network)
    {
        param__network = _param;
    }
    else if(do__biggest_square)
    {
        param__w = _param;
    }
    else
    {
        assert(false);
    }
}


Task(string _task_name, int param0, int param1, int param2) {
    assert(_task_name == str_task_name__biggest_square_with_kernel);
}

string get_task_name() override
{
    if(do__multiply_by)
    {
        return task_name + "(num=" + std::to_string(param__multiply_by) + ")";
    }
    else if(do__one_shift_idx__reverse_subtask || do__count_unary__reverse_subtask)
    {
        return task_name + "(init_size=" + std::to_string(param__init_size) + ")";
    }
    else if(do__add)
    {
        return task_name + "(num=" + std::to_string(param__add_by) + ")";
    }
    else if(do__gene_network)
    {
        return task_name + "(network=" + std::to_string(param__network) + ")";
    }
    else{
        return task_name;
    }
}*/