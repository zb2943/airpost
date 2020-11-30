#include <iostream>
#include <fstream>

using namespace std;

struct place {
    int pcode;
    string name;
    double plat;
    double plng;
    double linkDist1;
    double linkDist2;
    place* link1;
    place* link2;
    
    void printString()
    {
        cout << name << ": " << pcode << " (" << plat << ", " << plng << ")" << endl; 
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
        
        destinations[i].printString();
    }

    return 0;
}
