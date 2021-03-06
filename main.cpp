#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <array>

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

            visitedPlaces.push_back(p);
            tmpPath[pathIndex] = p;
            pathIndex++;

            if (p == dst)
            {
                paths.push_back(visitedPlaces);
                currentRoute++;
            }
            else
            {
                if (p->link1 != nullptr && !contains(visitedPlaces, p->link1))
                    SearchNeighbors(p->link1, src, dst);
                if (p->link2 != nullptr && !contains(visitedPlaces, p->link2))
                    SearchNeighbors(p->link2, src, dst);
            }

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

            cout << endl << placeTarget->toString() << endl;

            while(true)
            {
                cout << "input your destination's id: "; cin >> input;
                if (input == -1) return;

                destinationTarget = GetPlace(input, destinations, numPlaces);
                if (destinationTarget != nullptr) break;
                cout << "\tbad input\n";
            }

            cout << endl << destinationTarget->toString() << endl;

            paths.clear();
            currentRoute = 0;
            tmpPath.reserve(numPlaces); // might not be necessary
            for(int i =  0; i < numPlaces; i++)
            {
                tmpPath.push_back(nullptr);
            }
            pathIndex = 0;

            SearchNeighbors(placeTarget, placeTarget, destinationTarget);

            cout << "found routes: \n";

            // sort by hop count
            for(size_t i = 0; i < paths.size()-1; i++)
            {
                for(size_t j = 0; j < paths.size()-i-1; j++)
                {
                    if (paths[j].size() > paths[j+1].size())
                    {
                        vector<place*> tmp = paths[j];
                        paths[j] = paths[j+1];
                        paths[j+1] = tmp;
                    }
                }
            }

            cout << "\tsorted by hops: \n";

            for(size_t i = 0; i < paths.size(); i++)
            {
                cout << "\t\t";
                for(size_t j = 0; j < paths[i].size(); j++)
                {
                    cout << paths[i][j]->name;
                    if (j < paths[i].size()-1) cout << " -> ";
                }
                cout << "; " << paths[i].size() << " hop";
                if (paths[i].size() > 1) cout << 's';
                cout << endl;
            }

            cout << "\tsorted by distance: \n";

            // add distances together
            // distances are stored in a separate array and synced with the paths
            double distances[paths.size()];
            for(size_t i = 0; i < paths.size(); i++)
            {
                for(size_t j = 0; j < paths[i].size()-1; j++)
                {
                    if (paths[i][j]->link1 == paths[i][j+1])
                        distances[i] += paths[i][j]->linkDist1;
                    else if (paths[i][j]->link2 == paths[i][j+1])
                        distances[i] += paths[i][j]->linkDist2;
                }
            }

            // sort based on distances
            for(size_t i = 0; i < paths.size()-1; i++)
            {
                for(size_t j = 0; j < paths.size()-i-1; j++)
                {
                    if (distances[j] > distances[j+1])
                    {
                        double tmpDist = distances[j];
                        distances[j] = distances[j+1];
                        distances[j+1] = tmpDist;

                        vector<place*> tmp = paths[j];
                        paths[j] = paths[j+1];
                        paths[j+1] = tmp;
                    }
                }
            }


            for(size_t i = 0; i < paths.size(); i++)
            {
                cout << "\t\t";
                for(size_t j = 0; j < paths[i].size(); j++)
                {
                    cout << paths[i][j]->name;
                    if (j < paths[i].size()-1) cout << " -> ";
                }
                cout << "; " << distances[i] << " m\n";
            }

            cout << endl << endl;
            return;
        }

        void PrintHelp()
        {
            cout << "The main menu takes in the numbers 1-5 to select an operation.\n"
                 << "    1: Remove a link from an airport.\n"
                 << "    2: Add a link to an airport.\n"
                 << "    3: Trace all possible routes from one airport to another.\n"
                 << "    4: Print all the airports, their links, and other data.\n"
                 << "    5: Print this help message.\n\n"
                 << "   (protip: you can escape from most menus by inputting -1)\n\n\n";
        }
};

int main()
{
    AirpostManager manager;

    manager.ParsePlaces("destinations.txt");

    bool active = true;

    cout << "Airpost Flight Controller v1.0\n"
         << "You can input \'5\' for a list of commands.\n\n";


    while(active)
    {

        cout << "input 1-5, or 9 to exit: ";

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

            case 5:
                manager.PrintHelp();
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
