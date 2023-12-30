#ifndef HELPERS_H
#define HELPERS_H

#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include "MyGraph.h"
// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"

namespace line_parser {
    // get the command from a line
    std::string get_command(std::string line, MyGraph &graph);
    // split a string into a vector of strings
    void analyze_command(std::string command, std::string line, MyGraph &graph);
}

namespace polytime_reduction 
{
    // reduce the time of the graph
    void apply_first_cond(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num, 
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver);
    void apply_second_cond(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num, 
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver);
    void apply_third_cond(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num, 
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver);
    void apply_fourth_cond(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num, 
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver);
    void reduce_polytime(MyGraph &graph);
    std::vector<int> get_edges_vertices(MyGraph &graph);
}

#endif