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
#include <iomanip>
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

    Vertex(string& name, string& titleType, bool isAdult, string& startYear, string& endYear, string& runtimeMinutes, string& genres, string& rating) {
        this->name = name;
        this->titleType = titleType;
        this->isAdult = isAdult;
        if(startYear == "\\N"){
            this->startYear = 11111;
        }
        else{
            this->startYear = stoi(startYear);
        }
        if(endYear == "\\N"){
            this->endYear = 11110;
        }
        else{
            this->endYear = stoi(endYear);
        }
        if(runtimeMinutes == "\\N"){
            this->runtimeMinutes = -1;
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

bool ratingCompare(Vertex& a, Vertex& b){
    return a.averageRating > b.averageRating;
}

class DataSet {
    map<string, Vertex> dataset;
public:
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
                dataset.emplace(primaryTitle, Vertex(primaryTitle, titleType, isAdult, startYear, endYear, runtimeMinutes, genre, rating));
            }
            file.close();
        }
    }

    map<float, vector<Vertex>> jaccard(string& name){
        Vertex data = dataset[name];
        string inputedNameProperties[6];
        map<float, vector<Vertex>> similarTitles;
        for (int i = 0; i < 3; i++) {
            inputedNameProperties[i] = data.genre_list[i];
        }
        inputedNameProperties[3] = data.titleType;
        inputedNameProperties[4] = to_string(data.isAdult);
        inputedNameProperties[5] = to_string(data.runtimeMinutes);

        int n = sizeof(inputedNameProperties) / sizeof(inputedNameProperties[0]);
        for(auto iter = dataset.begin(); iter != dataset.end(); iter++){ // iterate through graph, compare each values' genre list to the given movie's genre list
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

            if (iter->first != name) {
                similarTitles[similarity].push_back(iter->second);
            }
        }
        return similarTitles;
    }

    map<float, vector<Vertex>> cosine(string& name){
        Vertex data = dataset[name];
        vector<string> inputedNameProperties;
        map<float, vector<Vertex>> similarTitles;
        for (int i = 0; i < 3; i++) {
            inputedNameProperties.push_back(data.genre_list[i]);
        }
        inputedNameProperties.push_back(data.titleType);
        inputedNameProperties.push_back(to_string(data.isAdult));
        inputedNameProperties.push_back(to_string(data.runtimeMinutes));
        for(auto iter = dataset.begin(); iter != dataset.end(); iter++){ // iterate through graph, compare each values' genre list to the given movie's genre list
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

            if (iter->first != name) {
                similarTitles[similarity].push_back(iter->second);
            }
        }
        return similarTitles;
    }

    void PrintTop50Jaccard(string& name, bool adult, int initialYear, string& movieOrShow, int runtimeMinutes){
        map<float, vector<Vertex>> titles = jaccard(name);
        int counter = 0;
        for(auto iter = titles.rbegin(); iter != titles.rend(); iter++) {
            sort(iter->second.begin(), iter->second.end(), ratingCompare);
        }
        cout << setw(10) << "Ranking" << " | " << setw(10) << "Movie/TV Show Title" << " | " << setw(10) << "Similarity (%)" << " | " << setw(10) << "Average Rating (out of 10)" << endl;
        cout << setw(10) << "---------------------------------------------------------------------------------" << endl;
        for(auto iter = titles.rbegin(); iter != titles.rend(); iter++){
            for(int i = 0; i < iter->second.size(); i++) {
                if(counter == 50){
                    return;
                }
                if (iter->second[i].runtimeMinutes > runtimeMinutes || iter->second[i].startYear < initialYear) {
                    continue;
                }
                if (!adult) {
                    if (iter->second[i].isAdult) {
                        continue;
                    }
                }
                if (movieOrShow == "movie") {
                    if (iter->second[i].titleType == "tvSeries") {
                        continue;
                    }
                } else if (movieOrShow == "tvSeries") {
                    if (iter->second[i].titleType == "movie") {
                        continue;
                    }
                }

                stringstream stream;
                stream << fixed << setprecision(0) << (iter->first * 100);
                string s = stream.str();

                cout << setw(10) << counter + 1 << " | " << setw(5) << iter->second[i].name << " | " << setw(2);
                cout << s << "% | " << setw(2);
                if (iter->second[i].averageRating == 0) {
                    cout << "N/A" << endl;
                } else {
                    cout << iter->second[i].averageRating << endl;
                }
                counter++;
            }
        }
    }

    void PrintTop50Cosine(string& name, bool adult, int initialYear, string& movieOrShow, int runtimeMinutes){
        map<float, vector<Vertex>> titles = cosine(name);
        int counter = 0;
        for(auto iter = titles.rbegin(); iter != titles.rend(); iter++) {
            sort(iter->second.begin(), iter->second.end(), ratingCompare);
        }
        cout << setw(10) << "Ranking" << " | " << setw(10) << "Movie/TV Show Title" << " | " << setw(10) << "Similarity (%)" << " | " << setw(10) << "Average Rating (out of 10)" << endl;
        cout << setw(10) << "---------------------------------------------------------------------------------" << endl;
        for(auto iter = titles.rbegin(); iter != titles.rend(); iter++){
            for(int i = 0; i < iter->second.size(); i++) {
                if(counter == 50){
                    return;
                }
                if(iter->second[i].runtimeMinutes > runtimeMinutes || iter->second[i].startYear < initialYear){
                    continue;
                }
                if(!adult){
                    if(iter->second[i].isAdult){
                        continue;
                    }
                }
                if(movieOrShow == "movie"){
                    if(iter->second[i].titleType == "tvSeries"){
                        continue;
                    }
                }
                else if(movieOrShow == "tvSeries"){
                    if(iter->second[i].titleType == "movie"){
                        continue;
                    }
                }
                stringstream stream;
                stream << fixed << setprecision(0) << (iter->first * 100);
                string s = stream.str();

                cout << setw(10) << counter + 1 << " | " << setw(5) << iter->second[i].name << " | " << setw(2);
                cout << s << "% | " << setw(2);
                if (iter->second[i].averageRating == 0) {
                    cout << "N/A" << endl;
                }
                else {
                    cout << iter->second[i].averageRating << endl;
                }
                counter++;
            }
        }
    }
};

