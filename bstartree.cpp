#include "bstartree.h"
#include "treenode.h"
#include "customer.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <algorithm> // For std::sort
#include <cmath>     // For std::floor

// Using namespace std is fine within a .cpp file
using namespace std;

BStarTree::BStarTree(const string& filename, int m): m(m), root(nullptr){
    vector<Customer*> customers = Customer::read_from_file(filename);
    for (size_t i = 0; i < customers.size(); i++) {
        insert(customers[i]);
    }
}

BStarTree::~BStarTree() {
    if (!root) return;

    queue<Treenode*> q;
    q.push(root);
    while (!q.empty()) {
        Treenode* next = q.front();
        q.pop();
        for (size_t i = 0; i < next->children.size(); i++) {
            // Children are only in non-leaf nodes
            q.push(next->children[i]);
        }
        // Deleting the node will trigger its destructor,
        // which deletes the customers it holds.
        delete next;
    }
}

Treenode *BStarTree::searchNode(long long key) {
    Treenode *curr = root;
    while (!curr->isLeaf) {
        bool isLast = false;
        int i = 0;
        for(i = 0; i < curr->getCurrSize(); i++) {
            // Use getter
            if(key < curr->customers[i]->getKey()) {
                curr = curr->children[i];
                break;
            }
        }
        // If loop finished without breaking, key is >= all customers, go to last child
        if(i == curr->getCurrSize()) {
             curr = curr->children[curr->getCurrSize()];
        }
    }
    return curr;
}

void BStarTree::sortCustomers(vector<Customer*> &customers) {
    sort(customers.begin(), customers.end(), [] (Customer* a, Customer* b) {
            // Use getter
            return a->getKey() < b->getKey();
        });
}

void BStarTree::splitRoot() {
    int mid = root->customers.size() / 2;
    Customer* midCustomer = root->customers[mid];

    // Create new root with the middle customer
    Treenode *newRoot = new Treenode(midCustomer, true, false, nullptr, m);
    
    Treenode* leftChild;
    Treenode* rightChild;

    // Create new children
    if (root->isLeaf) {
        leftChild = new Treenode(nullptr, false, true, newRoot, m);
        rightChild = new Treenode(nullptr, false, true, newRoot, m);
    } else {
        leftChild = new Treenode(nullptr, false, false, newRoot, m);
        rightChild = new Treenode(nullptr, false, false, newRoot, m);
    }

    // Assign customers to children
    leftChild->customers.assign(root->customers.begin(), root->customers.begin() + mid);
    rightChild->customers.assign(root->customers.begin() + mid + 1, root->customers.end());
    
    // Clear the customer vectors we just created
    leftChild->customers.clear(); 
    rightChild->customers.clear();
    
    // Re-assign customers using vector's range constructor
    leftChild->customers.assign(root->customers.begin(), root->customers.begin() + mid);
    rightChild->customers.assign(root->customers.begin() + mid + 1, root->customers.end());


    // Assign children (if old root was not a leaf)
    if (!root->isLeaf) {
        leftChild->children.assign(root->children.begin(), root->children.begin() + mid + 1);
        for (Treenode* child : leftChild->children) {
            child->parent = leftChild;
        }

        rightChild->children.assign(root->children.begin() + mid + 1, root->children.end());
        for (Treenode* child : rightChild->children) {
            child->parent = rightChild;
        }
    }

    // Link new children to new root
    newRoot->children.push_back(leftChild);
    newRoot->children.push_back(rightChild);
    
    // Delete old root (its customers are now owned by new nodes)
    root->customers.clear(); // Avoid double-delete
    root->children.clear(); // Avoid dangling pointers
    delete root;
    
    root = newRoot;
}

