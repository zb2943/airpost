#include <iostream>
#include <fstream>

#include "place.h"

using namespace std;

class AirpostManager
{
    place* destinations;
    int numPlaces = 0;

    place* placeTarget = nullptr;
    int input = 0;

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