int main(){
    DataSet ds;
    string name;
    ifstream myfile("merged_data.tsv");
    cout << "Please give us a moment while our data set is being created... :)" << endl;
    ds.read(myfile);
    cout << "Process complete!" << endl << endl;
    cout << "Welcome to TVRecommend, where we cater to all of your television interests by recommending you different " << endl;
    cout << "movies/shows based on how similar they are to your preferred movies/shows. Our data set includes over 8,000,000 " << endl;
    cout << "titles to choose from, but don't worry! We'll make sure to give you a tailor-made top 50 list of titles ranked " << endl;
    cout << "in terms of optimal similarity and average rating. Our similarity indices are calculated using title type, whether " << endl;
    cout << "or not the title is for adults, runtime, and genres. So let's get calculating!" << endl << endl;
    bool continueProgram = true;
    cout << "Please enter the name of a movie/show that you like: " << endl;
    getline(cin, name);
    cout << endl;
    string optionstring;
    bool custom;
    bool isAdult = true;
    int initialYear = 0;
    string moviesorshows = "both";
    int runTime = 1000000;
    while (true) {
        cout << "Would you like to customize your list with features like title type, minimum start year, and runtime?" << endl;
        cout << "(Respond with 'Yes' or 'No')" << endl;
        cin >> optionstring;
        if (optionstring == "Yes") {
            custom = true;
            break;
        } else if (optionstring == "No") {
            custom = false;
            break;
        } else {
            cout << "Please respond with 'Yes' or 'No'." << endl;
        }
    }
    cout << endl;
    string inputMOS;
    if (custom) {
        while (true) {
            cout << "Would you like your list to contain only movies, only tv shows, or both?" << endl;
            cout << "(Respond with 'Movies', 'Shows', or 'Both')" << endl;
            cin >> inputMOS;
            if (inputMOS == "Movies") {
                moviesorshows = "movie";
                break;
            }
            else if(inputMOS == "Shows") {
                moviesorshows = "tvSeries";
                break;
            }
            else if (inputMOS == "Both") {
                break;
            }
            else {
                cout << "Please respond with either 'Movies', 'Shows', or 'Both'." << endl << endl;
            }
        }
        cout << endl;
        string isAdultString;
        while (true) {
            cout << "Would you like your list to contain adult movies or tv shows?" << endl;
            cout << "(Respond with 'Yes' or 'No')" << endl;
            cin >> isAdultString;
            if (isAdultString == "Yes") {
                isAdult = true;
                break;
            } else if (isAdultString == "No") {
                isAdult = false;
                break;
            } else {
                cout << "Please respond with 'Yes' or 'No'." << endl << endl;
            }
        }
        cout << endl;
        string earliestRelaseDate;
        string initialYearString;
        bool isYear = false;
        while (true) {
            cout << "Does it matter when the movie or tv show originally came out?" << endl;
            cout << "(Respond with 'Yes' or 'No')" << endl;
            cin >> earliestRelaseDate;
            if (earliestRelaseDate == "No") {
                break;
            } else if (earliestRelaseDate == "Yes") {
                while (!isYear) {
                    cout << "What is the earliest release year you want?" << endl;
                    cout << "(Respond with only the year; value must be 2024 or less and contain exactly 4 digits)" << endl;
                    cin >> initialYearString;
                    bool notValidYear = false;
                    for (char num: initialYearString) {
                        if (!isdigit(num)) {
                            notValidYear = true;
                        }
                    }
                    if (notValidYear || initialYearString.length() != 4) {
                        cout << "Please enter a valid year." << endl << endl;
                    } else {
                        initialYear = stoi(initialYearString);
                        isYear = true;
                        break;
                    }
                }
                break;
            }
            else {
                cout << "Please respond with 'Yes' or 'No'." << endl << endl;
            }
        }
        cout << endl;
        string runtimeResponse, runtimeMinutesString;
        while (true) {
            cout << "Does it matter how long the movie or tv show is?" << endl;
            cout << "(Respond with 'Yes' or 'No')" << endl;
            cin >> runtimeResponse;
            if (runtimeResponse == "Yes") {
                bool runTimeBool = false;
                while (!runTimeBool) {
                    cout << "At most, how long can a movie or tv show episode be?" << endl;
                    cout << "(Respond with a length in minutes)" << endl;
                    cin >> runtimeMinutesString;
                    bool notValidRunTime = false;
                    for (char num: runtimeMinutesString) {
                        if (!isdigit(num)) {
                            notValidRunTime = true;
                        }
                    }
                    if (notValidRunTime) {
                        cout << "Please enter in a valid max movie/tv show episode length in minutes." << endl << endl;
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
                cout << "Please respond with 'Yes' or 'No'." << endl << endl;
            }
        }
        cout << endl;
    }
    string jaccardOrCosine;
    while (true) {
        cout << "Would you like to utilize our Jaccard Similarity Algorithm or our Cosine Similarity Algorithm?" << endl;
        cout << "(Respond with 'Jaccard' or 'Cosine')" << endl << endl;
        cout << "Jaccard: Ratio of the length of the intersection within data samples to the length of the union of the data samples." << endl;  // Definition from GeeksForGeeks
        cout << "Cosine: Ratio of the dot product of our two vectors (data samples) to the product of their magnitudes." << endl;  // Definition from GeeksForGeeks
        cin >> jaccardOrCosine;
        cout << endl;
        if (jaccardOrCosine == "Jaccard") {
            cout << "Creating your list, please wait a moment..." << endl << endl;
            ds.PrintTop50Jaccard(name, isAdult, initialYear, moviesorshows, runTime);
            break;
        } else if (jaccardOrCosine == "Cosine") {
            cout << "Creating your list, please wait a moment..." << endl << endl;
            ds.PrintTop50Cosine(name, isAdult, initialYear, moviesorshows, runTime);
            break;
        } else {
            cout << "Please respond with either 'Jaccard' or 'Cosine'." << endl;
        }
    }
}
