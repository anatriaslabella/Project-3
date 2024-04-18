#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <string>
#include <set>
#include <algorithm>
#include <utility>
using namespace std;

struct Vertex {
    string name;
    string titleType;
    bool isAdult;
    string startYear;
    string endYear;
    string runtimeMinutes;
    string genre_list[3];
    vector<string> directors;
    float averageRating;

    Vertex() {
        name = "";
        titleType = "";
        isAdult = false;
        startYear = "0000";
        endYear = "0000";
        runtimeMinutes = "0";
        averageRating = 0.0;
    }

    Vertex(string name, string& titleType, bool isAdult, string startYear, string endYear, string runtimeMinutes, string genres) {
        this->name = name;
        this->titleType = titleType;
        this->isAdult = isAdult;
        this->startYear = startYear;
        this->endYear = endYear;
        this->runtimeMinutes = runtimeMinutes;
        istringstream stream(genres);
        string temp_genre;
        int i = 0;
        while(getline(stream,temp_genre,',') && i < 3){
            this->genre_list[i] = temp_genre;
            i++;
        }
    }

    void ShoutName(){
        cout << name;
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
//            while(getline(file, line))
            for(int i = 0; i < 50; i++){
                getline(file,line);
//                getline(file, singleLine);
//                istringstream stream(singleLine);
//                getline(stream, ignore, '\t');
//                getline(stream, titleType, '\t');
//                cout << titleType;
//                getline(stream, primaryTitle, '\t');
//                cout << primaryTitle;
//                getline(stream, ignore, '\t');
//                getline(stream, isAdultString, '\t');
//                isAdult = stoi(isAdultString);
//                getline(stream, startYearString, '\t');
//                startYear = stoi(startYearString);
//                getline(stream, endYearString, '\t');
//                getline(stream, runtimeMinutesString, '\t');
//                runtimeMinutes = stoi(runtimeMinutesString);
//                getline(stream, genre, '\n');
//                cout << genre;
//                if (!genre.find(',')) {
//                    genre1 = genre;
//                }
//                else {
//                    genre1 = genre.substr(0, genre.find(','));
//                    genre.erase(genre1.begin(), genre1.end());
//                    if (!genre.find(',')) {
//                        genre2 = genre;
//                    }
//                    else {
//                        genre2 = genre.substr(0, genre.find(','));
//                        genre.erase(genre2.begin(), genre2.end());
//                        genre3 = genre;
//                    }
//                }
//                graph.emplace(primaryTitle, Vertex(titleType, isAdult, startYear, endYear, runtimeMinutes));
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
                graph.emplace(primaryTitle, Vertex(primaryTitle, titleType, isAdult, startYear, endYear, runtimeMinutes, genre));
            }
            file.close();
        }
    }
};

map<float, vector<Vertex>> jaccard(Graph g, string movie_name){
    Vertex movie = g.GiveGraph()[movie_name];
    set<string> inputed_movie;
    map<float, vector<Vertex>> similar_movies;
    for (int i = 0; i < 3; i++) {
        inputed_movie.insert(movie.genre_list[i]);
    }
    map <string, Vertex> list_of_stuff = g.GiveGraph();
    for(auto iter = list_of_stuff.begin(); iter != list_of_stuff.end(); iter++){ // iterate through graph, compare each values' genre list to the given movie's genre list
        set<string> iterated_movie;
        for(int i = 0; i < 3; i++){
            iterated_movie.insert(iter->second.genre_list[i]);
        }
        vector<string> union_genres;
        vector<string> intersection_genres;
        auto it = set_union(inputed_movie.begin(), inputed_movie.end(), iterated_movie.begin(), iterated_movie.end(), union_genres.begin());
        auto it2 = set_intersection(inputed_movie.begin(), inputed_movie.end(), iterated_movie.begin(), iterated_movie.end(), intersection_genres.begin());
        float similarity = intersection_genres.size() / union_genres.size();
        vector<Vertex> temporary = similar_movies[similarity];
        temporary.push_back(iter->second);
        similar_movies[similarity] = temporary;
    }
    return similar_movies;
}

void PrintTop20(map<float, vector<Vertex>> movies){
    int counter = 0;
    for(auto iter = movies.end(); iter != movies.begin(); iter--){
        for(int i = 0; i < iter->second.size(); i++) {
            if(counter == 20){
                return;
            }
            iter->second[i].ShoutName();
            counter++;
        }
    }
}

int main() {
    Graph g;
    cout << "hello" << endl;
    ifstream myfile("filtered_titles.tsv");
    g.read(myfile);
    cout << "here" << endl;
    g.GraphSize();
    map <float, vector<Vertex>> temp = jaccard(g, "Carmencita");
    cout << "here";
    PrintTop20(temp);
    return 0;
}
