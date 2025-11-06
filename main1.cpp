#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

class Customer {
public:
    Customer(long long key, string name, string surname, string email, long long f_key) {
        m_key = key;
        m_email = email;
        m_f_key = f_key;
        m_name = name;
        m_surname = surname;
    };

    ~Customer() {
    }

    void print() {
        cout << "Customer(ID=" << m_key
                  << ", Name=" << m_name
                  << ", Surname=" << m_surname
                  << ", Email=" << m_email
                  << ", FID=" << m_f_key
                  << ")\n";
    }

    static vector<Customer*> read_from_file(const string& filename) {
        vector<Customer*> customers;
        ifstream file(filename);
        string line;

        while (file >> line) {
            stringstream ss(line);
            string name, surname, email, segment;
            long long id, f_id;

            getline(ss, segment, '|');
            id = stol(segment);

            getline(ss, name, '|');
            getline(ss, surname, '|');
            getline(ss, email, '|');

            getline(ss, segment, '|');
            f_id  = stol(segment);

            Customer* c = new Customer(id, name, surname, email, f_id);
            customers.push_back(c);
        }
        return customers;
    }

    long long m_key;
    string m_name;
    string m_surname;
    string m_email;
    long long m_f_key;
};

class Treenode {
public:
    Treenode(Customer* customer, bool isRoot, bool isLeaf, Treenode *parent, int m) {
        customers.push_back(customer);
        this->isLeaf = isLeaf;
        this->isRoot = isRoot;
        this->parent = parent;
        this->m = m;
    }
    ~Treenode() {
        for (int i = 0; i < customers.size(); i++) {
            delete customers[i];
        }
    }

    void print() {
        cout << "ID: " <<  id << endl;
        for(int i = 0; i < customers.size(); i++) {
            customers[i]->print();
        }
        cout << endl;
    }

    int getCurrSize(){
        return customers.size();
    }

    vector<Treenode*> children;
    vector<Customer*> customers;
    Treenode* parent;

    int m;
    int id;
    bool isRoot;
    bool isLeaf;
};

class BStarTree {
public:
    BStarTree(const string& filename, int m): m(m), root(nullptr){
        vector<Customer*> customers = Customer::read_from_file(filename);
        for (int i = 0; i < customers.size(); i++) {
            insert(customers[i]);
        }
    }

    ~BStarTree() {
        queue<Treenode*> q;
        Treenode *next = root;
        q.push(next);
        while (!q.empty()) {
            next = q.front();
            q.pop();
            for (int i = 0; i < next->children.size(); i++) {
                if(!next->isLeaf) {
                    q.push(next->children[i]);
                }
            }
            delete next;
        }
    }

    Treenode *searchNode(long long key) {
        Treenode *curr = root;
        while (!curr->isLeaf) {
            bool isLast = false;
            for(int i = 0; i < curr->getCurrSize(); i++) {
                if(key < curr->customers[i]->m_key) {
                    curr = curr->children[i];
                    break;
                }
                if(i == curr->getCurrSize() - 1) isLast = true;
            }
            if(isLast) curr = curr->children[curr->getCurrSize()];
        }
        return curr;
    }

    void sortCustomers(vector<Customer*> &customers) {
        sort(customers.begin(), customers.end(), [] (Customer* a, Customer* b) {
                return a->m_key < b->m_key;
            });
    }

    void splitRoot() {
        int mid = root->customers.size()/2;
        Treenode *newRoot = new Treenode(root->customers[mid], true, false, nullptr, m);
        Treenode* leftChild, *rightChild;
        if (!root->isLeaf) {
            leftChild = new Treenode(root->customers[0], false, false, newRoot, m);
            rightChild = new Treenode(root->customers[mid + 1], false, false, newRoot, m);
        }
        else {
            leftChild = new Treenode(root->customers[0], false, true, newRoot, m);
            rightChild = new Treenode(root->customers[mid + 1], false, true, newRoot, m);
        }

        newRoot->children.push_back(leftChild);
        newRoot->children.push_back(rightChild);
        leftChild->customers  = vector<Customer*> (root->customers.begin(), root->customers.begin() + mid);
        rightChild->customers  = vector<Customer*> (root->customers.begin() + mid + 1, root->customers.end());

        if (!root->isLeaf) {
            leftChild->children  = vector<Treenode*> (root->children.begin(), root->children.begin() + mid + 1);
            for (int i = 0; i < leftChild->children.size(); i++) {
                leftChild->children[i]->parent = leftChild;
            }

            rightChild->children  = vector<Treenode*> (root->children.begin() + mid + 1, root->children.end());
            for (int i = 0; i < rightChild->children.size(); i++) {
                rightChild->children[i]->parent = rightChild;
            }
        }
        root = newRoot;
    }

