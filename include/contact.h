#ifndef PHONEBOOK_CONTACT_H
#define PHONEBOOK_CONTACT_H

#include <string>
#include <vector>

class Contact {
private:
    std::string firstName;
    std::string lastName;
    std::string patronymic;
    std::string address;
    std::string dateOfBirth;
    std::string email;
    std::vector<std::string> phoneNumbers;

public:
    Contact();
    Contact(const std::string& first, const std::string& last,
            const std::string& patron, const std::string& addr,
            const std::string& dob, const std::string& mail);

    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getPatronymic() const;
    std::string getAddress() const;
    std::string getDateOfBirth() const;
    std::string getEmail() const;
    std::vector<std::string> getPhoneNumbers() const;

    void setFirstName(const std::string& first);
    void setLastName(const std::string& last);
    void setPatronymic(const std::string& patron);
    void setAddress(const std::string& addr);
    void setDateOfBirth(const std::string& dob);
    void setEmail(const std::string& mail);
    void addPhoneNumber(const std::string& phone);

    void print() const;
    bool isValid() const;
};

#endif // PHONEBOOK_CONTACT_H
