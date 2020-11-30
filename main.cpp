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
};

int main()
{
    int numPlaces;
    ifstream file("destinations.txt");
    
    string dummy;
    while(!file.eof())
    {
        numPlaces++;
        for(int i = 0; i < 6; i++)
            getline(file, dummy);
    }
    
    place destinations[numPlaces];
    
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
        destinations[i].plat = stod(dummy);
    }
    
    return 0;
}