void BStarTree::twoThreeSplit(Treenode *curr) {
    Treenode *left = nullptr, *right = nullptr, *parent = curr->parent;
    const int i1 = floor((2.0 * m - 2.0) / 3.0);
    const int i2 = i1 + floor((2.0 * m - 1.0) / 3.0) + 1;

    int index = 0;
    for (size_t i = 0; i < parent->children.size(); i++ ) {
        if (parent->children[i] == curr) {
            index = i;
            break;
        }
    }
    if (index  < parent->children.size() - 1) right = parent->children[index + 1];
    if (index > 0) left = parent->children[index - 1];

    // Try splitting with right sibling first
    if (right && (right->getCurrSize() + curr->getCurrSize() < 2 * m - 2)) {
        vector<Customer*> allCustomers;
        vector<Treenode*> allChildren;

        // Collect all customers and children from curr, parent[index], and right
        allCustomers.insert(allCustomers.end(), curr->customers.begin(), curr->customers.end());
        allCustomers.push_back(parent->customers[index]);
        allCustomers.insert(allCustomers.end(), right->customers.begin(), right->customers.end());
        
        allChildren.insert(allChildren.end(), curr->children.begin(), curr->children.end());
        allChildren.insert(allChildren.end(), right->children.begin(), right->children.end());

        // Create a new third node
        Treenode *third = new Treenode(nullptr, false, curr->isLeaf, parent, m);

        // Update parent keys
        parent->customers[index] = allCustomers[i1];
        parent->customers.insert(parent->customers.begin() + index + 1, allCustomers[i2]);

        // Redistribute customers
        curr->customers.assign(allCustomers.begin(), allCustomers.begin() + i1);
        right->customers.assign(allCustomers.begin() + i1 + 1, allCustomers.begin() + i2);
        third->customers.assign(allCustomers.begin() + i2 + 1, allCustomers.end());

        // Redistribute children if not leaf
        if (!curr->isLeaf) {
            int c1 = i1 + 1;
            int c2 = i2 + 1;
            curr->children.assign(allChildren.begin(), allChildren.begin() + c1);
            right->children.assign(allChildren.begin() + c1, allChildren.begin() + c2);
            third->children.assign(allChildren.begin() + c2, allChildren.end());

            for(Treenode* child : curr->children) child->parent = curr;
            for(Treenode* child : right->children) child->parent = right;
            for(Treenode* child : third->children) child->parent = third;
        }
        
        // Insert new node into parent's children
        parent->children.insert(parent->children.begin() + index + 2, third);
    }
    // Else, split with left sibling
    else if (left && (left->getCurrSize() + curr->getCurrSize() < 2 * m - 2)) {
        vector<Customer*> allCustomers;
        vector<Treenode*> allChildren;
        
        // Collect all customers and children from left, parent[index-1], and curr
        allCustomers.insert(allCustomers.end(), left->customers.begin(), left->customers.end());
        allCustomers.push_back(parent->customers[index - 1]);
        allCustomers.insert(allCustomers.end(), curr->customers.begin(), curr->customers.end());

        allChildren.insert(allChildren.end(), left->children.begin(), left->children.end());
        allChildren.insert(allChildren.end(), curr->children.begin(), curr->children.end());
        
        // Create a new third node
        Treenode *third = new Treenode(nullptr, false, curr->isLeaf, parent, m);

        // Update parent keys
        parent->customers[index - 1] = allCustomers[i1];
        parent->customers.insert(parent->customers.begin() + index, allCustomers[i2]);
        
        // Redistribute customers
        left->customers.assign(allCustomers.begin(), allCustomers.begin() + i1);
        curr->customers.assign(allCustomers.begin() + i1 + 1, allCustomers.begin() + i2);
        third->customers.assign(allCustomers.begin() + i2 + 1, allCustomers.end());

        // Redistribute children if not leaf
        if (!curr->isLeaf) {
            int c1 = i1 + 1;
            int c2 = i2 + 1;
            left->children.assign(allChildren.begin(), allChildren.begin() + c1);
            curr->children.assign(allChildren.begin() + c1, allChildren.begin() + c2);
            third->children.assign(allChildren.begin() + c2, allChildren.end());

            for(Treenode* child : left->children) child->parent = left;
            for(Treenode* child : curr->children) child->parent = curr;
            for(Treenode* child : third->children) child->parent = third;
        }

        // Insert new node into parent's children
        parent->children.insert(parent->children.begin() + index + 1, third);
    }
}


