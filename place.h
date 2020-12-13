#include <iostream>
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
