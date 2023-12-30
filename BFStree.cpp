#include "BFStree.h"
#include <vector>
#include <algorithm>

BFStree::BFStree(int root_node, std::vector<int> children, BFStree ancestor)
{
    this->root_node = root_node;
    this->children = children;
    this->addAncestors(ancestor);
}

BFStree::BFStree(int root_node, std::vector<int> children)
{
    this->root_node = root_node;
    this->children = children;
}

int BFStree::getRootNode()
{
    return this->root_node;
}

std::vector<int> BFStree::getChildren()
{
    return this->children;
}

bool BFStree::isIncluded(int node)
{
    auto it = std::find(this->children.begin(), this->children.end(), node);
    if(it != this->children.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void BFStree::addGrandchild(BFStree child)
{
    this->grandchildren.push_back(child);
}

std::vector<int> BFStree::getAncestors()
{
    return this->ancestors;
}

void BFStree::addAncestors(BFStree ancestor)
{
    this->ancestors = ancestor.getAncestors();
    this->ancestors.push_back(ancestor.getRootNode());
}