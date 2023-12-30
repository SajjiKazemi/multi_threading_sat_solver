#include "helpers.h"
#include "MyGraph.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"


namespace line_parser{

    std::string get_command(std::string line, MyGraph &graph){

        std::regex vertice_define_pattern(R"(V (\d+))");
        std::regex edge_define_pattern(R"(E \{<\d+,\d+>(,<\d+,\d+>)*\})");
        std::regex shortest_path_pattern(R"(s (\d+) (\d+))");
        std::regex E_only_pattern(R"(E\s.*)");
        //std::regex s_only_pattern(R"(s\s.*)");
        std::string command;

        if(std::regex_match(line, vertice_define_pattern))
        {
            command = "V";
        }
        else if(std::regex_match(line, edge_define_pattern))
        {
            if (graph.getSize() == 0)
            {
                command = "Error";
                std::cout << "Error: no vertices defined" << std::endl;
            }
            else if (graph.getSize() != 0 && graph.edges.size() != 0)
            {
                command = "No Command";
            }
            else if (graph.getSize() != 0 && graph.edges.size() == 0)
            {
                if (graph.triedToSetEdges == true)
                {
                    command = "No Command";
                }
                else if (graph.triedToSetEdges == false)
                {
                    command = "E";
                }
            }
        }        
        else if(std::regex_match(line, shortest_path_pattern))
        {
            if (graph.edges.size() == 0)
            {
                command = "Error";
                std::cout << "Error: no edges defined" << std::endl;
            }
            else if (graph.edges.size() != 0)
            {
                command = "s";
            }
        }
        else if(std::regex_match(line, E_only_pattern))
        {
            if (graph.edges.size() !=0)
            {
                return "No command";        
            }
            else if (graph.edges.size() == 0)
            {
                graph.setTriedToSetEdges();
                std::cout << "Error: invalid command" << std::endl;
            }
            
        }
        return command;
    }    

    void analyze_command(std::string command, std::string line, MyGraph &graph)
    {
        std::vector<std::string> result;
        if(command == "V")
        {
            std::istringstream input(line);
            std::string prefix;
            int number;
            
            graph.resetEverything();
            input >> prefix >> number;
            graph.setNoVertices(number);
        }
        else if (command == "E")
        {
            std::map<int, std::pair<int, int>> edges;
            std::regex pattern(R"(<(\d+),(\d+)>)");
            std::sregex_iterator iter(line.begin(), line.end(), pattern);
            std::sregex_iterator end;
            int i = 0;
            while (iter != end) 
            {
                i++;
                auto match = *iter++;
                if (std::stoi(match[1]) > graph.getSize() || std::stoi(match[2]) > graph.getSize())
                {
                    std::cout << "Error: edge is not valid" << std::endl;
                    return;
                }
                edges[i] = std::make_pair(std::stoi(match[1]), std::stoi(match[2]));
            }
            graph.setEdges(edges);
        }
        else if (command == "s")
        {
            std::istringstream input(line);
            std::string prefix;
            int start;
            int end;

            input >> prefix >> start >> end;
            graph.findShortestPath(start, end);
            for(std::vector<int>::size_type i = 0; i < graph.getShortestPath().size(); i++)
            {
                if (i < graph.getShortestPath().size() - 1)
                {
                    std::cout << graph.getShortestPath()[i] << "-";
                }
                else
                {
                    std::cout << graph.getShortestPath()[i] << std::endl;
                }
            }
        }
        else if (command == "No Command" || command == "Error")
    {
        return;
    }
    }

}