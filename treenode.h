#ifndef TREENODE_H
#define TREENODE_H

#include <vector>
#include <iostream>

// Forward declarations to avoid circular includes
class Customer;
class BStarTree;

class Treenode {
public:
    Treenode(Customer* customer, bool isRoot, bool isLeaf, Treenode *parent, int m);
    ~Treenode();

    void print() const;
    int getCurrSize() const;

private:
    // Treenode is an internal implementation detail of BStarTree.
    // We make BStarTree a friend to allow it to manipulate
    // the node's internal vectors and properties directly.
    friend class BStarTree;

    std::vector<Treenode*> children;
    std::vector<Customer*> customers;
    Treenode* parent;

    int m;
    int id; // Used for printing
    bool isRoot;
    bool isLeaf;
};

#endif // TREENODE_H