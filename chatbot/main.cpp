#include <iostream>
#include <string>

std::string aktuelle_antwort;
void answer(std::string antwort);
void no_answer();

int main() {
    
    std::cout << "Chatbot geladen\n";

    while (true) {
        aktuelle_antwort;
        std::cin >> aktuelle_antwort;
        bool is_answer;
        answer(aktuelle_antwort);
    }
    return 0;
}

void answer(std::string antwort) {
    std::cout << "Antwort\n";
}

void no_answer() {
    std::cout << "Keine Antwort\n";
}