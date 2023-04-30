/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   globals.h
 * Author: Nick, Adam, and Ridvik
 *
 * Created on February 22, 2023, 11:18 p.m.
 */

/* globals.h - Nick
 *
 * Define all libraries and header files used for each milestone
 * Define all structs used for global variables
 * Define all global variables with extern, referencing their definitions in m1, such that they can be used in future milestones
 *
 * Ridvik - No need for globals.cpp file because we don't have explicit constructors anymore
 */

#ifndef GLOBALS_H
#define GLOBALS_H

/* std libraries */
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <set>
#include <utility>
#include <chrono>
#include <sstream>
#include <numeric>
#include <thread>
#include <queue>
#include <limits>
#include <list>

/* ezgl libraries */
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/camera.hpp"
//#include "ezgl/color.hpp"

/* project header files */
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "m4.h"
#include "m2_drawMapElements.h"
#include "m2_drawMapNames.h"
#include "m2_drawMapHighlights.h"
#include "m2_PartialNames.h"
#include "m2_Project.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"

/* Define Infinity for Dijkstra */
const double infinity = 9999999;

/* Define Structs */

// Define a struct to hold POI information
struct POIStruct {
    // Define members for POIStruct
    std::string type;   // type of the POI; e.g. "restaurant", "tourist trap", etc.
    std::string name; // name of the POI
    LatLon poiLoc;  // location of the POI defined by the LatLon class
    ezgl::point2d poiXYLoc;
    POIIdx index; // this is needed for tracking POIs in the closest POI function.
    bool poiHighlight;
    bool whichPOIEntry;
};

// Define a struct to hold intersection information
struct IntersectStruct {
    // Define members for IntersectStruct
    LatLon intersectLoc; // location of the intersection
    ezgl::point2d intersectXYLoc;
    IntersectionIdx interIndex; // needed for tracking intersections in the closest intersection function.
    bool intersectHighlight = false;
    bool displayed = false;   // counter for how many times
    // create a vector that stores adjacent intersections.
    //These are all intersections that can be reached by going down any one of the street segments that connect to the intersection
    std::vector<IntersectionIdx> adjacentIntersections;
    // create a vector that stores connected Street Segments
    std::vector<StreetSegmentIdx> connectedStreetSegments;
};

// Define a struct to hold street information
struct StreetStruct {
    // Define members of StreetStruct
    double streetLength = 0; // length of the street
    StreetIdx streetIndex; // index of the street
    std::string streetName; // name of the street
    bool streetHighlight;
    bool whichStreetEntry;
    std::vector<StreetSegmentIdx> segsOfStreet; // a vector of all the ids of street segments connected to a street
    std::vector<IntersectionIdx> intersectsOfStreet; // a vector of all the ids of all intersections connected to a street
};

enum StreetSegType{
    DEFAULT = 0,
    PRIMARY = 1,
    MOTORWAY = 2,
    TRUNK = 2,
    MOTORWAY_LINK = 3,
    TRUNK_LINK = 3,
    SECONDARY = 4,
    MINOR = 5,
    SUBMINOR = 6
};

// Define a struct to hold street segment information
struct SegStruct {
    // Define members for SegStruct
    StreetSegmentIdx segId;
    OSMID wayID;
    double segLength = 0; // length of the street segment
    double segSpeedLimit = 0; // speed limit of the street segment
    double segTravelTime = 0;
    StreetIdx myStreet; // id of the street associated with the street segment
    IntersectionIdx from, to; // ids of the 2 intersections which this segment spans
    int numOfCurvePoints = 0;
    std::vector<ezgl::point2d> curvePoints;
    bool oneWay = false;
    bool drawMe = false;
    ezgl::color segmentColour;
    double drawScaleFactor;
    StreetSegType streetSegClass;
    bool pathHighlight;
};

// Define a struct to hold feature information
struct FeatureStruct {
    // Define members of FeatureStruct
    int numOfPoints = 0;
    bool featureHighlight = false;
    bool whichFeatureEntry = false;
    //bool beingDrawn = false;

    std::vector<LatLon> featurePoints;
    std::vector<ezgl::point2d> featureXYPoints;
    std::string featureName, featureTypeName;
    ezgl::color featureColour;
    double featureArea = 0;
    double featureLength = 0;
    ezgl::point2d centroid;
    ezgl::point2d min, max; // the minimum (x,y) and maximum (x,y) bounds for MBR
};

struct intersectionNode{
    IntersectionIdx id;
    // create a pair of matching vectors that hold the connected street
//    std::pair<std::vector<IntersectionIdx>, std::vector<StreetSegmentIdx>> connectedIntersectionsAndStreetSegments;
    // create a 2d vector that will store the following: start intersection, street segment, end intersection
    std::vector<std::vector<int>> adjacentPaths;
    double travelTime;
};

struct waveElem{
    IntersectionIdx nodeID; // current node we are checking the wave
    StreetSegmentIdx edgeID; // edge used to reach the node
    double travelTime; // total travel time to reach the node using the edge

    // default constructor should be good enough
    waveElem(IntersectionIdx _nodeID, StreetSegmentIdx _edgeID, double _travelTime){
        this->nodeID = _nodeID;
        this->edgeID = _edgeID;
        this->travelTime = _travelTime;
    }
};

struct compareTravelTime{
    bool operator()(waveElem const& lhs, waveElem const& rhs){
        return lhs.travelTime > rhs.travelTime;
    }
};

struct deliveryNode{
    IntersectionIdx pickup = -1; // if the delivery is a pickup
    IntersectionIdx dropoff = -1; // if the delivery is a dropoff
    bool isPickup = false;
    double distance; // the distance to this node from a related node that we are comparing to.