bool BStarTree::canOverflow(Treenode *curr) {
    Treenode *p = curr->parent;
    int currIdx = 0;

    for (size_t i = 0; i < p->children.size(); i++) {
        if (p->children[i] == curr) {
            currIdx = i;
            break;
        }
    }

    // Check right sibling
    if (currIdx < p->children.size() - 1) {
        Treenode* rightBrother = p->children[currIdx + 1];
        if(rightBrother->getCurrSize() < m - 1) {
            return true;
        }
    }
    // Check left sibling
    if (currIdx > 0){
        Treenode* leftBrother = p->children[currIdx - 1];
        if(leftBrother->getCurrSize() < m - 1) {
            return true;
        }
    }
    return false;
}

void BStarTree::overflowNode(Treenode *curr) {
    Treenode *left = nullptr, *right = nullptr, *parent = curr->parent;
    int index = 0;
    for (size_t i = 0; i < curr->parent->children.size(); i++ ) {
        if (curr->parent->children[i] == curr) {
            index = i;
            break;
        }
    }
    if (index < parent->children.size() - 1) right = parent->children[index + 1];
    if (index > 0) left = parent->children[index - 1];


    // Try overflowing to right sibling first
    if (right && right->getCurrSize() < m - 1) {
        // Move parent's key down to right sibling
        right->customers.insert(right->customers.begin(), parent->customers[index]);
        
        // Move last key from curr up to parent
        parent->customers[index] = curr->customers.back();
        curr->customers.pop_back();

        // Move last child from curr to right sibling
        if (!curr->isLeaf) {
            right->children.insert(right->children.begin(), curr->children.back());
            curr->children.back()->parent = right;
            curr->children.pop_back();
        }
    }
    // Else, overflow to left sibling
    else if (left && left->getCurrSize() < m - 1) {
        // Move parent's key down to left sibling
        left->customers.push_back(parent->customers[index - 1]);

        // Move first key from curr up to parent
        parent->customers[index - 1] = curr->customers.front();
        curr->customers.erase(curr->customers.begin());

        // Move first child from curr to left sibling
        if (!curr->isLeaf) {
            left->children.push_back(curr->children.front());
            curr->children.front()->parent = left;
            curr->children.erase(curr->children.begin());
        }
    }
}

void BStarTree::insert(Customer *c) {
    if(!root){
        root = new Treenode(c, true, true, nullptr, m);
        return;
    }

    Treenode *curr = searchNode(c->getKey()); // Use getter
    insertNonFull(curr, c);

    // Check if node is overfull
    while (curr != nullptr && curr->getCurrSize() >= m) {
        if(curr->isRoot) {
            splitRoot();
            curr = root; // Update curr to new root
        }
        else if (canOverflow(curr)) {
            overflowNode(curr);
            curr = curr->parent; // Move up to check parent
        }
        else {
            twoThreeSplit(curr);
            curr = curr->parent; // Move up to check parent
        }
    }
}

void BStarTree::insertNonFull(Treenode *curr, Customer* c) {
    curr->customers.push_back(c);
    sortCustomers(curr->customers);
}

bool BStarTree::deleteCustomer(long long key) {
    // This function was not implemented in the original code.
    cerr << "Delete operation is not implemented." << endl;
    return false;
}