    void twoThreeSplit(Treenode *curr) {
        Treenode *left = nullptr, *right = nullptr, *parent = curr->parent;
        const int i1 = floor((2 * m - 2) / 3);
        const int i2 = floor((2 * m - 2) / 3) + floor((2 * m - 1) / 3) + 1;
        int index = 0;
        for (int i = 0; i < curr->parent->children.size(); i++ ) {
            if (curr->parent->children[i] == curr) index = i;
        }
        if (index  != parent->children.size() - 1) right = parent->children[index + 1];
        if (index != 0) left = parent->children[index - 1];

        if (right) {
            vector<Customer*> allCustomers;
            vector<Treenode*> allChildren;

            for (int i = 0; i < curr->getCurrSize(); i++) {
                allCustomers.push_back(curr->customers[i]);
            }

            for (int i = 0; i < curr->children.size(); i++) {
                allChildren.push_back(curr->children[i]); //novo
            }

            for (int i = 0; i < right->children.size(); i++) {
                allChildren.push_back(right->children[i]); //novo
            }

            allCustomers.push_back(parent->customers[index]); // mozda - 1 if index != 0

            for (int i = 0; i < right->getCurrSize(); i++) {
                allCustomers.push_back(right->customers[i]);
            }

            curr->customers.assign(allCustomers.begin(), allCustomers.begin() + i1);
            right->customers.assign(allCustomers.begin() + i1 + 1, allCustomers.begin() + i2);

            Treenode *third;

            if (!curr->isLeaf) {
                third = new Treenode(allCustomers[0], false, false, parent, m);
                curr->children.assign(allChildren.begin(), allChildren.begin() + i1 + 1);
                for (int i = 0; i < curr->children.size(); i++) {
                    curr->children[i]->parent = curr;
                }
                right->children.assign(allChildren.begin() + i1 + 1, allChildren.begin() + i2 + 1);
                for (int i = 0; i < right->children.size(); i++) {
                    right->children[i]->parent = right;
                }
                third->children.assign(allChildren.begin() + i2 + 1, allChildren.end());
                for (int i = 0; i < third->children.size(); i++) {
                    third->children[i]->parent = third;
                }
            }
            else {
                third = new Treenode(allCustomers[0], false, true, parent, m);
            }

            third->customers.assign(allCustomers.begin() + i2 + 1, allCustomers.end());

            parent->customers[index] = allCustomers[i1];
            if ((index + 2) == parent->children.size()) parent->customers.push_back(allCustomers[i2]);
            else parent->customers.insert(parent->customers.begin() + index + 2, allCustomers[i2]);
            parent->children.insert(parent->children.begin() + index + 2, third);
        }else {
            vector<Customer*> allCustomers;
            vector<Treenode*> allChildren;

            for (int i = 0; i < left->getCurrSize(); i++) {
                allCustomers.push_back(left->customers[i]);
            }

            for (int i = 0; i < left->children.size(); i++) {
                allChildren.push_back(left->children[i]);
            }

            for (int i = 0; i < curr->children.size(); i++) {
                allChildren.push_back(curr->children[i]);
            }

            allCustomers.push_back(parent->customers[index - 1]);

            for (int i = 0; i < curr->getCurrSize(); i++) {
                allCustomers.push_back(curr->customers[i]);
            }

            left->customers.assign(allCustomers.begin(), allCustomers.begin() + i1);
            curr->customers.assign(allCustomers.begin() + i1 + 1, allCustomers.begin() + i2);

            Treenode *third;

            if (!curr->isLeaf) {
                third = new Treenode(allCustomers[0], false, false, parent, m);
                left->children.assign(allChildren.begin(), allChildren.begin() + i1 + 1);
                for (int i = 0; i < left->children.size(); i++) {
                    left->children[i]->parent = left;
                }
                curr->children.assign(allChildren.begin() + i1 + 1, allChildren.begin() + i2 + 1);
                for (int i = 0; i < curr->children.size(); i++) {
                    curr->children[i]->parent = curr;
                }
                third->children.assign(allChildren.begin() + i2 + 1, allChildren.end());
                for (int i = 0; i < third->children.size(); i++) {
                    third->children[i]->parent = third;
                }
            }
            else {
                third = new Treenode(allCustomers[0], false, true, parent, m);
            }

            third->customers.assign(allCustomers.begin() + i2 + 1, allCustomers.end());

            parent->customers[index - 1] = allCustomers[i1];
            parent->customers.insert(parent->customers.begin() + index, allCustomers[i2]);
            parent->children.insert(parent->children.begin() + index + 1, third);
        }
    }

