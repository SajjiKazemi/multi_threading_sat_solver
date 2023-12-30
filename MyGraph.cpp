#include "MyGraph.h"
#include <vector>
#include <algorithm>
#include <memory>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"
#include "BFStree.h"

// Constructor definition
MyGraph::MyGraph()
{
    this->num_vertices = 0;
}

MyGraph::MyGraph(int num_vertices)
{
    this->num_vertices = num_vertices;
}

// Method definition
int MyGraph::getSize()
{
    return this->num_vertices;
}

void MyGraph::setEdges(std::map<int, std::pair<int,int>> edges)
{
    resetEverything();
    this->edges = edges;
}

void MyGraph::setNoVertices(int num_vertices)
{
    this->num_vertices = num_vertices;
    resetEverything();
}

bool MyGraph::checkNodeInEdges(int node)
{
    for (auto const& x : this->edges)
    {
        if(x.second.first == node || x.second.second == node)
        {
            return true;
        }
    }
    return false;
}

std::vector<int> MyGraph::getConnections(int node)
{
    std::vector<int> connections;
    for (auto const& x : this->edges)
    {
        if(x.second.first == node)
        {
            connections.push_back(x.second.second);
        }
        else if(x.second.second == node)
        {
            connections.push_back(x.second.first);
        }
    }
    std::sort(connections.begin(), connections.end());
    connections.erase(std::unique(connections.begin(), connections.end()), connections.end());
    return connections;
}

bool MyGraph::isConnected(int start, int end)
    {
        std::vector<int> connections = getConnections(start);    
            
        auto it = std::find(connections.begin(), connections.end(), end);
        if(it != connections.end())
            {
                return true;
            }
        else
            {
                return false;
            }
    }

void MyGraph::createRootTree(int start)
{
    BFStree root_tree = BFStree(start, this->getConnections(start));
    for (std::vector<int>::size_type i = 0; i < root_tree.getChildren().size(); i++)
    {
        root_tree.addGrandchild(BFStree(root_tree.getChildren()[i], this->getConnections(root_tree.getChildren()[i]), root_tree));            
    }
    this->unvisited_trees.push_back(root_tree);
}

bool MyGraph::checkTree(BFStree tree, int end)
{
    if(tree.isIncluded(end))
    {
        return true;
    }
    else
    {
        this->visited_nodes.push_back(tree.getRootNode());
        return false;
    }
}

void MyGraph::getUnvisitedTrees()
{   
    int controller = this->unvisited_trees.size();
    for(int i=0; i < controller; i++)
    {
        for(std::vector<int>::size_type j=0; j<this->unvisited_trees[i].getChildren().size(); j++)
        {
            if(std::find(this->visited_nodes.begin(), this->visited_nodes.end(), this->unvisited_trees[i].getChildren()[j]) == this->visited_nodes.end())
            {
                this->unvisited_trees.push_back(BFStree(this->unvisited_trees[i].getChildren()[j], this->getConnections(this->unvisited_trees[i].getChildren()[j]), this->unvisited_trees[i]));
            }
        }
    }
    this->unvisited_trees.erase(this->unvisited_trees.begin(), this->unvisited_trees.begin() + controller);

}

std::vector<int> MyGraph::findShortestPath(int start, int end)
{
    resetVisitedNodes();
    if (start == end && checkNodeInEdges(start) == true)
    {
        shortest_path.push_back(start);
        return shortest_path;
    }
    
    createRootTree(start);

    if(checkTree(this->unvisited_trees[0], end) == true)
    {
        shortest_path.push_back(start);
        shortest_path.push_back(end);
        return shortest_path;
    }
    else
    {
        bool found = false;
        while (found != true && this->unvisited_trees.size() != 0)
        {
            for (std::vector<int>::size_type i=0; i<unvisited_trees.size(); i++)
            {
                if(checkTree(unvisited_trees[i], end))
                {
                    found = true;
                    shortest_path = unvisited_trees[i].getAncestors();
                    shortest_path.push_back(unvisited_trees[i].getRootNode());
                    shortest_path.push_back(end);
                    break;
                }
            }
            getUnvisitedTrees();
        }
        if(shortest_path.empty())
        {
            std::cout << "Error: no path exists" << std::endl;
            return shortest_path;
        }
        else
        {
            return shortest_path;        
        }
    }
}

std::vector<int> MyGraph::getShortestPath()
{
    return this->shortest_path;
}

void MyGraph::resetVisitedNodes()
{
    this->unvisited_trees.clear();
    this->unvisited_nodes.clear();
    this->visited_nodes.clear();
    this->shortest_path.clear();
}

