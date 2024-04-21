#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <set>
#include <algorithm>
#include <utility>
#include <math.h>
using namespace std;

struct Vertex {
    string name;
    string titleType;
    bool isAdult;
    int startYear;
    int endYear;
    int runtimeMinutes;
    string genre_list[3];
    float averageRating;

    Vertex() {
        name = "";
        titleType = "";
        isAdult = false;
        startYear = 0;
        endYear = 0;
        runtimeMinutes = 0;
        averageRating = 0.0;
    }

    Vertex(string name, string& titleType, bool isAdult, string startYear, string endYear, string runtimeMinutes, string& genres, string rating) {
        this->name = name;
        this->titleType = titleType;
        this->isAdult = isAdult;
        if(startYear == "\\N"){
            this->startYear = 0;
        }
        else{
            this->startYear = stoi(startYear);
        }
        if(endYear == "\\N"){
            this->endYear = 0;
        }
        else{
            this->endYear = stoi(endYear);
        }
        if(runtimeMinutes == "\\N"){
            this->runtimeMinutes = 0;
        }
        else{
            this->runtimeMinutes = stoi(runtimeMinutes);
        }
        if(rating == "\\N"){
            this->averageRating = 0;
        }
        else{
            this->averageRating = stof(rating);
        }

        istringstream stream(genres);
        string temp_genre;
        int i = 0;
        while (getline(stream, temp_genre, ',') && i < 3) {
            this->genre_list[i] = temp_genre;
            i++;
        }
    }
};

class Graph {
    map<string, Vertex> graph;
public:
    void GraphSize(){
        cout << graph.size();
    }

    map<string, Vertex> GiveGraph(){
        return graph;
    }

    void read(ifstream& file) {
        string singleLine;
        string primaryTitle;
        string ignore;
        string titleType;
        bool isAdult;
//        string isAdultString;
//        string startYearString;
//        string endYearString;
//        string runtimeMinutesString;
        int runtimeMinutes;
//        string genres[3];
        string genre;
//        string genre1;
//        string genre2;
//        string genre3;
        int size = 1667546;

        if (file.is_open()) {
            getline(file, singleLine);
            istringstream stream(singleLine);
            getline(stream, ignore);
            string line;
            while(getline(file, line)) {
                istringstream stream(line);
                vector<string> movie_qualities;
                string quality;
                while(getline(stream, quality, '\t')){
                    movie_qualities.push_back(quality);
                }
                string primaryTitle = movie_qualities[2];
                string titleType = movie_qualities[1];
                bool isAdult = stoi(movie_qualities[4]);
                string startYear = movie_qualities[5];
                string endYear = movie_qualities[6];
                string runtimeMinutes = movie_qualities[7];
                string genre = movie_qualities[8];
                string rating = movie_qualities[9];
                graph.emplace(primaryTitle, Vertex(primaryTitle, titleType, isAdult, startYear, endYear, runtimeMinutes, genre, rating));
            }
            file.close();
        }
    }

