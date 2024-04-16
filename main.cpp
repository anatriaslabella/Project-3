#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <string>
using namespace std;

struct Vertex {
    string titleType;
    bool isAdult;
    int startYear;
    int endYear;
    int runtimeMinutes;
    string genres[3];
    vector<string> directors;
    float averageRating;

    Vertex() {
        titleType = "";
        isAdult = false;
        startYear = 0000;
        endYear = 0000;
        runtimeMinutes = 0;
        averageRating = 0.0;
    }

    Vertex(string& titleType, bool isAdult, int startYear, int endYear, int runtimeMinutes) {
        this->titleType = titleType;
        this->isAdult = isAdult;
        this->startYear = startYear;
        this->endYear = endYear;
        this->runtimeMinutes = runtimeMinutes;
    }
};

class Graph {
    map<string, Vertex> graph;
public:
    void GraphSize(){
        cout << graph.size();
    }

    void read(ifstream& file) {
        string singleLine;
        string primaryTitle;
        string ignore;
        string titleType;
        bool isAdult;
        string isAdultString;
        int startYear;
        string startYearString;
        string endYearString;
        int endYear;
        string runtimeMinutesString;
        int runtimeMinutes;
        string genres[3];
        string genre;
        string genre1;
        string genre2;
        string genre3;
        int size = 9916880;

        if (file.is_open()) {
            getline(file, singleLine);
            istringstream stream(singleLine);
            getline(stream, ignore, '\t');
            getline(stream, titleType, '\t');
            getline(stream, primaryTitle, '\t');
            getline(stream, ignore, '\t');
            getline(stream, isAdultString, '\t');
            getline(stream, startYearString, '\t');
            getline(stream, endYearString, '\t');
            getline(stream, runtimeMinutesString, '\t');
            getline(stream, genre, '\n');
            for (int i = 0; i < size; i++) {
                getline(file, singleLine);
                istringstream stream(singleLine);
                getline(stream, ignore, '\t');
                getline(stream, titleType, '\t');
                getline(stream, primaryTitle, '\t');
                getline(stream, ignore, '\t');
                getline(stream, isAdultString, '\t');
                isAdult = stoi(isAdultString);
                getline(stream, startYearString, '\t');
                startYear = stoi(startYearString);
                getline(stream, endYearString, '\t');
                getline(stream, runtimeMinutesString, '\t');
                runtimeMinutes = stoi(runtimeMinutesString);
                getline(stream, genre, '\n');
                if (!genre.find(',')) {
                    genre1 = genre;
                }
                else {
                    genre1 = genre.substr(0, genre.find(','));
                    genre.erase(genre1.begin(), genre1.end());
                    if (!genre.find(',')) {
                        genre2 = genre;
                    }
                    else {
                        genre2 = genre.substr(0, genre.find(','));
                        genre.erase(genre2.begin(), genre2.end());
                        genre3 = genre;
                    }
                }
                graph.emplace(primaryTitle, Vertex(titleType, isAdult, startYear, endYear, runtimeMinutes));
            }
        }
        myfile.close();
    }
};

int main() {
    Graph g;
    cout << "hello" << endl;
    ifstream myfile("title.basics.tsv");
    g.read(myfile);
    if(!myfile.is_open()){
        cout << "naw";
    }
    g.GraphSize();
    return 0;
}
