#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct MovieData {
    std::string titleType;
    std::string primaryTitle;
    std::string originalTitle;
    int isAdult;
    std::string startYear;
    std::string endYear;
    std::string runtimeMinutes;
    std::string genres;
    std::string averageRating = "\\N";  // Initialize to "\N" to handle missing ratings
};

// Function to parse a line from the TSV file
std::vector<std::string> parseTSVLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (getline(iss, token, '\t')) {
        tokens.push_back(token);
    }
    return tokens;
}

// Read movie data from TSV file
void readMoviesTSV(const std::string& filename, std::unordered_map<std::string, MovieData>& moviesMap) {
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // Skip header line
    while (getline(file, line)) {
        auto tokens = parseTSVLine(line);
        MovieData data{
            tokens[1], tokens[2], tokens[3], std::stoi(tokens[4]), tokens[5], tokens[6], tokens[7], tokens[8]
        };
        moviesMap[tokens[0]] = data;
    }
}

// Read ratings from TSV file and update movie data with ratings
void mergeRatings(const std::string& filename, std::unordered_map<std::string, MovieData>& moviesMap) {
    std::ifstream file(filename);
    std::string line;
    getline(file, line); // Skip header line
    while (getline(file, line)) {
        auto tokens = parseTSVLine(line);
        if (moviesMap.find(tokens[0]) != moviesMap.end()) {
            moviesMap[tokens[0]].averageRating = tokens[1];
        }
    }
}

// Write the merged data to a TSV file
void writeMergedTSV(const std::string& filename, const std::unordered_map<std::string, MovieData>& moviesMap) {
    std::ofstream file(filename);
    file << "tconst\ttitleType\tprimaryTitle\toriginalTitle\tisAdult\tstartYear\tendYear\truntimeMinutes\tgenres\taverageRating\n";
    for (const auto& pair : moviesMap) {
        const auto& data = pair.second;
        file << pair.first << '\t'
            << data.titleType << '\t'
            << data.primaryTitle << '\t'
            << data.originalTitle << '\t'
            << data.isAdult << '\t'
            << data.startYear << '\t'
            << data.endYear << '\t'
            << data.runtimeMinutes << '\t'
            << data.genres << '\t'
            << data.averageRating << '\n';
    }
}


int main() {
    std::unordered_map<std::string, MovieData> moviesMap;

    // Load the movies and ratings data
    readMoviesTSV("movies_tvshows.tsv", moviesMap);
    mergeRatings("title.ratings.tsv", moviesMap);

    // Save the merged data
    writeMergedTSV("merged_data.tsv", moviesMap);

    return 0;
}
