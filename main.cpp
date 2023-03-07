#include <iostream>
#include <iomanip>
#include <fstream>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

// prototype functions
void printRatings(vector<string> bl, map<string, vector<double> > rm);                                                          // debug function to print input data
vector<pair<double, string> > average(vector<string> bl, map<string, vector<double> > rm);                                      // returns the average rating of every book
vector<pair<double, string> > similarities(string user, map<string, vector<double> > rm);                                       // vector of how similar each person is to user with dot product
vector<pair<double, string> > averageSim(vector<string> bl, vector<pair<double, string> > sim, map<string, vector<double> > rm); // average ratings of top 3 similar users to return book list

int main(int argc, char *argv[])
{
    // file opening
    if (argc != 2)
    {
        cout << "Error no file given via command line!" << endl;
        return 1;
    }

    ifstream inputFile(argv[1]);
    if (!inputFile.is_open())
    {
        cout << "Error opening file!" << endl;
        return 1;
    }

    // adds all books from file to bookSet
    string line;
    set<string> bookSet; // set so no duplicates
    int count = 1;
    while (getline(inputFile, line))
    {
        if (count % 3 == 2)
        { // only lines books are on
            bookSet.insert(line);
        }
        count++;
    }

    // bookSet to booksList(vector) (in project specifications)
    vector<string> booksList(bookSet.begin(), bookSet.end());

    // map of person name and their rating, index or rating corresponds to the index of book it belongs to
    map<string, vector<double> > ratingsMap;
    inputFile.clear(); // file to start again
    inputFile.seekg(0, ios::beg);

    string name, book, ratingStr;
    double rating, bookIndex;
    while (getline(inputFile, name))//gets the name, book, and rating
    {
        getline(inputFile, book);
        getline(inputFile, ratingStr);
        rating = stod(ratingStr); // string to double 

        if (ratingsMap.count(name) == 0) // if the name is not in the map, add the name and a vector of 0s size of bookList
        {
            ratingsMap[name] = vector<double>(booksList.size(), 0); 
        }
        bookIndex = find(booksList.begin(), booksList.end(), book) - booksList.begin(); // where the book is in book vector (change to function later)
        ratingsMap[name][bookIndex] = rating; 
    }
    // printRatings(booksList, ratingsMap); // for debug how file is read in
    const vector<pair<double, string> > ratingAverage = average(booksList, ratingsMap); //wont change so only needed onces at start
    string userInput;
    do
    {
        cin >> userInput;
        if (userInput == "recommend") // recommend option
        {
            string userName;
            cin >> userName;
            if (ratingsMap.count(userName) == 0) // if the name is not in list print average
            {
                for (auto const &[rating, book] : ratingAverage) // print
                {
                    cout << book << " " << rating << endl;
                }
            }
            else 
            {
                vector<pair<double, string> > userSimilarities = similarities(userName, ratingsMap); // find the most similar users 
                vector<pair<double, string> > userSimAvg = averageSim(booksList, userSimilarities, ratingsMap); // highest rated books from top 3 users

                for (auto const &[rating, book] : userSimAvg) // print
                {
                    if (rating > 0)
                    {
                        cout << book << " " << rating << endl;
                    }
                }
            }
        }
        else if (userInput == "averages") // averages option
        {
            for (auto const &[rating, book] : ratingAverage) // print
            {
                cout << book << " " << rating << endl;
            }
        }
    } while (userInput != "quit"); // quit option
    return 0;
}

void printRatings(vector<string> bl, map<string, vector<double> > rm) // debug function to print file input
{
    cout << right << setw(10) << ' ';
    for (string b : bl)
    {
        cout << setw(13) << b << " ";
    }
    cout << endl;
    for (auto const &[person, ratings] : rm)
    {
        cout << right << setw(8) << person << ": ";
        for (double rating : ratings)
        {
            cout << right << setw(13) << rating << " ";
        }
        cout << endl;
    }
}

// average rating for each book from all non 0 ratings
vector<pair<double, string> > average(vector<string> bl, map<string, vector<double> > rm) 
{
    vector<pair<double, string> > ratingAvg;
    int index = 0; 
    for (string book : bl) 
    {
        double count = 0, avg = 0; // count: number of non 0, avg: running total or ratings
        for (auto const &[person, ratings] : rm)
        {
            if (ratings[index] != 0) // if non 0 rating
            {
                avg += ratings[index];
                count++;
            }
        }
        ratingAvg.push_back(make_pair(avg / count, book)); // divide to find avg
        index++;
    }
    sort(ratingAvg.rbegin(), ratingAvg.rend()); // sort highest first
    return ratingAvg;
}

// finds the most similar names to user in order 
vector<pair<double, string> > similarities(string user, map<string, vector<double> > rm)
{
    vector<pair<double, string> > similarList;
    double sum;
    for (auto const &[person, ratings] : rm)
    {
        sum = 0;
        if (person != user)
        {
            for (auto it_map = rm[user].cbegin(), it_vec = ratings.begin(); it_vec != ratings.end(); ++it_map, ++it_vec)
            {
                sum += (*it_map * *it_vec);
            }
            similarList.push_back(make_pair(sum, person));
        }
    }
    sort(similarList.rbegin(), similarList.rend());
    return similarList;
}

// takes top 3 most similar names to user, averages the books that are non 0, retuns best books in order
vector<pair<double, string> > averageSim(vector<string> bl, vector<pair<double, string> > sim, map<string, vector<double> > rm)
{
    vector<pair<double, string> > avgSimList;
    int count;
    string name;
    for (int bookI = 0; bookI < bl.size(); bookI++) // for each book
    {
        avgSimList.push_back(make_pair(0, bl[bookI])); // list of pair rating 0, book name
        count = 0;
        for (int i = 0; i < 3; i++)
        {
            name = sim[i].second;
            if (rm[name][bookI] != 0) // for the top 3 that did not rate it 0, add rating to list
            {
                avgSimList[bookI].first += rm[name][bookI];
                count++;
            }
        }
        if (count != 0)
        {
            avgSimList[bookI].first = (avgSimList[bookI].first) / count; // if it was rated find the average 
        }
    }
    sort(avgSimList.rbegin(), avgSimList.rend()); // sort highest first
    return avgSimList;
}
