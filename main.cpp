#include <iostream>
#include <string>
#include <vector>
#include <limits> // For numeric_limits

#include "bstartree.h"
#include "customer.h"

using namespace std;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main(){
    BStarTree *indeks = nullptr;
    vector<long long> keys;
    long long p_key, f_key, key, x;
    vector<Customer*> customers;
    string searchName, name, surname, email, filename;
    Customer *customer;
    
    while (true) {
        cout << endl << "-----------------------------" << endl;
        cout << "Unesite broj komande 1 - 8:" << endl;
        cout << "1. Napravite indeks" << endl;
        cout << "2. Ispisite indeks" << endl;
        cout << "3. Ubacite customera" << endl;
        cout << "4. Brisanje customera" << endl;
        cout << "5. Pretrazivanje (po primarnom kljucu)" << endl;
        cout << "6. Pretrazivanje svih podataka (po imenu)" << endl;
        cout << "7. Pretrazivanje k podataka" << endl;
        cout << "8. Kraj" << endl;
        cout << "-----------------------------" << endl;
        cout << "Izbor: ";
        
        int choice, m, k;
        if (!(cin >> choice)) {
            cout << "Neispravan unos. Molimo unesite broj." << endl;
            clearInput();
            continue;
        }
        
        switch (choice) {
            case 1: {
                cout << "Unesite ime fajla: ";
                cin >> filename;
                cout << "Unesite m (red stabla, >= 3): ";
                while (!(cin >> m) || m < 3) {
                    cout << "Neispravan unos. 'm' mora biti broj >= 3: ";
                    clearInput();
                }
                if (indeks) delete indeks; // Prevent memory leak
                indeks = new BStarTree(filename, m);
                cout << "Indeks je napravljen." << endl;
                break;
            }
            case 2: {
                if (!indeks) cout << "Napravite indeks!" << endl;
                else indeks->print();
                break;
            }
            case 3: {
                if (!indeks) {
                    cout << "Napravite indeks!" << endl;
                    break;
                }
                cout << "Unesite primarni kljuc: ";
                while (!(cin >> p_key)) {
                    cout << "Neispravan kljuc. Unesite broj: ";
                    clearInput();
                }
                cout << "Unesite ime: ";
                cin >> name;
                cout << "Unesite prezime: ";
                cin >> surname;
                cout << "Unesite email: ";
                cin >> email;
                cout << "Unesite strani kljuc: ";
                while (!(cin >> f_key)) {
                    cout << "Neispravan kljuc. Unesite broj: ";
                    clearInput();
                }
                // Note: The tree takes ownership of this pointer
                Customer* c = new Customer(p_key, name, surname, email, f_key);
                indeks->insert(c);
                cout << "Customer ubacen." << endl;
                break;
            }
            case 4: {
                 if (!indeks) {
                    cout << "Napravite indeks!" << endl;
                    break;
                }
                cout << "Unesite primarni kljuc za brisanje: ";
                 while (!(cin >> key)) {
                    cout << "Neispravan kljuc. Unesite broj: ";
                    clearInput();
                }
                indeks->deleteCustomer(key); // This is not implemented
                break;
            }
            case 5: {
                if (!indeks) {
                    cout << "Napravite indeks!" << endl;
                    break;
                }
                cout << "Unesite primarni kljuc po kom pretrazivate: ";
                while (!(cin >> key)) {
                    cout << "Neispravan kljuc. Unesite broj: ";
                    clearInput();
                }
                customer = indeks->search(key);
                if (customer) {
                    customer->print();
                }
                break;
            }
            case 6: {
                if (!indeks) {
                    cout << "Napravite indeks!" << endl;
                    break;
                }
                cout << "Unesite pretrazivano ime: ";
                cin >> searchName;
                customers = indeks->searchByName(searchName);
                cout << endl << "Pronadjeno " << customers.size() << " customera:" << endl;
                for (size_t i = 0; i < customers.size(); i++) {
                    customers[i]->print();
                }
                break;
            }
            case 7: {
                if (!indeks) {
                    cout << "Napravite indeks!" << endl;
                    break;
                }
                cout << "Unesite broj kljuceva: ";
                while (!(cin >> k) || k < 0) {
                    cout << "Neispravan unos. Unesite pozitivan broj: ";
                    clearInput();
                }
                keys.clear(); // Clear previous keys
                for (int i = 0; i < k; i++) {
                    cout << "Unesite " << i + 1 << "-ti kljuc: ";
                    while (!(cin >> x)) {
                       cout << "Neispravan kljuc. Unesite broj: ";
                       clearInput();
                    }
                    keys.push_back(x);
                }
                indeks->searchMulKeys(keys);
                break;
            }
            case 8: {
                delete indeks; // Clean up memory
                cout << "Kraj." << endl;
                return 0;
            }
            default: {
                cout << "Nepoznata komanda." << endl;
                clearInput();
                break;
            }
        }
    }
}