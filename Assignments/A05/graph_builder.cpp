//*******************************************************************
//                      Matt Stanley
//                   Assignment 3 - Graphs
//                     October 28 2019
//*******************************************************************
// Description: This program reads in a json file of cities and 
// stores them into an array. Once loaded, the array of cities is 
// looped through city by city. For each city, the closest cities
// to the current city is determined by calculating the priority 
// of the other cities in regards to the current city through
// the haversine formula. With the haversine distance calculated 
// for each city, the cities are inserted into a min heap, which 
// will ensure the closest cities are at the top. The closest
// cities are then printed out based on the number entered in the
// command line. Finally, the stack is emptied to prepare for the 
// next city in the array.
//*******************************************************************
#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <string>
#include "heap.hpp"
#include "JsonFacade.hpp"
#include "json.hpp"
#include <cmath> // for M_PI

using namespace std;

using json = nlohmann::json;

/////////////////////////////////////////////////////////////////////
// Name: Haversine
// Parameters: the latitude and longitude of the first city, and 
// the latitude and longitude of the second.
// Description: This function calculates the haversine distance 
// between two cities through the haversine formula. This 
// value will be used as the priority in a min heap.
/////////////////////////////////////////////////////////////////////
static double haversine(double lat1, double lon1, double lat2, 
double lon2) { 
    //  calculate distance between latitudes 
    // and longitudes 
    double dLat = (lat2 - lat1) * 
                  M_PI / 180.0; 
    double dLon = (lon2 - lon1) *  
                  M_PI / 180.0; 

    // convert to radians 
    lat1 = (lat1) * M_PI / 180.0; 
    lat2 = (lat2) * M_PI / 180.0; 

    // apply formulas
    double a = pow(sin(dLat / 2), 2) +  
                pow(sin(dLon / 2), 2) *  
                cos(lat1) * cos(lat2); 
    double rad = 6371; 
    double c = 2 * asin(sqrt(a)); 
    return rad * c; 
} 

struct City{
  string name;
  double growth;
  double lat;
  double lon;
  int population;
  int rank;
  string state;
  int id;
  double priority;

  // Default city constructor
  City(){
    id = 0;
    name = "";
    growth = 0.0;
    lat = 0.0;
    lon = 0.0;
    population = 0;
    rank = 0;
    state = "";
    priority = 0.0;
    
  }

  // Programmer defined constructor
  City(json obj){
    name = obj["city"];
    growth = obj["growth"];
    lat = obj["latitude"];
    lon = obj["longitude"];
    population = obj["population"];
    rank = obj["rank"];
    state = obj["state"];
    priority = 0.0;
  
  }

};

int main(int argc, char * argv[]){
    // Define necessary variables
    json obj;
    City** Cities;
    City* temp;
    string filename = "cities.json";
    JsonHelper J(filename);
    double lat1;
    double lon1;
    double lat2;
    double lon2;

    ofstream outfile;
    outfile.open("closest_cities.txt");

    // Get the number of closest cities from the command line
    int closest = atoi(argv[1]);

    outfile << "Matt Stanley\n";
    outfile << "Program 3 - Closest Cities\n";
    outfile << "Closest " << closest << " cities\n\n";


    // Create a minimum heap of 1000 cities 
    Heap <City> H(1000,false);

    // Set size equal to the json array size
    int size = J.getSize();

    // loads an array of cities with the json data
    Cities = new City *[size];
    for (int i = 0; i <size; i++) {
        obj = J.getNext();
        Cities[i] = new City(obj);
    }

    // Loop through the cities
    for(int i = 0; i < size; i++)
    {
      // Only print out the closest cities for the first 10
      // and last 10 cities
      if(i < 10 || i >= size - 10)
      {
      lat1 = Cities[i]->lat;
      lon1 = Cities[i]->lon;

      outfile << i+1 << " " << Cities[i]->name << "\n";
      outfile << "------------------------------\n";

      // Loop through the cities and calculate their priority
      // in regards to the current city being looked at
      // then put them into a min heap.
      for(int j = 0; j < size; j++)
      {
        // This if loop will make sure the current city isn't
        // compared to itself
        if(Cities[i]->name != Cities[j]->name)
        {
          lat2 = Cities[j]->lat;
          lon2 = Cities[j]->lon;
          Cities[j]->priority = haversine(lat1, lon1, lat2, lon2);

          H.Insert(Cities[j]);
        }

      }

      // Print out the cities that are closest to the 
      // current city being evaluated
      for(int i = 0; i < closest; i++)
      {
        temp = H.Extract();
        outfile << i+1 << ") " << temp->name << " " << temp->priority << endl;
      }

      outfile << endl;

      // Empty the heap
      while(!H.Empty())
      {
        temp = H.Extract();
      }
     }
    }

    outfile.close();
}
