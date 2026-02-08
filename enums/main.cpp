#include <iostream>

int main() {
    std::cout << "Enums" << std::endl;

    enum class Auto {
        FAHREN,
        STOPPEN,
        BESCHLEUNIGEN
    };

    Auto audi;
    

    return 0;
}