    map<float, vector<Vertex>> jaccard(string& name, bool adult, int initialYear, string& movieOrShow, int runtimeMinutes){
        Vertex data = graph[name];
        string inputedNameProperties[6];
        map<float, vector<Vertex>> similarTitles;
        for (int i = 0; i < 3; i++) {
            inputedNameProperties[i] = data.genre_list[i];
        }
        inputedNameProperties[3] = data.titleType;
        inputedNameProperties[4] = to_string(data.isAdult);
        inputedNameProperties[5] = to_string(data.runtimeMinutes);

        int n = sizeof(inputedNameProperties) / sizeof(inputedNameProperties[0]);
        for(auto iter = graph.begin(); iter != graph.end(); iter++){ // iterate through graph, compare each values' genre list to the given movie's genre list
            string iteratedNameProperties[6];
            for(int i = 0; i < 3; i++){
                iteratedNameProperties[i] = iter->second.genre_list[i];
            }
            iteratedNameProperties[3] = iter->second.titleType;
            iteratedNameProperties[4] = to_string(iter->second.isAdult);
            iteratedNameProperties[5] = to_string(iter->second.runtimeMinutes);
            set<string> unionGenres;
            set<string> intersectionGenres;
            set_union(inputedNameProperties, inputedNameProperties + n, iteratedNameProperties, iteratedNameProperties + n, inserter(unionGenres, unionGenres.begin()));
            if (unionGenres.find("") != unionGenres.end()) {
                unionGenres.erase("");
            }
            set_intersection(inputedNameProperties, inputedNameProperties + n, iteratedNameProperties, iteratedNameProperties + n, inserter(intersectionGenres, intersectionGenres.begin()));
            if (intersectionGenres.find("") != intersectionGenres.end()) {
                intersectionGenres.erase("");
            }
            float similarity = (float)intersectionGenres.size() / (float)unionGenres.size();
            if(iter->second.runtimeMinutes > runtimeMinutes || iter->second.startYear < initialYear){
                continue;
            }
            if(!adult){
                if(iter->second.isAdult){
                    continue;
                }
            }
            if(movieOrShow == "movie"){
                if(iter->second.titleType == "tvSeries"){
                    continue;
                }
            }
            else if(movieOrShow == "tv show"){
                if(iter->second.titleType == "movie"){
                    continue;
                }
            }

            if (iter->first != name) {
                similarTitles[similarity].push_back(iter->second);
            }
        }
        return similarTitles;
    }

    map<float, vector<Vertex>> cosine(string& name, bool adult, int initialYear, string& movieOrShow, int runtimeMinutes){
        Vertex data = graph[name];
        vector<string> inputedNameProperties;
        map<float, vector<Vertex>> similarTitles;
        for (int i = 0; i < 3; i++) {
            inputedNameProperties.push_back(data.genre_list[i]);
        }
        inputedNameProperties.push_back(data.titleType);
        inputedNameProperties.push_back(to_string(data.isAdult));
        inputedNameProperties.push_back(to_string(data.runtimeMinutes));
        for(auto iter = graph.begin(); iter != graph.end(); iter++){ // iterate through graph, compare each values' genre list to the given movie's genre list
            set<string> unionProperties;
            vector<string> iteratedNameProperties;
            for(int i = 0; i < 3; i++){
                iteratedNameProperties.push_back(iter->second.genre_list[i]);
                unionProperties.insert(inputedNameProperties[i]);
                unionProperties.insert(iter->second.genre_list[i]);
            }
            iteratedNameProperties.push_back(iter->second.titleType);
            unionProperties.insert(inputedNameProperties[3]);
            unionProperties.insert(iter->second.titleType);
            iteratedNameProperties.push_back(to_string(iter->second.isAdult));
            unionProperties.insert(inputedNameProperties[4]);
            unionProperties.insert(to_string(iter->second.isAdult));
            iteratedNameProperties.push_back(to_string(iter->second.runtimeMinutes));
            unionProperties.insert(inputedNameProperties[5]);
            unionProperties.insert(to_string(iter->second.runtimeMinutes));

            if (unionProperties.find("") != unionProperties.end()) {
                unionProperties.erase("");
            }
            vector<int> temp = {0, 0};
            map<string, vector<int>> similarityTable;
            for (auto iter2 = unionProperties.begin(); iter2 != unionProperties.end(); iter2++) {
                similarityTable[*iter2] = temp;
                if (find(inputedNameProperties.begin(), inputedNameProperties.end(), *iter2) != inputedNameProperties.end()) {
                    similarityTable[*iter2][0] = 1;
                }
                if (find(iteratedNameProperties.begin(), iteratedNameProperties.end(), *iter2) != iteratedNameProperties.end()) {
                    similarityTable[*iter2][1] = 1;
                }
            }
            int dot_product = 0;
            float a_distance = 0;
            float b_distance = 0;
            for (auto iter3 = similarityTable.begin(); iter3 != similarityTable.end(); iter3++) {
                dot_product += (iter3->second[0] * iter3->second[1]);
                a_distance += (float)pow(iter3->second[0], 2);
                b_distance += (float)pow(iter3->second[1], 2);
            }
            a_distance = sqrt(a_distance);
            b_distance = sqrt(b_distance);
            float similarity = (float)dot_product / (a_distance * b_distance);
            if(iter->second.runtimeMinutes > runtimeMinutes || iter->second.startYear < initialYear){
                continue;
            }
            if(!adult){
                if(iter->second.isAdult){
                    continue;
                }
            }
            if(movieOrShow == "movie"){
                if(iter->second.titleType == "tvSeries"){
                    continue;
                }
            }
            else if(movieOrShow == "tv show"){
                if(iter->second.titleType == "movie"){
                    continue;
                }
            }
            if (iter->first != name) {
                similarTitles[similarity].push_back(iter->second);
            }
        }
        return similarTitles;
    }

