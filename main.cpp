#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

struct place {
    int pcode;
    string name;
    double plat;
    double plng;
    double linkDist1 = 0;
    double linkDist2 = 0;
    place* link1 = nullptr;
    place* link2 = nullptr;

    string toString(bool recursive = true)
    {
        stringstream ss;
        ss << name << ": " << pcode << " (" << plat << ", " << plng << ")";

        if (link1 != nullptr && recursive)
        {
             ss << "\n\tlink: " << link1->toString(false)
                << "\n\t\tdistance: " << linkDist1;
        }

        if (link2 != nullptr && recursive)
        {
             ss << "\n\tlink: " << link2->toString(false)
                << "\n\t\tdistance: " << linkDist2;
        }

        return ss.str();
    }

    void CalculateDistances()
    {
        if (link1 != nullptr)
        {
            linkDist1 = 60 * sqrt(pow(plng - link1->plng, 2)
                                + pow(plat - link1->plat, 2));
        }

        if (link2 != nullptr)
        {
            linkDist2 = 60 * sqrt(pow(plng - link2->plng, 2)
                                + pow(plat - link2->plat, 2));
        }
    }

    void RemoveLink(int n)
    {
        if (n == 1)
        {
            link1 = nullptr;
            linkDist1 = 0.0;
        }
        else if (n == 2)
        {
            link2 = nullptr;
            linkDist2 = 0.0;
        }
    }

    void AddLink(place* dst)
    {
        if (link1 == nullptr)
        {
            link1 = dst;
            CalculateDistances();
        }
        else if (link2 == nullptr)
        {
            link2 = dst;
            CalculateDistances();
        }

    }
};

place* GetPlace(int id, place* placeList, int listSize)
{
    cout << id << endl;
    for(int i = 0; i < listSize; i++)
    {
        if (placeList[i].pcode == id) return &placeList[i];
    }

    return nullptr;
}

void ParsePlaces(string, place*, int&);

int main()
{
    int numPlaces = 0;

    fstream file("destinations.txt");
    string dummy;
    while(!file.eof())
    {
        numPlaces++;
        for(int i = 0; i < 6; i++)
            getline(file, dummy);
    }
    file.close();

    place destinations[numPlaces];

    ParsePlaces("destinations.txt", destinations, numPlaces);

    bool active = true;
    int input;
    place* placeTarget = nullptr;

    cout << "Airpost Flight Controller v0.0\n";


    while(active)
    {
        // 1 = remove, 2 = add, 3 = find route
        // todo: modularize, make it look nice

        cin >> input;
        switch(input)
        {
            case 1:
                while(true)
                {
                    cout << "input a place id: "; cin >> input;
                    placeTarget = GetPlace(input, destinations, numPlaces);
                    if (placeTarget != nullptr) break;
                    cout << "bad input\n";
                }

                cout << placeTarget->toString() << endl;

                while(input <= 0 || input >= 3)
                {
                    cout << "input a link id: "; cin >> input;
                }
                break;

            case 2:
                break;

            case 3:
                break;

        }
    }

    return 0;
}

void ParsePlaces(string filename, place* destinations, int& numPlaces)
{
    ifstream file(filename);
    string dummy;

    file.seekg(0, file.beg);
    for(int i = 0; i < numPlaces; i++)
    {
        getline(file, dummy);
        destinations[i].pcode = stoi(dummy);
        getline(file, dummy);
        destinations[i].name = dummy;
        getline(file, dummy);
        destinations[i].plat = stod(dummy);
        getline(file, dummy);
        destinations[i].plng = stod(dummy);

        getline(file, dummy);
        getline(file, dummy);

        cout << destinations[i].toString() << endl;
    }

    file.seekg(0, file.beg);
    for(int i = 0; i < numPlaces; i++)
    {
        getline(file, dummy);
        getline(file, dummy);
        getline(file, dummy);
        getline(file, dummy);

        getline(file, dummy);
        destinations[i].link1 = GetPlace(stoi(dummy), destinations, numPlaces);
        getline(file, dummy);
        destinations[i].link2 = GetPlace(stoi(dummy), destinations, numPlaces);

        destinations[i].CalculateDistances();

        cout << destinations[i].toString() << endl;
    }
    
    file.close();
}
