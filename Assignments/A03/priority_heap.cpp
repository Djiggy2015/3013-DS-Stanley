//*******************************************************************
//                   Name: Matt Stanley
//                   Date: 10/4/2019
//                   Course: CMPS 3013
//                   Program: A03
//*******************************************************************
// Description: This program reads in a JSON file that is filled with
// information about animals with the use of "json.hpp" and 
// "json_helper.cpp". This animal data is stored in a 2D array of
// animal structures. Once the data has been stored, the priority
// for each animal is calculated based on latitude, longitude, 
// name length, and an adjuster value. With the priority calculated,
// each animal is then sorted into a set of "buckets" where each
// bucket is essentially a pointer to a max heap. Once the animals
// are stored in their respective buckets, each bucket is then
// max heap sorted so the largest values will be at the top. After
// the values are sorted, the top value in each bucket is extracted,
// and the top five animals are printed to an output file. 
//*******************************************************************

#include "json_helper.cpp"
#include <fstream>
#include <iostream>
#include <string>

#define _USE_MATH_DEFINES

#include <math.h>

using namespace std;

// Used to calculate Halversine Distance
const static double EarthRadiusKm = 6372.8;

inline double DegreeToRadian(double angle) {
    return 3.14 * angle / 180.0;
}

// This function will take in a latitude and longitude and turn them
// into a cooridinate point
class Coordinate {
public:
    Coordinate(double latitude, double longitude) : myLatitude(latitude), myLongitude(longitude) {}

    double Latitude() const {
        return myLatitude;
    }

    double Longitude() const {
        return myLongitude;
    }

private:
    double myLatitude;
    double myLongitude;
};

// This function is used to compute the Haversine Distance between
// two coordinate objects. This distance will then be used
// to help calculate the priority
double HaversineDistance(const Coordinate &p1, const Coordinate &p2) {
    double latRad1 = DegreeToRadian(p1.Latitude());
    double latRad2 = DegreeToRadian(p2.Latitude());
    double lonRad1 = DegreeToRadian(p1.Longitude());
    double lonRad2 = DegreeToRadian(p2.Longitude());

    double diffLa = latRad2 - latRad1;
    double doffLo = lonRad2 - lonRad1;

    double computation = asin(sqrt(sin(diffLa / 2) * sin(diffLa / 2) 
    + cos(latRad1) * cos(latRad2) * sin(doffLo / 2) * sin(doffLo / 2)));
    return 2 * EarthRadiusKm * computation;
}

/////////////////////////////////////////////////////////////////////
// Function: getPriority
// Description: This function uses many different values from
// an animal struct in order to calculate the priority of that animal.
// This value will be used to sort the max heaps.
/////////////////////////////////////////////////////////////////////
float getPriority(string animal_name, double lat, double lon, 
float adj, bool val)
{
    Coordinate c1(33.9137, -98.4934);
    Coordinate c2(lat, lon);
    double distance;
    float priority;

    distance = HaversineDistance(c1, c2);

    int Length = animal_name.length();
    
    priority = (EarthRadiusKm - distance) * adj / Length;

    // Invert Priority if validated is true
    if(val == true)
    {
        priority = 1 / priority;
    }

    return priority;
}

struct Animal {
    string animal_name;
    long date;
    double latitude;
    double longitude;
    float adjuster;
    bool validated;
    string version;
    float priority;

    // Default Constructor
    Animal() {
        animal_name = "";
        date = 0;
        latitude = 0.0;
        longitude = 0.0;
        adjuster = 0.0;
        validated = 0;
        version = "";
        priority = 0.0;
    }

    Animal(string name, long _date, double lat, double lon, float adj, bool val, string ver) {
        animal_name = name;
        date = _date;
        latitude = lat;
        longitude = lon;
        adjuster = adj;
        validated = val;
        version = ver;
        priority = 0.0;
    }

    Animal(json j) {
        animal_name = j["animal_name"];
        date = j["date"];
        latitude = j["latitude"];
        longitude = j["longitude"];
        adjuster = j["adjuster"];
        validated = j["validated"];
        version = j["version"];
        priority = getPriority(animal_name, latitude, longitude, adjuster, validated);
    }
};

class AnimalHelper {
private:
    Animal **Animals;
    JsonHelper *J;
    json obj;
    int size;

public:
    AnimalHelper(string filename) {
        J = new JsonHelper(filename);
        size = J->getSize();
        Animals = new Animal *[size];
        for (int i = 0; i < size; i++) {
            obj = J->getNext();
            Animals[i] = new Animal(obj);
        }
    }


    // This function will print out the data that is read in from
    // animals.json.
    void PrintAnimals() {

        for (int i = 0; i < size; i++) {

            cout << Animals[i]->animal_name << " " 
            << Animals[i]->latitude << " " 
            << Animals[i]->longitude << " " 
            << Animals[i]->date << " " 
            << Animals[i]->adjuster << " " 
            << Animals[i]->validated << " " 
            << Animals[i]->version << " "
            << Animals[i]->priority << endl;

        }
    }

    // Accessor function to obtain a pointer to a specific animal
    // structure.
    Animal * getAnimals(int i)
    {
        return Animals[i];
    }

    // Accessor Function to get the date of an animal
    long getDate(int i)
    {
        return Animals[i]->date;
    }
};

class MaxHeap{
private:
    Animal **heap;
    int size;
    int last;

    // return the position of the current node's parent
    int parent(int i)
    {
        return i / 2;
    }