    void PrintTop20Jaccard(string& name, bool adult, int initialYear, string movieOrShow, int runtimeMinutes){
        map<float, vector<Vertex>> titles = jaccard(name, adult, initialYear, movieOrShow, runtimeMinutes);
        int counter = 0;
        for(auto iter = titles.rbegin(); iter != titles.rend(); iter++){
            for(int i = 0; i < iter->second.size(); i++) {
                if(counter == 50){
                    return;
                }
                cout << iter->second[i].name << "\t" << iter->first << "\t" << iter->second[i].averageRating << endl;
                counter++;
            }
        }
    }

    void PrintTop20Cosine(string& name, bool adult, int initialYear, string movieOrShow, int runtimeMinutes){
        map<float, vector<Vertex>> titles = cosine(name, adult, initialYear, movieOrShow, runtimeMinutes);
        int counter = 0;
        for(auto iter = titles.rbegin(); iter != titles.rend(); iter++){
            for(int i = 0; i < iter->second.size(); i++) {
                if(counter == 50){
                    return;
                }
                cout << iter->second[i].name << " " << iter->first << endl;
                counter++;
            }
        }
    }
};

//int main() {
//    Graph g;
//    string name;
//    ifstream myfile("filtered_titles.tsv");
//    g.read(myfile);
//    cout << "here" << endl;
//    g.GraphSize();
//    cout << endl;
//    cout << "Enter movie/show name: " << endl;
//    getline(cin, name);
//    g.jaccard(name);
//    g.PrintTop20Jaccard(name);
//    cout << endl;
//    cout << "here" << endl;
//    cout << endl;
//    g.cosine(name);
//    g.PrintTop20Cosine(name);
//    return 0;
//}

