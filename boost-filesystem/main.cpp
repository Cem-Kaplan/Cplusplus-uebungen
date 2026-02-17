#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <string>

int main() {
    //boost::filesystem::create_directory("test-dir");
    //boost::filesystem::remove("test-dir");
    
    //prüfen, ob csv datei da ist
    boost::filesystem::path csv_path = "datei.csv";
    if (!boost::filesystem::exists(csv_path)) {
        std::cout << "datei nicht vorhanden\n";
    } else {
        std::cout << "datei vorhanden\n";
    }
    //CSV datei einlesen
    std::ifstream datei(csv_path.string());
    std::string line;
    std::getline(datei, line);
    std::cout << line << std::endl;
 
    return 0;
}