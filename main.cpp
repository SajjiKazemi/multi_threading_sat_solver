#include <iostream>
#include "helpers.h"
#include "MyGraph.h"

int main(int argc, char **argv) 
{
    MyGraph graph;
    while (!std::cin.eof())
    {
        std::string line;
        std::getline(std::cin, line);
        std::string command = line_parser::get_command(line, graph);
        line_parser::analyze_command(command, line, graph);
        if (graph.edges.size() > 0)
            {
                graph.getVertexCover();
                graph.printVertexCover();
                graph.resetEverything();
            }
        
    }
    
 return 0;
}