int main(){
    Graph g;
    string name;
    ifstream myfile("merged_data.tsv");
    cout << "Please give us a moment while our list is being created :)" << endl << endl;
    g.read(myfile);
    bool continueProgram = true;
        cout << "Enter movie/show name: " << endl;
        getline(cin, name);
        cout << endl;
        string optionstring;
        bool custom = false;
        bool isAdult = true;
        int initialYear = 0;
        string moviesorshows = "both";
        int runTime = 1000000;
        while (true) {
            cout << "Do you want to customize your search? Ex: Adult Rated movies shown, Earliest year made. (Respond with Yes or No)" << endl;
            cin >> optionstring;
            if (optionstring == "Yes") {
                custom = true;
                break;
            } else if (optionstring == "No") {
                custom = false;
                break;
            } else {
                cout << "Please respond with Yes or No." << endl;
            }
        }
    cout << endl;
        if (custom) {
            while (true) {
                cout << "Would you like your list to contain movies, tv shows, or both? (Respond with movies, shows, or both)" << endl;
                cin >> moviesorshows;
                if (moviesorshows == "movies" || moviesorshows == "shows" || moviesorshows == "both") {
                    break;
                }
                else {
                    cout << "Please respond with either movies, tv shows, or both." << endl << endl;
                }
            }
            cout << endl;
            string isAdultString;
            while (true) {
                cout << "Would you like your list to contain adult rating movies or tv shows? (Respond with Yes or No)"
                     << endl;
                cin >> isAdultString;
                if (isAdultString == "Yes") {
                    isAdult = true;
                    break;
                } else if (isAdultString == "No") {
                    isAdult = false;
                    break;
                } else {
                    cout << "Please respond with Yes or No." << endl << endl;
                }
            }
            cout << endl;
            string earliestRelaseDate;
            string initialYearString;
            while (true) {
                cout << "Does it matter when the movie or tv show originally came out? (Respond with Yes or No)"
                     << endl;
                cin >> earliestRelaseDate;
                if (earliestRelaseDate == "No") {
                    break;
                } else if (earliestRelaseDate == "Yes") {
                    bool isYear = false;
                    while (!isYear) {
                        cout
                                << "What is the earliest release year you want? (Respond with only the year, value cannot be over 2024, must have 4 digits Ex: 2002, 0001)"
                                << endl;
                        cin >> initialYearString;
                        bool notValidYear = false;
                        for (char num: initialYearString) {
                            if (!isdigit(num)) {
                                notValidYear = true;
                            }
                        }
                        if (!notValidYear || initialYearString.length() != 4) {
                            cout << "Please put a valid year." << endl << endl;
                        } else {
                            initialYear = stoi(initialYearString);
                            isYear = true;
                            break;
                        }
                    }
                    break;
                }
            }
            cout << endl;
            string runtimeResponse, runtimeMinutesString;
            while (true) {
                cout << "Does it matter how long the movie or tv show is? (Respond with Yes or No)" << endl;
                cin >> runtimeResponse;
                if (runtimeResponse == "Yes") {
                    bool runTimeBool = false;
                    while (!runTimeBool) {
                        cout << "At most, how long can a movie be? (Respond with a number in minutes Ex: 60, 120)"
                             << endl;
                        cin >> runtimeMinutesString;
                        bool notValidRunTime = false;
                        for (char num: runtimeMinutesString) {
                            if (!isdigit(num)) {
                                notValidRunTime = true;
                            }
                        }
                        if (notValidRunTime) {
                            cout << "Please put in a valid max movie length in minutes." << endl << endl;
                        } else {
                            runTime = stoi(runtimeMinutesString);
                            runTimeBool = true;
                            break;
                        }
                    }
                    break;
                } else if (runtimeResponse == "No") {
                    break;
                } else {
                    cout << "Please respond with Yes or No." << endl << endl;
                }
            }
            cout << endl;
        }
        string jaccardOrCosine;
        while (true) {
            cout
                    << "Would you like to utilize Jaccard Similarity Sorting or Cosine Similarity Sorting? (Respond with exactly 'Jaccard' or 'Cosine' Ex: Jaccard)"
                    << endl;
            cout << "Jaccard & Cosine Description HERE" << endl;
            cin >> jaccardOrCosine;
            if (jaccardOrCosine == "Jaccard") {
                cout << "Loading your list, please wait a moment." << endl << endl;
                g.PrintTop20Jaccard(name, isAdult, initialYear, moviesorshows, runTime);
                break;
            } else if (jaccardOrCosine == "Cosine") {
                cout << "Loading your list, please wait a moment." << endl << endl;
                g.PrintTop20Cosine(name, isAdult, initialYear, moviesorshows, runTime);
                break;
            } else {
                cout << "Please respond with either 'Jaccard' or 'Cosine'" << endl;
            }
        }
//        string continueProgramResponse;
//        while(true) {
//            cout << "Would you like to make another list? (Respond with 'Yes' or 'No')" << endl;
//            cin >> continueProgramResponse;
//            if (continueProgramResponse == "Yes") {
//                continueProgram = true;
//                break;
//            } else if (continueProgramResponse == "No") {
//                continueProgram = false;
//                break;
//            } else {
//                cout << "Please respond with either 'Yes' or 'No'" << endl;
//            }
//        }
}
