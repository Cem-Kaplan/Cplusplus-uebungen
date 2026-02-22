#include <cmath>
#include <iostream>
#include <string>

std::string input;
float calculate(std::string x);
bool checkCalculatable(std::string x);

int main() {

    while (true) {

        std::cout << "Rechnung eingeben\n"
                  << "+ -> Plus\n"
                  << "- -> Minus\n"
                  << "/ -> Geteilt\n"
                  << "* -> Mal\n";

        std::cin >> input;
        if (input.length() == 0) break;
        else if (input.length() > 0) calculate(input);
    }
    return 0;
}

float calculate(std::string x) {
    if (checkCalculatable(x)) {
        
    } else {
    }
}

bool checkCalculatable(std::string x) {
    bool canCalculate = true;
    for (int i = 0; i < x.length(); i++) {
        if (x.at(i) == '+') {
            std::cout << "Ein Plus gefunden\n";
        } else if (x.at(i) == '-') {
            std::cout << "Ein Minus gefunden\n";
        } else if (x.at(i) == '*') {
            std::cout << "Ein Stern gefunden\n";
        }else if (x.at(i) == '/') {
            std::cout << "Ein Slash gefunden\n";
        }
    }
    return canCalculate;
}
