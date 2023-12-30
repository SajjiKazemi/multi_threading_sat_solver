/**
 * An example unit test using doctest. See documentation at
 * https://github.com/onqtam/doctest/blob/master/doc/markdown/tutorial.md
 */
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "helpers.h"
#include "MyGraph.h"
#include "BFStree.h"
#include <iostream>
#include <algorithm>
#include <vector>


TEST_CASE("Successful Test Example") {
    MyGraph graph;
    std::string line = "V 5";
    std::string command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    line = "E {<1,3>,<1,2>,<1,4>,<4,3>,<2,4>}";
    command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    std::vector<int> edges_vertices = graph.getEdgesVertices();
    CHECK(edges_vertices.size() == 4);
    CHECK(edges_vertices[0] == 1);
    CHECK(edges_vertices[1] == 2);
    CHECK(edges_vertices[2] == 3);
    CHECK(edges_vertices[3] == 4);
}

TEST_CASE("Test one case")
{
    MyGraph graph;
    std::string line = "V 3";
    std::string command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    line = "E {<1,2>,<1,3>}";
    command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    std::ostringstream output;
    std::streambuf* oldCout = std::cout.rdbuf(output.rdbuf());
    graph.getVertexCover();
    graph.printVertexCover();
    std::cout.rdbuf(oldCout);
    CHECK(output.str() == "1\n");

}

TEST_CASE("Test the HW case")
{
    MyGraph graph;
    std::string line = "V 5";
    std::string command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    line = "E {<1,5>,<5,2>,<1,4>,<4,5>,<4,3>,<2,4>}";
    command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    std::ostringstream output;
    std::streambuf* oldCout = std::cout.rdbuf(output.rdbuf());
    graph.getVertexCover();
    graph.printVertexCover();
    std::cout.rdbuf(oldCout);
    CHECK(output.str() == "4 5\n");
}

TEST_CASE("Both of the above together")
{
    MyGraph graph;
    std::string line = "V 3";
    std::string command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    line = "E {<1,2>,<1,3>}";
    command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    std::ostringstream output;
    std::streambuf* oldCout = std::cout.rdbuf(output.rdbuf());
    graph.getVertexCover();
    graph.printVertexCover();
    std::cout.rdbuf(oldCout);
    CHECK(output.str() == "1\n");
    line = "V 5";
    command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    line = "E {<1,5>,<5,2>,<1,4>,<4,5>,<4,3>,<2,4>}";
    command = line_parser::get_command(line, graph);
    line_parser::analyze_command(command, line, graph);
    std::ostringstream output2;
    std::streambuf* oldCout2 = std::cout.rdbuf(output2.rdbuf());
    graph.getVertexCover();
    graph.printVertexCover();
    std::cout.rdbuf(oldCout2);
    CHECK(output2.str() == "4 5\n");
}

