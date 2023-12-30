#ifndef MYGRAPH_H
#define MYGRAPH_H

#include <memory>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include <map>
#include <iostream>
#include <vector>
#include "BFStree.h"

class MyGraph
{
private:
    int num_vertices;

public:
    std::map<int, std::pair<int, int>> edges;
    std::vector<BFStree> unvisited_trees;
    std::vector<int> visited_nodes;
    std::vector<int> unvisited_nodes;
    std::vector<int> shortest_path;
    std::vector<int> vertex_cover;
    bool triedToSetEdges = false;
    MyGraph();
    MyGraph(int num_vertices);
    void setNoVertices(int num_vertices);
    int getSize();
    void setSize(int num_vertices);
    void setEdges(std::map<int, std::pair<int,int>> edges);
    std::vector<int> getConnections(int node, bool repeat_check);
    bool isConnected(int start, int end);
    void createRootTree(int start);
    bool checkTree(BFStree tree, int end);
    std::vector<int> findShortestPath(int start, int end);
    std::vector<int> getShortestPath();
    void getUnvisitedTrees();
    void resetVisitedNodes();
    void resetEverything();
    void setTriedToSetEdges();
    bool checkNodeInEdges(int node);
    std::vector<int> getEdgesVertices();
    void CnfSatVcFirstCondition(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num, 
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver);
    void CnfSatVcSecondCondition(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num,
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver);
    void CnfSatVcThirdCondition(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num,
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver);
    void CnfSatVcFourthCondition(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num,
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver);
    void CnfSatVc();
    void approxCv1();
    void printVertexCover(bool thread=false);
};


#endif