    // return the position of the current node's left child
    int left(int i)
    {
        return i * 2;
    }

    // return the position of the current node's right child
    int right(int i)
    {
        return i * 2 + 1;
    }

public:
    MaxHeap()
    {
        size = 400;
        heap = new Animal *[size];
        last = 0;
        heap[0]->priority = INT32_MAX; // Largest possible value
    }

    // This function will insert a new value at the end of the heap
    void Insert(Animal * ani)
    {
        heap[++last] = ani;
        Bubbleup();
    }

    // This function will move the largest value to the beginning
    // of the heap.
    void Bubbleup()
    {
        int i = last;
        Animal *temp;

        while(heap[parent(i)]->priority < heap[i]->priority)
        {
            //  Do a swap
            temp = heap[parent(i)];
            heap[parent(i)] = heap[i];
            heap[i] = temp;
            i = parent(i);
        }
    }

    /////////////////////////////////////////////////////////////////
    // Function: getLargestChild
    // Description: This function returns the largest child of the
    // current node. It then checks to make sure it is larger
    // than the current node before returning it to bubbledown
    /////////////////////////////////////////////////////////////////
    int getLargestChild(int i)
    {
        int largest = 0;

        // No children
        if(2 * i > last)
        {
            return -1;
        }

        // Check both children for largest
        // If left child is larger
        if(heap[2 * i]->priority > heap[2 * i + 1]->priority)
        {
            // Make sure child is smaller than parent
            if(heap[i]->priority < heap[2 * i]->priority)
            {
                return 2 * i;
            }
            // If child is not smaller return sentinel value
            else
            {
                return -1;
            }
        }
        else
        {
            if(heap[i]->priority < heap[2 * i + 1]->priority)
            {
                return 2 * i + 1;
            }
            else
            {
                return -1;
            }
        }
        
    }

    // This function will perform a classic swap
    void swap(int i, int j)
    {
        Animal *temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;

    }

    /////////////////////////////////////////////////////////////////
    // Function: BubbleDown
    // Description: This function will look at the current node to be
    // evaluated and swap it with its children if they are larger.
    // The swapping will continue, moving down the heap, until no
    // swappable children are found.
    /////////////////////////////////////////////////////////////////
    void BubbleDown(int i)
    {
        int j = getLargestChild(i);

        // If getLargestChild did not find anything
        if(j < 0)
        {
            return;
        }

        if(j != -1)
        {
            swap(i, j);
            BubbleDown(j);
        }
    }


    /////////////////////////////////////////////////////////////////
    // Function: Heapify
    // Description: This function will sort the heap according to 
    // max heap rules. The sorting is accomplished by bubbling down 
    //starting at the value in the middle of the heap, and moving up 
    // until the beginning of the heap is reached.
    /////////////////////////////////////////////////////////////////
    void Heapify()
    {
        int i = (last - 1) / 2;
        for(int j = i; j >= 1; j--)
        { 
            BubbleDown(j);
        }
    }

    /////////////////////////////////////////////////////////////////
    // Function: Print
    // Description: This function will print out the first five 
    // animals in each heap by priority order.
    /////////////////////////////////////////////////////////////////
    void Print(ofstream & outfile)
    {
        for(int i = 1; i <= 5; i++)
        {
              outfile << heap[i]->animal_name << " " 
            << heap[i]->priority << "\n\n";
        }
    }

    /////////////////////////////////////////////////////////////////
    // Function: RemoveMax
    // Description: This function removes the top value in the heap
    // and prints it out before swapping it with the last value 
    // inserted into the heap. The last value is then bubbled down
    // to maintain heap order.
    /////////////////////////////////////////////////////////////////
    void RemoveMax(ofstream & outfile)
    {
         outfile << heap[1]->animal_name << " " 
            << heap[1]->priority << "\n\n";

         heap[1] = heap[last];
         BubbleDown(1);


    }
};

int main(int argc, char **argv) {

    ofstream outfile;
    outfile.open("output.txt");

    // Fill up AH with the data
    AnimalHelper AH("animals.json");

    // Get the number of buckets 
    int NumberOfBuckets = 6;

    // Create a double pointer that will point to an 
    // array of maxheap pointers
    MaxHeap** Buckets = new MaxHeap*[NumberOfBuckets];

    // Create the number of buckets required
    for(int i = 0; i < NumberOfBuckets; i++)
    {
        Buckets[i] = new MaxHeap;
    }

    // Put the animals into their respective buckets
    for (int i = 0; i < 1000; i++)
    {
    int BucketNumber = abs(AH.getDate(i)) % NumberOfBuckets;
    
    Buckets[BucketNumber]->Insert(AH.getAnimals(i));
    }

    // Heapify all of the buckets
    for(int i = 0; i < NumberOfBuckets; i++)
    {
        Buckets[i]->Heapify();
    }

    // Get the max value in each bucket
    outfile << "Output with 6 Heaps:\n\n";
    outfile << "Highest priority in each Heap:\n\n";
    for(int i = 0; i < NumberOfBuckets; i++)
    {
        Buckets[i]->RemoveMax(outfile);
    }

    outfile << "\n\n";
    // Print the first five animals in each bucket
    for(int i = 0; i < NumberOfBuckets; i++)
    {
        outfile << "Heap " << i << endl;
        outfile << "======================\n";
        Buckets[i]->Print(outfile);
    }

    outfile.close();
    return 0;
}