    deliveryNode(IntersectionIdx _pickup, IntersectionIdx _dropoff, bool _isPickup, double _distance){
        this->pickup = _pickup;
        this->dropoff = _dropoff;
        this->isPickup = _isPickup;
        this->distance = _distance;
    }
};

struct encodedDeliveryNode{
    IntersectionIdx intersectionID = -1; // if the delivery is a dropoff
    IntersectionIdx pickUpID = -1;
    bool isPickup = false;

    encodedDeliveryNode(IntersectionIdx index, IntersectionIdx pickUp, bool _isPickup){
        this->intersectionID = index;
        this->pickUpID = pickUp;
        this->isPickup = _isPickup;
    }

    encodedDeliveryNode(){
        this->intersectionID = -1;
        this->pickUpID = -1;
        this->isPickup = false;
    }

    void operator=(const encodedDeliveryNode &rhs){
        this->pickUpID = rhs.pickUpID;
        this->intersectionID = rhs.intersectionID;
        this->isPickup = rhs.isPickup;
    }
};

// compare member function to sort the delivery nodes in the priority queue.
struct compareDistance{
    bool operator()(deliveryNode const& lhs, deliveryNode const& rhs){
        return lhs.distance > rhs.distance;
    }
};

struct Node{
//    std::vector<StreetSegmentIdx> outGoingEdges; // adjacent edges
    StreetSegmentIdx reachingEdge;
    double bestTime; // when bestTime != infinity we visited
};

struct pair_hash
{
    IntersectionIdx I1, I2;
    std::size_t operator() (const std::pair<IntersectionIdx , IntersectionIdx> &pair) const {
        return std::hash<IntersectionIdx>()(pair.first) ^ std::hash<IntersectionIdx>()(pair.second);
    }
};

/* Define data structures that use custom structs*/
extern std::vector<IntersectStruct> gIntersectVector;
extern std::vector<POIStruct> gPOIVector;
extern std::vector<StreetStruct> gStreetsVector;
extern std::vector<SegStruct> gSegmentVector;
extern std::vector<FeatureStruct> gFeatureVector;
//extern std::vector<Node> gIntersectionNodeVector;
//extern std::vector<Node> gNodesVector;
// Preload the adjacentSegments in the format From intersection, street segment, To intersection
// note that from intersection is the index;
// this basically means [index] = wavelem (node + edge)
extern std::vector<std::vector<std::pair<StreetSegmentIdx, IntersectionIdx>>> gAdjacentSegments;

/* Define data structures that use predefined types */
extern std::vector<std::pair<std::string, StreetIdx>> gStreetsNameVector;
extern std::unordered_map<OSMID, std::vector<std::pair<std::string, std::string>>> gOSMNodeHash;
extern std::vector<std::pair<double, FeatureIdx>> gClosedPoly;
extern std::vector<FeatureIdx> gLinePoly;
extern std::vector<FeatureIdx> gSinglePoint;
extern std::vector<std::pair<std::string, FeatureIdx>> gFeaturesNameVector;
extern std::vector<std::pair<std::string, POIIdx>> gPOINameVector;
extern std::unordered_map<OSMID, std::string> gStreetClassificationMap;
extern std::vector<const OSMNode*> osmSubwayStations;

extern std::vector<StreetSegmentIdx> gPrimarySegs;
extern std::vector<StreetSegmentIdx> gMotorwaySegs;
extern std::vector<StreetSegmentIdx> gLinkSegs;
extern std::vector<StreetSegmentIdx> gSecondarySegs;
extern std::vector<StreetSegmentIdx> gSubMinorSegs;
extern std::vector<StreetSegmentIdx> gMinorSegs;
extern std::vector<std::string> pathInstruction;

// Globals needed in callback.cpp and control.cpp to limit zoom/scrolling/panning
extern int zoom_level;
extern ezgl::rectangle initial_world;

// Global pointers used in M2 for png drawing
//load in all pointers to image files
extern ezgl::surface *surface_education;
extern ezgl::surface *surface_entertainment;
extern ezgl::surface *surface_financial;
extern ezgl::surface *surface_food;
extern ezgl::surface *surface_general;
extern ezgl::surface *surface_healthcare;
extern ezgl::surface *surface_publicService;
extern ezgl::surface *surface_transportation;
extern ezgl::surface *surface_waste;
extern ezgl::surface *find_poi_1;
extern ezgl::surface *find_poi_2;
extern ezgl::surface *find_feature_1;
extern ezgl::surface *find_feature_2;
extern ezgl::surface *intersection_highlight;
extern ezgl::surface  *surface_subway;

/* Define variables that are used in multiple files */
extern double averageLatitude;
extern double worldWidth;
extern double worldHeight;
extern double screenWidth;
extern double screenHeight;
extern double worldArea;
extern double scaleFactor;
extern double area;
extern double scale;
extern double initialScale;
extern bool toggle_bg;
extern int fastestStreetSegSpeed;
extern bool clearBool;

/* Define constant variables */
extern const double kClosedPolygonScaleFactor;
extern const double kPointScaleFactor;
extern const double kClosedPolygonTextScaleFactor;
extern const double kPointTextScaleFactor;
extern const double kPointFeatureArea;
extern const double kPoiArea;

/* Declare multiDijkstra function */
std::vector<double> multiDijkstra(const IntersectionIdx &source, const std::vector<IntersectionIdx> &intersections,
                                                          const double &turn_penalty);


#endif /* GLOBALS_H */