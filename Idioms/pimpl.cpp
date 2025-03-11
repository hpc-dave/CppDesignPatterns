#include <memory>
#include <iostream>
#include <string>

class Person {
public:
    Person();
    ~Person();

    Person(const Person& other);
    Person& operator=(const Person& other);
    Person(Person&& other);
    Person& operator=(Person&& other);

    int year_of_birth() const;
    std::string full_name() const;

    void set_year_of_birth(int year);
    void set_forename(std::string name);
    void set_surname(std::string name);

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

// could be in cpp file now

struct Person::Impl {
    std::string forename{"none"};
    std::string surname{"none"};
    int year_of_birth{0};
    // ... Potentially many more data members
};

Person::Person()
    : pimpl{std::make_unique<Impl>()} {}

Person::~Person() = default;

Person::Person(Person const& other)
    : pimpl{std::make_unique<Impl>(*other.pimpl)} {}

Person& Person::operator=(Person const& other) {
    *pimpl = *other.pimpl;
    return *this;
}

Person::Person(Person&& other)
    : pimpl{std::make_unique<Impl>(std::move(*other.pimpl))} {}

Person& Person::operator=(Person&& other) {
    *pimpl = std::move(*other.pimpl);
    return *this;
}

int Person::year_of_birth() const {
    return pimpl->year_of_birth;
}

std::string Person::full_name() const {
    std::string name{pimpl->forename};
    name += " ";
    name += pimpl->surname;
    return name;
}

void Person::set_year_of_birth(int year){
    pimpl->year_of_birth = year;
}

void Person::set_forename(std::string name) {
    pimpl->forename = name;
}

void Person::set_surname(std::string name) {
    pimpl->surname = name;
}

int main() {
    Person p1{};
    p1.set_year_of_birth(2000);
    p1.set_forename("Alex");
    p1.set_surname("Balex");
    std::cout << "This is " << p1.full_name() << " who was born in " << p1.year_of_birth() << std::endl;
}

// can be compiled with g++ -o pimpl pimpl.cpp
