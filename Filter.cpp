#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

int main() {
    std::ifstream inFile("title.basics.tsv");
    std::ofstream outFile("movies_tvshows.tsv");

    std::string line;
    std::getline(inFile, line);
    outFile << line << '\n';

    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;

        while (std::getline(iss, token, '\t')) {
            tokens.push_back(token);
        }

        // Check if titleType is "movie" or "short"
        if (tokens[1] == "movie" /*|| tokens[1] == "short"*/ || tokens[1] == "tvSeries") {
            outFile << line << '\n';
        }
    }

    inFile.close();
    outFile.close();

    return 0;
}
