#include "customer.h"
#include <fstream>
#include <sstream>

Customer::Customer(long long key, std::string name, std::string surname, std::string email, long long f_key)
    : m_key(key), m_name(name), m_surname(surname), m_email(email), m_f_key(f_key) {
    // Constructor body
}

Customer::~Customer() {
    // Destructor body
}

void Customer::print() const {
    std::cout << "Customer(ID=" << m_key
              << ", Name=" << m_name
              << ", Surname=" << m_surname
              << ", Email=" << m_email
              << ", FID=" << m_f_key
              << ")\n";
}

std::vector<Customer*> Customer::read_from_file(const std::string& filename) {
    std::vector<Customer*> customers;
    std::ifstream file(filename);
    std::string line;

    // Check if file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return customers;
    }

    while (std::getline(file, line)) { // Use getline to read whole lines
        if (line.empty()) continue; // Skip empty lines

        std::stringstream ss(line);
        std::string name, surname, email, segment;
        long long id, f_id;

        if (!std::getline(ss, segment, '|')) continue;
        try {
            id = std::stoll(segment);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Warning: Skipping line with invalid ID: " << line << std::endl;
            continue;
        }

        if (!std::getline(ss, name, '|')) continue;
        if (!std::getline(ss, surname, '|')) continue;
        if (!std::getline(ss, email, '|')) continue;

        if (!std::getline(ss, segment, '|')) continue;
         try {
            f_id  = std::stoll(segment);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Warning: Skipping line with invalid FID: " << line << std::endl;
            continue;
        }

        Customer* c = new Customer(id, name, surname, email, f_id);
        customers.push_back(c);
    }
    file.close();
    return customers;
}