    bool canOverflow(Treenode *curr) {
        Treenode *p = curr->parent;
        int currIdx;

        for (int i = 0; i < p->children.size(); i++) {
            if (p->children[i] == curr) {
                currIdx = i;
                break;
            }
        }

        Treenode* rightBrother = nullptr, *leftBrother = nullptr;
        if (currIdx != p->children.size() - 1) {
            rightBrother = p->children[currIdx + 1];
            if(rightBrother->getCurrSize() < m - 1) {
                return true;
            }
        }
        if (currIdx != 0){
            leftBrother = p->children[currIdx - 1];
            if(leftBrother->getCurrSize() < m - 1) {
                return true;
            }
        }
        return false;
    }

    void overflowNode(Treenode *curr) {
        Treenode *left = nullptr, *right = nullptr, *parent = curr->parent;
        int index = 0;
        for (int i = 0; i < curr->parent->children.size(); i++ ) {
            if (curr->parent->children[i] == curr) index = i;
        }
        if (index  != parent->children.size() - 1) right = parent->children[index + 1];
        if (index != 0) left = parent->children[index - 1];

        Customer* parentCustomer = parent->customers[index - 1];

        if (right && right->getCurrSize() < m - 1) {
            right->customers.insert(right->customers.begin(), parentCustomer);
            curr->parent->customers[index - 1] = curr->customers[curr->getCurrSize() - 1];
            curr->customers.erase(curr->customers.end());
            if (!curr->isLeaf) {
                right->children.insert(right->children.begin(), curr->children[curr->children.size() - 1]);
                curr->children[curr->children.size() - 1]->parent = right;
                curr->children.erase(curr->children.end());
            }
        }else {
            left->customers.push_back(parentCustomer);
            curr->parent->customers[index - 1] = curr->customers[0];
            curr->customers.erase(curr->customers.begin());
            if (!curr->isLeaf) {
                left->children.push_back(curr->children[0]);
                curr->children[0]->parent = left;
                curr->children.erase(curr->children.begin());
            }
        }
    }

    void insert(Customer *c) {
        if(!root){
            root = new Treenode(c, true, true, nullptr, m);
            return;
        }

        Treenode *curr = searchNode(c->m_key);
        insertNonFull(curr, c);

        if(curr->isRoot && curr->getCurrSize() <=  2 * floor((2 * m - 2) / 3)) return;
        if (curr->getCurrSize() <= m - 1) return;

        while (true) {
            if(curr->isRoot) {
                splitRoot();
                return;
            }

            if (canOverflow(curr)) {
                overflowNode(curr);
                return;
            }

            twoThreeSplit(curr);
            if(curr->parent->isRoot && curr->parent->getCurrSize() <=  2 * floor((2 * m - 2) / 3)) return;
            if (curr->parent->getCurrSize() <= m - 1) return;

            curr = curr->parent;
        }
    }

    void insertNonFull(Treenode *curr, Customer* c) {
        curr->customers.push_back(c);
        sortCustomers(curr->customers);
    }

    bool deleteCustomer(long long key);

    void print() {
        queue<Treenode*> q;
        Treenode *next = root;
        q.push(next);
        int cnt = 1;
        while (!q.empty()) {
            next = q.front();
            next->id = cnt++;
            q.pop();
            if(next->parent) {
                cout << "Parent ID: " << next->parent->id << endl;
            }else cout << "Root Node" << endl;
            next->print();
            for (int i = 0; i < next->children.size(); i++) {
                if(!next->isLeaf) {
                    q.push(next->children[i]);
                }
            }
        }
        cout << endl;
    }

    void makeOutputFile(vector<Customer*> customers) {
        ofstream output("output.txt");
        for (int i = 0; i < customers.size(); i++) {
            output
            << customers[i]->m_key << "|"
            << customers[i]->m_name << "|"
            << customers[i]->m_surname << "|"
            << customers[i]->m_email << "|"
            << customers[i]->m_f_key;
            if (i != customers.size() - 1) output << endl;
        }
        output.close();
    }

