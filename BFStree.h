#ifndef BFSTREE_H
#define BFSTREE_H

#include <vector>

class BFStree
{
private:
    int root_node;
    std::vector<int> children;
    std::vector<BFStree> grandchildren;
    std::vector<int> ancestors;

public:
    BFStree(int root_node, std::vector<int> children);
    BFStree(int root_node, std::vector<int> children, BFStree ancestor);
    int getRootNode();
    std::vector<int> getChildren();
    bool isIncluded(int node);
    void addGrandchild(BFStree child);
    void addAncestors(BFStree ancestor);
    std::vector<int> getAncestors();
};

#endif