void BStarTree::print() {
    if (!root) {
        cout << "Tree is empty." << endl;
        return;
    }
    queue<Treenode*> q;
    q.push(root);
    int cnt = 1;
    while (!q.empty()) {
        int levelSize = q.size();
        cout << "--- LEVEL " << cnt++ << " ---" << endl;
        for (int k = 0; k < levelSize; k++) {
            Treenode *next = q.front();
            q.pop();
            
            if(next->parent) {
                cout << "Parent ID: " << next->parent->id;
            } else {
                cout << "Root Node";
            }
            cout << " | Node ID: " << next->id << (next->isLeaf ? " (LEAF)" : "") << endl;
            next->print(); // This will print customers
            
            for (size_t i = 0; i < next->children.size(); i++) {
                next->children[i]->id = (cnt-1)*100 + i; // Simple ID for children
                q.push(next->children[i]);
            }
        }
        cout << endl;
    }
}


void BStarTree::makeOutputFile(vector<Customer*> customers) {
    ofstream output("output.txt");
    if (!output.is_open()) {
        cerr << "Error: Could not open output.txt for writing." << endl;
        return;
    }
    for (size_t i = 0; i < customers.size(); i++) {
        if (customers[i] == nullptr) continue; // Safety check
        // Use getters
        output
        << customers[i]->getKey() << "|"
        << customers[i]->getName() << "|"
        << customers[i]->getSurname() << "|"
        << customers[i]->getEmail() << "|"
        << customers[i]->getFKey();
        if (i != customers.size() - 1) output << endl;
    }
    output.close();
    cout << "Wrote " << customers.size() << " customer(s) to output.txt" << endl;
}

Customer* BStarTree::search(long long key) {
    int steps = 0;
    if (!root) {
        cout << "Tree is empty." << endl;
        return nullptr;
    }

    Customer* customer = nullptr;
    Treenode* curr = root;
    steps++;

    while (curr) {
        bool found = false;
        int i = 0;
        for (i = 0; i < curr->getCurrSize(); i++) {
            steps++; // Counting key comparisons
            if (key == curr->customers[i]->getKey()) {
                customer = curr->customers[i];
                found = true;
                break;
            }
            if (key < curr->customers[i]->getKey()) {
                if (!curr->isLeaf) {
                    curr = curr->children[i];
                } else {
                    curr = nullptr; // Not found in leaf
                }
                found = true;
                break;
            }
        }
        if (customer) break; // Found it
        if (found && curr) continue; // Moved down a level

        // If loop finished, key is > all customers in node
        if (i == curr->getCurrSize()) {
            if (!curr->isLeaf) {
                curr = curr->children[curr->getCurrSize()];
            } else {
                curr = nullptr; // Not found in leaf
            }
        }
        if (!curr) break; // Reached end
    }

    cout << "Broj koraka (cvorova): " << steps << endl;
    if (!customer) {
        cout << "Taj kljuc ne postoji" << endl;
    }
    return customer;
}


void BStarTree::searchMulKeys(vector<long long> keys) {
    vector<Customer*> customers;
    customers.reserve(keys.size());
    for (size_t i = 0; i < keys.size(); i++) {
        cout << "--- Pretraga za kljuc: " << keys[i] << " ---" << endl;
        Customer *c = search(keys[i]);
        if (c) {
            customers.push_back(c);
        }
    }
    makeOutputFile(customers);
}

vector<Customer*> BStarTree::searchByName(string name) {
    vector<Customer*> customers;
    if (!root) return customers;

    queue<Treenode*> q;
    q.push(root);
    int cnt = 0; // Node visit counter
    while (!q.empty()) {
        Treenode *next = q.front();
        q.pop();
        cnt++;
        
        // Search all customers in this node
        for (size_t i = 0; i < next->customers.size(); i++) {
            if (next->customers[i]->getName() == name) {
                customers.push_back(next->customers[i]);
            }
        }
        
        // Add all children to the queue
        if (!next->isLeaf) {
            for (size_t i = 0; i < next->children.size(); i++) {
                q.push(next->children[i]);
            }
        }
    }
    cout << "Broj koraka (cvorova): " << cnt << endl;
    return customers;
}