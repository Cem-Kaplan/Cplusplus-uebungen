#include <iostream>
#include <string>

// Strukturen
struct Person {
    std::string name_;
    int alter_;
    void drucke() const;
    ~Person();
    Person get() const;
};

void Person::drucke() const {
    std::cout << name_ << std::endl;
    std::cout << alter_ << std::endl;
}

Person::~Person() {
    std::cout << "Objekt: " << name_ << " gelöscht" << std::endl;
}

Person Person::get() const {
    return {name_, alter_};
}

int main() {
    Person cem_struktur = {.name_ = "Cem", .alter_ = 19};
    cem_struktur.drucke();

    return 0;
}