#include "treenode.h"
#include "customer.h" // Needed for print() and destructor

Treenode::Treenode(Customer* customer, bool isRoot, bool isLeaf, Treenode *parent, int m)
    : parent(parent), m(m), id(0), isRoot(isRoot), isLeaf(isLeaf) {
    if (customer) { // Allow creating a node without an initial customer
        customers.push_back(customer);
    }
}

Treenode::~Treenode() {
    // The BStarTree destructor handles deleting nodes,
    // but each node is responsible for the customers it holds.
    for (size_t i = 0; i < customers.size(); i++) {
        delete customers[i];
    }
    // Note: Child nodes are deleted by the BStarTree destructor, not here.
}

void Treenode::print() const {
    std::cout << "ID: " <<  id << " (Size: " << customers.size() << ")" << std::endl;
    for(size_t i = 0; i < customers.size(); i++) {
        customers[i]->print();
    }
    std::cout << std::endl;
}

int Treenode::getCurrSize() const {
    return customers.size();
}