    Customer* search(long long key) {
        int steps = 0;
        Customer* customer = nullptr;
        Treenode* curr = root;
        while (curr) {
            bool found = false;
            for (int i = 0; i < curr->getCurrSize(); i++) {
                if (key == curr->customers[i]->m_key) {
                    customer = curr->customers[i];
                    found = true;
                    break;
                }
                if (key < curr->customers[i]->m_key) {
                    curr = curr->children[i];
                    found = true;
                    break;
                }
            }
            if (!found) {
                curr = curr->children[curr->getCurrSize()];
            }
            steps++;
            if (curr && curr->isLeaf) {
                break;
            }
        }
        if (curr) {
            for (int i = 0; i < curr->getCurrSize(); i++) {
                if (key == curr->customers[i]->m_key) {
                    customer = curr->customers[i];
                    break;
                }
            }
        }
        cout << "Broj koraka: " << steps << endl;
        if (!customer) {
            cout << "Taj kljuc ne postoji" << endl;
        }

        return customer;
    }

    void searchMulKeys(vector<long long> keys) {
        vector<Customer*> customers;

        customers.reserve(keys.size());
        for (int i = 0; i < keys.size(); i++) {
            Customer *c = search(keys[i]);
            customers.emplace_back(c);
        }
        makeOutputFile(customers);
    }

    vector<Customer*> searchByName(string name) {
        vector<Customer*> customers;
        queue<Treenode*> q;
        Treenode *next = root;
        q.push(next);
        int cnt = 1;
        while (!q.empty()) {
            next = q.front();
            cnt++;
            q.pop();
            for (int i = 0; i < next->customers.size(); i++) {
                if (next->customers[i]->m_name == name) {
                    customers.push_back(next->customers[i]);
                }
            }
            for (int i = 0; i < next->children.size(); i++) {
                if(!next->isLeaf) {
                    q.push(next->children[i]);
                }
            }
        }
        cout << "Broj koraka: " << cnt << endl;
        return customers;
    }

    int m;
    Treenode *root;
};

int main(){
    BStarTree *indeks = nullptr;
    vector<long long> keys;
    long long p_key, f_key, key, x;
    vector<Customer*> customers;
    string searchName, name, surname, email, filename;
    Customer *customer, *c ;
    while (true) {
        cout << endl << "-----------------------------" << endl;
        cout << "Unesite broj komande 1 - 7:" << endl;
        cout << "1.Napravite indeks" << endl;
        cout << "2.Ispisite indeks" << endl;
        cout << "3.Ubacite customera" << endl;
        cout << "4.Brisanje customera" << endl;
        cout << "5.Pretrazivanje (po primarnom kljucu)" << endl;
        cout << "6.Pretrazivanje svih podataka (po imenu)" << endl;
        cout << "7.Pretrazivanje k podataka" << endl;
        cout << "8.Kraj" << endl;
        cout << "-----------------------------" << endl;
        int choice, m, k;
        cin >> choice;
        switch (choice) {
            case 1: {
                cout << "Unesite ime fajla: " << endl;
                cin >> filename;
                cout << "Unesite m (3 - 10): " << endl;
                cin >> m;
                indeks = new BStarTree(filename, m);
                break;
            }
            case 2: {
                if (!indeks) cout << "Napravite indeks!" << endl;
                indeks->print();
                break;
            }
            case 3: {
                cout << "Unesite primarni kljuc: " << endl;
                cin >> p_key;
                cout << "Unesite ime: " << endl;
                cin >> name;
                cout << "Unesite prezime: " << endl;
                cin >> surname;
                cout << "Unesite email: " << endl;
                cin >> email;
                cout << "Unesite strani kljuc: " << endl;
                cin >> f_key;
                c = new Customer(p_key, name, surname, email, f_key);
                indeks->insert(c);
                break;
            }
            case 4: {
                break;
            }
            case 5: {
                cout << "Unesite primarni kljuc po kom pretrazivate: " << endl;
                cin >> key;
                customer = indeks->search(key);
                customer->print();
                break;
            }
            case 6: {
                cout << "Unesite pretrazivano ime: " << endl;
                cin >> searchName;
                customers = indeks->searchByName(searchName);
                cout << endl;
                for (int i = 0; i < customers.size(); i++) {
                    customers[i]->print();
                }
                break;
            }
            case 7: {
                cout << "Unesite broj kljuceva: " << endl;
                cin >> k;
                for (int i = 0; i < k; i++) {
                    cout << "Unesite " << i + 1 << "-ti kljuc: " << endl;
                    cin >> x;
                    keys.push_back(x);
                }
                indeks->searchMulKeys(keys);
                keys.clear();
                break;
            }
            case 8: {
                delete indeks;
                return 0;
            }
        }
    }
}