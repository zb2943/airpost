#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

struct place {
    int pcode;
    string name;
    double plat;
    double plng;
    double linkDist1;
    double linkDist2;
    place* link1 = nullptr;
    place* link2 = nullptr;
    
    string toString(bool recursive = true)
    {
        stringstream ss;
        ss << name << ": " << pcode << " (" << plat << ", " << plng << ")";
        
        if (link1 != nullptr && recursive)
        {
             ss << "\n\tlink: " << link1->toString(false);
        }
        
        if (link2 != nullptr && recursive)
        {
             ss << "\n\tlink: " << link2->toString(false);
        }
        
        return ss.str();
    }
};

place* GetPlace(int id, place* placeList, int listSize)
{
    for(int i = 0; i < listSize; i++)
    {
        if (placeList[i].pcode == id) return &placeList[i];
    }
    
    return nullptr;
}

int main()
{
    int numPlaces = 0;
    ifstream file("destinations.txt");
    
    string dummy;
    while(!file.eof())
    {
        numPlaces++;
        for(int i = 0; i < 6; i++)
            getline(file, dummy);
    }
    
    place destinations[numPlaces];
    
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
        
        cout << destinations[i].toString() << endl;
    }

    return 0;
}
