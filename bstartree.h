#ifndef BSTARTREE_H
#define BSTARTREE_H

#include <string>
#include <vector>

// Forward declarations
class Customer;
class Treenode;

class BStarTree {
public:
    BStarTree(const std::string& filename, int m);
    ~BStarTree();

    void insert(Customer *c);
    bool deleteCustomer(long long key); // Declared but not implemented in original
    void print();
    Customer* search(long long key);
    void searchMulKeys(std::vector<long long> keys);
    std::vector<Customer*> searchByName(std::string name);

private:
    // Internal helper methods
    Treenode *searchNode(long long key);
    void sortCustomers(std::vector<Customer*> &customers);
    void splitRoot();
    void twoThreeSplit(Treenode *curr);
    bool canOverflow(Treenode *curr);
    void overflowNode(Treenode *curr);
    void insertNonFull(Treenode *curr, Customer* c);
    void makeOutputFile(std::vector<Customer*> customers);

    // Member variables
    int m;
    Treenode *root;
};

#endif // BSTARTREE_H