void MyGraph::resetEverything()
{
    this->edges.clear();
    this->unvisited_trees.clear();
    this->visited_nodes.clear();
    this->shortest_path.clear();
    this->triedToSetEdges = false;
    this->vertex_cover.clear();
}

void MyGraph::setTriedToSetEdges()
{
    this->triedToSetEdges = true;
}

std::vector<int> MyGraph::getEdgesVertices()
{
    std::vector<int> edges_vertices;
    for (auto const& x : this->edges)
    {
        edges_vertices.push_back(x.second.first);
        edges_vertices.push_back(x.second.second);
    }
    std::sort(edges_vertices.begin(), edges_vertices.end());
    edges_vertices.erase(std::unique(edges_vertices.begin(), edges_vertices.end()), edges_vertices.end());
    return edges_vertices;
}

void MyGraph::vertexCoverFirstCondition(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num, 
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver)
{
    Minisat::vec<Minisat::Lit> first_condition_clauses;
    for (size_t i=0; i<cols_num; i++)
    {
        for (size_t j=0; j<rows_num; j++)
        {
            first_condition_clauses.push(literals[j][i]);
        }
        solver->addClause(first_condition_clauses);
        first_condition_clauses.clear();
    }
}

void MyGraph::vertexCoverSecondCondition(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num, 
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver)
{
    for (size_t m=0; m<rows_num; m++)
    {
        for (size_t q=0; q<cols_num; q++)
        {
            for (size_t p=0; p<q; p++)
            {
                solver->addClause(~literals[m][p], ~literals[m][q]);
            }
        }
    }
}

void MyGraph::vertexCoverThirdCondition(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num, 
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver)
{
    for (size_t m=0; m<cols_num; m++)
    {
        for (size_t q=0; q<rows_num; q++)
        {
            for (size_t p=0; p<q; p++)
            {
                solver->addClause(~literals[p][m], ~literals[q][m]);   
            }
        }
    }
}

void MyGraph::vertexCoverFourthCondition(std::vector<std::vector<Minisat::Lit>> literals, size_t rows_num, size_t cols_num, 
                        MyGraph &graph, std::unique_ptr<Minisat::Solver>& solver)
{
    Minisat::vec<Minisat::Lit> fourth_condition_clauses;
    for (const auto& x : graph.edges)
    {
        int i = x.second.first;
        int j = x.second.second;
        for (size_t k=0; k<cols_num; k++)
        {
            fourth_condition_clauses.push(literals[i-1][k]);
            fourth_condition_clauses.push(literals[j-1][k]);
        }
        solver->addClause(fourth_condition_clauses);
        fourth_condition_clauses.clear();
    }
}

void MyGraph::getVertexCover()
{
    std::unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    std::vector<int> edges_vertices = getEdgesVertices();
    int edges_vertices_num = edges_vertices.size();

    for (int k = 1; k < edges_vertices_num; k++)
        {
            std::vector<std::vector<Minisat::Lit>> literals(edges_vertices_num, std::vector<Minisat::Lit>(k));
            for (int i = 0; i < edges_vertices_num; i++)
            {
                for (int j = 0; j < k; j++)
                {
                    literals[i][j] = Minisat::mkLit(solver->newVar());
                }
            }
            int rows_num = literals.size();
            int cols_num = literals[0].size();

            vertexCoverFirstCondition(literals, rows_num, cols_num, *this, solver);
            vertexCoverSecondCondition(literals, rows_num, cols_num, *this, solver);
            vertexCoverThirdCondition(literals, rows_num, cols_num, *this, solver);
            vertexCoverFourthCondition(literals, rows_num, cols_num, *this, solver);
            
            bool res = solver->solve();
            if (res == true)
                {
                    for (int i = 0; i < edges_vertices_num; i++)
                    {
                        for (int j = 0; j < k; j++)
                        {
                            if (Minisat::toInt(solver->modelValue(literals[i][j])) == 0)
                                this->vertex_cover.push_back(edges_vertices[i]);
                        }
                    }
                    return;
                }
            else
                {
                    solver.reset(new Minisat::Solver());
                }
        }

}

void MyGraph::printVertexCover()
{
    for (std::vector<int>::size_type i = 0; i < this->vertex_cover.size(); i++)
    {
        if (i == this->vertex_cover.size() - 1)
        {
            std::cout << this->vertex_cover[i] << std::endl;
            break;
        }
        else
        {
            std::cout << this->vertex_cover[i] << " ";
        }
    }
}