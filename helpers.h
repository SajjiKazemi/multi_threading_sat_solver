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

namespace project_needs{
    int rand_num(int min, int max);
}

#endif