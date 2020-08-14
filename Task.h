//
// Created by Kliment Serafimov on 1/13/20.
//

#ifndef COMPACTPOSET_TASK_H
#define COMPACTPOSET_TASK_H

#include <string>
#include <cassert>

using namespace std;

class IncrementalTypeExpression;
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

    virtual void generate_bittree_task_expression(IncrementalTypeExpression * holder);

    virtual void solve(BittreeInputOutputType * holder);

};

class Task__sum : public Task
{
public:
    Task__sum() : Task("sum") {}
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__greater : public Task
{ public:
    Task__greater() : Task("greater") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__cumulative_binary_operator : public Task
{public:
    Task__cumulative_binary_operator() : Task("cumulative_binary_operator") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__bitwise_binary_operator : public Task
{public:
    Task__bitwise_binary_operator() : Task("bitwise_binary_operator") {}
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__one_shift_idx : public Task
{public:
    Task__one_shift_idx() : Task("one_shift_idx") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__count_unary : public Task
{public:
    Task__count_unary() : Task("count_unary") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__unary_sum : public Task
{public:
    Task__unary_sum() : Task("unary_sum") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__least_set_bit : public Task
{public:
    Task__least_set_bit() : Task("least_set_bit") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__strech_of_0s : public Task
{public:
    Task__strech_of_0s() : Task("strech_of_0s") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__strech_of_0s_w_state : public Task
{public:
    Task__strech_of_0s_w_state() : Task("strech_of_0s_w_state") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__linear_and_or_expression : public Task
{public:
    Task__linear_and_or_expression() : Task("linear_and_or_expression") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__linear_and_or_nand_nor_expression : public Task
{public:
    Task__linear_and_or_nand_nor_expression() : Task("linear_and_or_nand_nor_expression") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__sort_bits : public Task
{public:
    Task__sort_bits() : Task("sort_bits") { }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__biggest_square_with_kernel : public Task
{
    int param__w;
    int tile_w;
    int tile_h;
public:
    Task__biggest_square_with_kernel(int _param__w, int _tile_w, int _tile_h) : Task("biggest_square_with_kernel") {
        assert(_tile_w == 2 && _tile_h == 1);
        param__w = _param__w;
        tile_w = _tile_w;
        tile_h = _tile_h;
    }
    string get_task_name() override
    {
        return Task::get_task_name() + "(w=" + std::to_string(param__w) + ")";
    }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
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
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__sum_of_n_k_bit_integers_in_unary : public Task
{
    int param_k;
public:
    Task__sum_of_n_k_bit_integers_in_unary(int _param_k) : Task("list_sum_in_unary")
    {
        param_k = _param_k;
    }
    string get_task_name() override
    {
        return Task::get_task_name() + "(n=" + std::to_string(param_k) + ")";
    }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__sum_of_n_k_bit_integers : public Task
{
    int param_k;
public:
    Task__sum_of_n_k_bit_integers(int _param_k) : Task("list_sum")
    {
        param_k = _param_k;
    }
    string get_task_name() override
    {
        return Task::get_task_name() + "(n=" + std::to_string(param_k) + ")";
    }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__sum_of_n_k_bit_integers_with_intermediate_state : public Task
{
    int param_k;
public:
    Task__sum_of_n_k_bit_integers_with_intermediate_state(int _param_k) : Task("list_sum_w_inter")
    {
        param_k = _param_k;
    }
    string get_task_name() override
    {
        return Task::get_task_name() + "(n=" + std::to_string(param_k) + ")";
    }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};

class Task__sum_of_n_k_bit_integers_with_second_order_intermediate_state : public Task
{
    int param_k;
public:
    Task__sum_of_n_k_bit_integers_with_second_order_intermediate_state(int _param_k) : Task("list_sum_w_inter_w_inter")
    {
        param_k = _param_k;
    }
    string get_task_name() override
    {
        return Task::get_task_name() + "(n=" + std::to_string(param_k) + ")";
    }
    void generate_bittree_task_expression(IncrementalTypeExpression* holder) override;
    void solve(BittreeInputOutputType* holder) override;
};
#endif //COMPACTPOSET_TASK_H

