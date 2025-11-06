#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <vector>
#include <iostream>

class Customer {
public:
    Customer(long long key, std::string name, std::string surname, std::string email, long long f_key);
    ~Customer();

    void print() const;

    // Static helper to read from file
    static std::vector<Customer*> read_from_file(const std::string& filename);

    // Getters for private members
    long long getKey() const { return m_key; }
    const std::string& getName() const { return m_name; }
    const std::string& getSurname() const { return m_surname; }
    const std::string& getEmail() const { return m_email; }
    long long getFKey() const { return m_f_key; }

private:
    long long m_key;
    std::string m_name;
    std::string m_surname;
    std::string m_email;
    long long m_f_key;
};

#endif // CUSTOMER_H