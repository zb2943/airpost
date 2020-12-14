#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "place.h"

using namespace std;

bool contains(vector<place*>& v, place* p)
{
    return find(v.begin(), v.end(), p) != v.end();
}

class AirpostManager
{
    place* destinations;
    int numPlaces = 0;

    place* placeTarget = nullptr;
    int input = 0;

    // for route tracing
    vector<place*> visitedPlaces;
    vector<vector<place*>> paths;

    vector<place*> tmpPath;
    int pathIndex = 0;

    place* lastValidPlace;
    int currentRoute;
    int maxRoutes;

    public:
        vector<place*> GetInputs(place* p)
        {
            vector<place*> inputs;

            for(int i = 0; i < numPlaces; i++)
            {
                if (destinations[i].link1 == p || destinations[i].link2 == p)
                {
                    inputs.push_back(&destinations[i]);
                }
            }

            return inputs;
        }

        void SearchNeighbors(place* p, place* src, place* dst)
        {
            // src points to the initial place, i.e. NYC in a NYC -> SF route
            // dst points to the destination place, i.e. SF

            // given a source, look at its links
            // see if there's an link that leads to the destination,
            // and if so then create a new path with what you've gathered
            // if not,
            //     and the link leads to somewhere you haven't been before
            //     searchNeighbors that new place (and add it to your visited places)
            // if you hit a dead end (all places are in visitedPlaces), go back a step

            cout << 1 << endl;
            visitedPlaces.push_back(p);
            cout << "pathIndex: " << pathIndex << endl << "tmpPlaces size: " << tmpPath.size() << endl;
            for(size_t i = 0; i < tmpPath.size(); i++)
            {
                if (tmpPath[i] == nullptr)
                {
                    cout << "nullptr, ";
                } else cout << tmpPath[i]->name << ", ";
            }
            tmpPath[pathIndex] = p;
            pathIndex++;

            cout << "TESTING STATEMENT: " << (p->name) << endl;
            if (p == dst)
            {
                cout << "found destination! (" << p->name << " == " << dst->name << ")" << endl;
                paths.push_back(visitedPlaces);
                currentRoute++;
            }
            else
            {
                cout << p->name << endl;
                cout << "visited places size: " << visitedPlaces.size() << endl;

                if (p->link1 != nullptr && !contains(visitedPlaces, p->link1))
                    SearchNeighbors(p->link1, src, dst);
                if (p->link2 != nullptr && !contains(visitedPlaces, p->link2))
                    SearchNeighbors(p->link2, src, dst);

            }

            cout << 4 << endl;

            visitedPlaces.pop_back();
            pathIndex--;
        }

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
            }

            file.close();
        }

        place* GetPlace(int id, place* placeList, int listSize)
        {
            for(int i = 0; i < listSize; i++)
            {

                if (placeList[i].pcode == id)
                {
                    cout << id << " COMP " << placeList[i].pcode << endl;
                    return &placeList[i];
                }
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

        void UIRemoveLink()
        {
            placeTarget = nullptr;
            while(true)
            {
                cout << "\ninput a place id: "; cin >> input;
                if (input == -1) return;
                placeTarget = GetPlace(input, destinations, numPlaces);
                if (placeTarget != nullptr) break;
                cout << "\tbad input\n";
            }

            cout << placeTarget->toString() << endl;

            if (placeTarget->link1 == nullptr || placeTarget->link2 == nullptr)
            {
                cout << "\tA destination cannot have less than one link at a time. aborting.\n";
                return;
            }

            while(true)
            {
                int incomingLinks = 0;
                cout << "input a link id: "; cin >> input;
                if (input == -1) return;
                if (input == 1 && placeTarget->link1 != nullptr)
                {
                    for (int i = 0; i < numPlaces; i++)
                    {
                        if (destinations[i].link1 == placeTarget->link1 ||
                            destinations[i].link2 == placeTarget->link1)
                        {
                            incomingLinks++;
                        }
                    }
                    if (incomingLinks <= 1) // this should never be < 1
                    {
                        cout << "cannot remove link to " << placeTarget->link1->name << ": this is the only link to it!\n";
                        return;
                    }
                    placeTarget->RemoveLink(1);
                    cout << "removed " << placeTarget->name << "\'s first link.\n";
                    break;
                }
                else if (input == 2 && placeTarget->link2 != nullptr)
                {
                    for (int i = 0; i < numPlaces; i++)
                    {
                        if (destinations[i].link1 == placeTarget->link2 ||
                            destinations[i].link2 == placeTarget->link2)
                        {
                            incomingLinks++;
                        }
                    }
                    if (incomingLinks <= 1) // this should also never be < 1
                    {
                        cout << "cannot remove link to " << placeTarget->link2->name << ": this is the only link to it!\n";
                        return;
                    }
                    placeTarget->RemoveLink(2);
                    cout << "removed " << placeTarget->name << "\'s second link.\n";
                    break;
                }
                cout << "\tsorry, try again.\n";
            }
        }

        void UIAddLink()
        {
            placeTarget = nullptr;
            place* addTarget = nullptr;

            while(true)
            {
                cout << "input a place id: "; cin >> input;
                if (input == -1) return;

                placeTarget = GetPlace(input, destinations, numPlaces);
                if (placeTarget != nullptr) break;
                cout << "\tbad input\n";
            }

            cout << placeTarget->toString() << endl;

            if (placeTarget->link1 != nullptr && placeTarget->link2 != nullptr)
            {
                cout << "warn: " << placeTarget->name << " already has two links established.\nplease choose one to overwrite: ";
                while(true)
                {
                    cin >> input;
                    if (input == -1) return;
                    if (input > 0 && input < 3)
                    {
                        placeTarget->RemoveLink(input);
                        cout << "\tremoved link " << input << endl;
                        break;
                    }
                }
            }

            while(true)
            {
                cout << "link created! \n\twhere are you linking this?\n\t(input a place id): "; cin >> input;
                if (input == -1) return;

                addTarget = GetPlace(input, destinations, numPlaces);
                if (addTarget->pcode == placeTarget->pcode)
                {
                    cout << "\tnice try, but you can't link a place to itself.\n";
                    addTarget = nullptr;
                }
                if (addTarget != nullptr) break;
                cout << "\tbad input\n";
            }

            placeTarget->AddLink(addTarget);

            cout << "Linked " << addTarget->name << " to " << placeTarget->name << endl << endl;

        }

        void UITraceRoute()
        {

            placeTarget = nullptr;
            place* destinationTarget = nullptr;

            while(true)
            {
                cout << "input a place id: "; cin >> input;
                if (input == -1) return;

                placeTarget = GetPlace(input, destinations, numPlaces);
                if (placeTarget != nullptr) break;
                cout << "\tbad input\n";
            }

            cout << placeTarget->toString() << endl;

            while(true)
            {
                cout << "input your destination's id: "; cin >> input;
                if (input == -1) return;

                destinationTarget = GetPlace(input, destinations, numPlaces);
                if (destinationTarget != nullptr) break;
                cout << "\tbad input\n";
            }

            cout << destinationTarget->toString() << endl;

            //vector<place> visitedPlaces; //  this is for places you've visited in a single session
            //place* lastValidPlace; // this is for the last valid place you've visited, with room to explore more
            //vector<vector<place>> prevPaths; // this is for paths you take
            // run this recursive func for only the amt of times you get a complete trip, i.e. you run into the
            // source place

            paths.clear();
            currentRoute = 0;
            tmpPath.reserve(numPlaces);
            for(int i =  0; i < numPlaces; i++)
            {
                tmpPath.push_back(nullptr);
            }
            pathIndex = 0;

            SearchNeighbors(placeTarget, placeTarget, destinationTarget);

            cout << "found routes: \n";

            for(size_t i = 0; i < paths.size(); i++)
            {
                cout << "\t";
                for(size_t j = paths[i].size()-1; j > 0; j--)
                {
                    cout << paths[i][j]->name;
                    if (j > 0) cout << " -> ";
                }
                cout << paths[i][0]->name << endl;
            }

            return;
        }

};

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

        cout << "input 1-4, or 9 to exit: (menu wip) : ";

        int input;

        cin >> input;
        switch(input)
        {
            case 1:
                manager.UIRemoveLink();
                break;

            case 2:
                manager.UIAddLink();
                break;

            case 3:
                manager.UITraceRoute();
                break;

            case 4:
                manager.PrintPlaces();
                break;

            case 9:
                cout << "seeya\n";
                return 0;

            default:
                cout << "\tinvalid id";
                break;

        }
    }

    return 0;
}
