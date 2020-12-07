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
             ss << "\n\tlink 1: " << link1->toString(false)
                << "\n\t\tdistance: " << linkDist1;
        }

        if (link2 != nullptr && recursive)
        {
             ss << "\n\tlink 2: " << link2->toString(false)
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

class AirpostManager
{
    place* destinations;
    int numPlaces = 0;

    public:
        void ParsePlaces(string filename)
        {
            fstream file("destinations.txt");
            string dummy;
            while(!file.eof())
            {
                numPlaces++;
                for(int i = 0; i < 6; i++)
                    getline(file, dummy);
            }

            file.seekg(0, file.beg);

            destinations = new place[numPlaces];

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

        place* GetPlace(int id, place* placeList, int listSize)
        {
            cout << id << endl;
            for(int i = 0; i < listSize; i++)
            {
                if (placeList[i].pcode == id) return &placeList[i];
            }

            return nullptr;
        }

        void PrintPlaces()
        {
            for(int i = 0; i < numPlaces; i++)
            {
                cout << destinations[i].toString() << endl;
            }
        }

        void ManageRemoveLink()
        {
            place* placeTarget = nullptr;
            int input;
            while(true)
            {
                cout << "input a place id: "; cin >> input;
                placeTarget = GetPlace(input, destinations, numPlaces);
                if (placeTarget != nullptr) break;
                cout << "bad input\n";
            }

            cout << placeTarget->toString() << endl;

            if (placeTarget->link1 == nullptr && placeTarget->link2 == nullptr)
            {
                cout << "current place has no links! aborting.\n";
                return;
            }

            while(true)
            {
                cout << "input a link id: "; cin >> input;
                if (input == 1 && placeTarget->link1 != nullptr)
                {
                    placeTarget->RemoveLink(1);
                    cout << "removed " << placeTarget->name << "\'s first link.\n";
                    break;
                }
                else if (input == 2 && placeTarget->link2 != nullptr)
                {
                    placeTarget->RemoveLink(2);
                    cout << "removed " << placeTarget->name << "\'s second link.\n";
                    break;
                }
                cout << "sorry, try again.\n";
            }
        }

        void ManageAddLink() {}

        void ManageTraceRoute() {}

};



void ParsePlaces(string, place*, int&);


void ManageAddLink();

int main()
{
    AirpostManager manager;

    manager.ParsePlaces("destinations.txt");

    bool active = true;

    cout << "Airpost Flight Controller v0.0\n";

    while(active)
    {
        // 1 = remove, 2 = add, 3 = find route
        // todo: modularize, make it look nice

        cout << "input 1-3, or 4 to exit: (menu wip) : ";

        int input;

        cin >> input;
        switch(input)
        {
            case 1:
                manager.ManageRemoveLink();
                break;

            case 2:
                break;

            case 3:
                break;
            case 4:
                cout << "seeya\n";
                return 0;

        }
    }

    return 0;
}
