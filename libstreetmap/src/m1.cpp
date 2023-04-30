/* 
 * Copyright 2023 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "globals.h"

/* Define data structures used in m1.cpp */
std::vector<POIStruct> gPOIVector;
std::vector<IntersectStruct> gIntersectVector;
std::vector<StreetStruct> gStreetsVector; //vector of all streets of type StreetStruct
std::vector<std::pair<std::string, StreetIdx>> gStreetsNameVector;
std::vector<std::pair<std::string, FeatureIdx>> gFeaturesNameVector;
std::vector<std::pair<std::string, POIIdx>> gPOINameVector;
std::vector<SegStruct> gSegmentVector;
std::unordered_map<OSMID, std::vector<std::pair<std::string, std::string>>> gOSMNodeHash;
std::vector<FeatureStruct> gFeatureVector;
std::vector<std::pair<double, FeatureIdx>> gClosedPoly;
std::vector<FeatureIdx> gLinePoly;
std::vector<FeatureIdx> gSinglePoint;
std::unordered_map<OSMID, std::string> gStreetClassificationMap;
std::vector<const OSMNode*> osmSubwayStations;
//std::vector<Node> gNodesVector;
std::vector<std::vector<std::pair<StreetSegmentIdx, IntersectionIdx>>> gAdjacentSegments;

std::vector<StreetSegmentIdx> gPrimarySegs;
std::vector<StreetSegmentIdx> gMotorwaySegs;
std::vector<StreetSegmentIdx> gLinkSegs;
std::vector<StreetSegmentIdx> gSecondarySegs;
std::vector<StreetSegmentIdx> gMinorSegs;
std::vector<StreetSegmentIdx> gSubMinorSegs;

// define universal average latitude
double averageLatitude;
int fastestStreetSegSpeed;

/* Forward declaration of loadMap() helper functions */
void initializePOI();
void initializeStreets();
void initializeIntersections();
void initializeStreetSegs();
void initializeOSM();
void initializeWays();
void initializeFeatures();
//void initializeRelations();

ezgl::renderer *m1_rend;
ezgl::surface *surface_education;
ezgl::surface *surface_entertainment;
ezgl::surface *surface_financial;
ezgl::surface *surface_food;
ezgl::surface *surface_general;
ezgl::surface *surface_healthcare;
ezgl::surface *surface_publicService;
ezgl::surface *surface_transportation;
ezgl::surface *surface_waste;
ezgl::surface *find_poi_1;
ezgl::surface *find_poi_2;
ezgl::surface *find_feature_1;
ezgl::surface *find_feature_2;
ezgl::surface *intersection_highlight;
ezgl::surface *surface_subway;

/********** DEFINE THE LOADMAP() FUNCTION **********/
bool loadMap(std::string map_streets_database_filename) {
    auto start = std::chrono::system_clock::now();
    
    bool load_successful = false; //Indicates whether the map has loaded
    //successfully load the streets database binary with the given filename
    load_successful = loadStreetsDatabaseBIN(map_streets_database_filename);

    // if the streets database binary did not load properly, then exit the program
    if (!load_successful) {
        return load_successful;
    }

    // remove the .streets.bin
    int removeIndex = map_streets_database_filename.find(".streets");
    map_streets_database_filename.erase(map_streets_database_filename.begin() + removeIndex, map_streets_database_filename.end());

    // append .osm.bin
    map_streets_database_filename += ".osm.bin";

    // load the OSM database binary
    load_successful = loadOSMDatabaseBIN(map_streets_database_filename);

    // if the OSM database binary did not load properly, then exit the program
    if (!load_successful) {
        return load_successful;
    }

    // print out that the map has been loaded properly
    std::cout << "loadMap: " << map_streets_database_filename << std::endl;

    // initialize intersection data structures first because we need averageLatitutde for poiXY projection
    std::thread Intersections(initializeIntersections);

    // initialize osm data structures
    std::thread OSM(initializeOSM);

    // initialize street classifications using osm ways
    std::thread StreetClassification(initializeWays);

    Intersections.join();
    OSM.join();
    StreetClassification.join();

    // intialize street segment data structures
    std::thread StreetSegs(initializeStreetSegs);

    StreetSegs.join();

    // initialize street data structures
    std::thread Streets(initializeStreets);

    // intitialize poi data structures
    std::thread POI(initializePOI);

    // intialize feature data strctures
    std::thread Features(initializeFeatures);

    Streets.join();
    POI.join();
    Features.join();
    
    //load in all pointers to image files
    surface_education = m1_rend->load_png("libstreetmap/resources/icons/Education.png");
    surface_entertainment = m1_rend->load_png("libstreetmap/resources/icons/Entertainment.png");
    surface_financial = m1_rend->load_png("libstreetmap/resources/icons/Financial.png");
    surface_food = m1_rend->load_png("libstreetmap/resources/icons/Food.png");
    surface_general = m1_rend->load_png("libstreetmap/resources/icons/General.png");
    surface_healthcare = m1_rend->load_png("libstreetmap/resources/icons/Healthcare.png");
    surface_publicService = m1_rend->load_png("libstreetmap/resources/icons/PublicService.png");
    surface_transportation = m1_rend->load_png("libstreetmap/resources/icons/Transportation.png");
    surface_waste = m1_rend->load_png("libstreetmap/resources/icons/Waste.png");
    find_poi_1 = m1_rend->load_png("libstreetmap/resources/icons/find_location_1.png");
    find_poi_2 = m1_rend->load_png("libstreetmap/resources/icons/find_location_2.png");
    find_feature_1 = m1_rend->load_png("libstreetmap/resources/icons/find_location_3.png");
    find_feature_2 = m1_rend->load_png("libstreetmap/resources/icons/find_location_4.png");
    intersection_highlight = m1_rend->load_png("libstreetmap/resources/icons/intersection_icon.png");
    surface_subway = m1_rend->load_png("libstreetmap/resources/icons/Subway.png");

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Preload Time: " << diff.count() << "\n";
    
    return load_successful;
}

/* Initialize POI data structures with helper function */
void initializePOI(){
    // Predefine the size for data structures
    gPOIVector.resize(getNumPointsOfInterest());
    gPOINameVector.resize(getNumPointsOfInterest());

    // for loop to initialize poi information struct vector
    for (POIIdx poiID = 0; poiID < getNumPointsOfInterest(); poiID++) {
        gPOIVector[poiID].type = getPOIType(poiID);
        std::string poiName = getPOIName(poiID);
        gPOIVector[poiID].name = getPOIName(poiID);
        gPOIVector[poiID].poiLoc = getPOIPosition(poiID);
        gPOIVector[poiID].index = poiID;
        gPOIVector[poiID].poiXYLoc = {projectX(gPOIVector[poiID].poiLoc.longitude()), projectY(gPOIVector[poiID].poiLoc.latitude())};

        // remove all spaces from the POI name
        poiName.erase(std::remove_if(poiName.begin(), poiName.end(), [](unsigned char x) {
            return std::isspace(x); }), poiName.end());

        // turn to all lowercase
        for (int i = 0; i < poiName.length(); i++) {
            poiName[i] = tolower(poiName[i]);
        }

        gPOINameVector[poiID] = std::make_pair(poiName, poiID);
    }
    std::sort(gPOINameVector.begin(), gPOINameVector.end());
}

/* Initialize street data structures with helper function */
void initializeStreets(){
//    Timer timer;

    // Predefine the size for data structures
    gStreetsNameVector.resize(getNumStreets());

    //initialize the global gStreetsVector vector with all streets; should initialize the global gSegmentVector as well
    for (StreetIdx street = 0; street < getNumStreets(); street++) {

        // Get the street name
        std::string _streetName = getStreetName(street);

        // remove spaces from the street name
        _streetName.erase(std::remove_if(_streetName.begin(), _streetName.end(), [](unsigned char x) {
            return std::isspace(x); }), _streetName.end());

        for (int i = 0; i < _streetName.length(); i++) { // get lower case no space version of streetName
            _streetName[i] = tolower(_streetName[i]);
        }

        gStreetsVector[street].streetName = getStreetName(street);
        gStreetsVector[street].streetIndex = street;

        // remove duplicates from the intersectsOfStreet vector
        std::sort(gStreetsVector[street].intersectsOfStreet.begin(), gStreetsVector[street].intersectsOfStreet.end());
        gStreetsVector[street].intersectsOfStreet.erase(std::unique(gStreetsVector[street].intersectsOfStreet.begin(), gStreetsVector[street].intersectsOfStreet.end()), gStreetsVector[street].intersectsOfStreet.end());

        // store the street name without spaces and lowercase in gStreetsNameVector for the partial names function.
        gStreetsNameVector[street] = std::pair<std::string, StreetIdx>(_streetName, street);
    }

    // sort the gStreetsNameVector to use with lower_bound
    std::sort(gStreetsNameVector.begin(), gStreetsNameVector.end());
}

/* Initialize intersection data structures with helper function */
void initializeIntersections(){
    // Predefine the size for data structures
    gIntersectVector.resize(getNumIntersections());
//    gIntersectionNodeVector.resize(getNumIntersections());
//    gNodesVector.resize(getNumIntersections());

    gAdjacentSegments.resize(getNumIntersections());

    // Setup values for each element (intersection) in gIntersectVector
    for (IntersectionIdx interID = 0; interID < getNumIntersections(); interID++) {
        gIntersectVector[interID].intersectLoc = getIntersectionPosition(interID);

        gIntersectVector[interID].interIndex = interID;
        gIntersectVector[interID].intersectHighlight = false;

        // Predefine the size for the connected streets segment vector
        gIntersectVector[interID].connectedStreetSegments.resize(getNumIntersectionStreetSegment(interID));

        // Update averageLatitude
        averageLatitude += gIntersectVector[interID].intersectLoc.latitude();

//        gNodesVector[interID].bestTime = infinity;
//        gNodesVector[interID].reachingEdge = -1;

        // Setup adjacentIntersections and connectedStreetSegments vector for each intersection
        for (int ssCounter = 0; ssCounter < getNumIntersectionStreetSegment(interID); ssCounter++) {
            // get the current street segment ID
            StreetSegmentIdx ssID = getIntersectionStreetSegment(interID, ssCounter);
            // put the connected street segment ID into the connectedStreetSegments vector
            gIntersectVector[interID].connectedStreetSegments[ssCounter] = ssID;
            // no need for duplicate checking because we know there are distinct street segments that connect to an intersection

            // get the current street segment struct;
            StreetSegmentInfo currentStreetSegment = getStreetSegmentInfo(ssID);

            // check if currentStreetSegment from matches interIndex
            if (currentStreetSegment.from == interID) {
                // push to the vector forgoing duplicate check
                gAdjacentSegments[interID].push_back(std::make_pair(ssID, currentStreetSegment.to));
                gIntersectVector[interID].adjacentIntersections.push_back(currentStreetSegment.to);
            } else { // else then to is the intersection for intersection_id
                // check if the street segment is one way, because we can't put anything in the vector if it is
                if (!(currentStreetSegment.oneWay)) {
                    // push to the vector forgoing duplicate check
                    gAdjacentSegments[interID].push_back(std::make_pair(ssID, currentStreetSegment.from));
                    gIntersectVector[interID].adjacentIntersections.push_back(currentStreetSegment.from);
                }
            }
        }

        // sort the adjacentIntersections vector
        std::sort(gIntersectVector[interID].adjacentIntersections.begin(),gIntersectVector[interID].adjacentIntersections.end());
        // remove duplicates from the adjacentIntersections vector
        gIntersectVector[interID].adjacentIntersections.erase(std::unique(gIntersectVector[interID].adjacentIntersections.begin(), gIntersectVector[interID].adjacentIntersections.end()), gIntersectVector[interID].adjacentIntersections.end());
    }

    // final update to averageLatitude
    averageLatitude = ((averageLatitude)/(gIntersectVector.size())) * kDegreeToRadian;
    averageLatitude = std::cos(averageLatitude);

    // update the projected x and y coordinates for intersections now that averageLatitude is defined
    for (int i = 0; i < getNumIntersections(); i++){
        gIntersectVector[i].intersectXYLoc = { projectX(gIntersectVector[i].intersectLoc.longitude()),
                                                     projectY(gIntersectVector[i].intersectLoc.latitude()) };
    }
}

/* Initialize street segment data structure with helper function */
void initializeStreetSegs(){
    fastestStreetSegSpeed = 0;
    // Predefine the size for data structures
    gSegmentVector.resize(getNumStreetSegments());
    gStreetsVector.resize(getNumStreets());
    
    gPrimarySegs.reserve(getNumStreetSegments());
    gMotorwaySegs.reserve(getNumStreetSegments());
    gLinkSegs.reserve(getNumStreetSegments());
    gSecondarySegs.reserve(getNumStreetSegments());
    gMinorSegs.reserve(getNumStreetSegments());
    gSubMinorSegs.reserve(getNumStreetSegments());
    
    for (StreetSegmentIdx seg = 0; seg < getNumStreetSegments(); seg++) {
        gSegmentVector[seg].segId = seg;

        StreetSegmentInfo StrSegInfo = getStreetSegmentInfo(seg); // get information associated with the id of a street segment

        /* define variables that can be immediately fetched from StrSegInfo */
        gSegmentVector[seg].wayID = StrSegInfo.wayOSMID;
        gSegmentVector[seg].segSpeedLimit = StrSegInfo.speedLimit;
        gSegmentVector[seg].myStreet = StrSegInfo.streetID;
        gSegmentVector[seg].from = StrSegInfo.from;
        gSegmentVector[seg].to = StrSegInfo.to;
        gSegmentVector[seg].numOfCurvePoints = StrSegInfo.numCurvePoints;
        gSegmentVector[seg].oneWay = StrSegInfo.oneWay;
        if (StrSegInfo.speedLimit > fastestStreetSegSpeed){
            fastestStreetSegSpeed = std::ceil(StrSegInfo.speedLimit);
//            std::cout << fastestStreetSegSpeed << std::endl;
        }
        
        if(gStreetClassificationMap[gSegmentVector[seg].wayID] == "primary"){
            gSegmentVector[seg].streetSegClass = PRIMARY;
            gSegmentVector[seg].drawScaleFactor = 4;
            gSegmentVector[seg].drawMe = true;
            gSegmentVector[seg].segmentColour = ezgl::color(255, 128, 128); //reddish orange
            
            gPrimarySegs.push_back(seg);
        }
        else if(gStreetClassificationMap[gSegmentVector[seg].wayID] == "motorway" || gStreetClassificationMap[gSegmentVector[seg].wayID] == "trunk") {
            gSegmentVector[seg].streetSegClass = MOTORWAY;
            gSegmentVector[seg].drawScaleFactor = 4;
            gSegmentVector[seg].drawMe = true;
            gSegmentVector[seg].segmentColour = ezgl::color(239, 185, 75); //orange
            gMotorwaySegs.push_back(seg);
        }
        else if(gStreetClassificationMap[gSegmentVector[seg].wayID] == "motorway_link" || gStreetClassificationMap[gSegmentVector[seg].wayID] == "trunk_link"){
            gSegmentVector[seg].streetSegClass = MOTORWAY_LINK;
            gSegmentVector[seg].drawScaleFactor = 3;
            gSegmentVector[seg].segmentColour = ezgl::color(239, 185, 75); //also orange
            gLinkSegs.push_back(seg);
        }

        else if(gStreetClassificationMap[gSegmentVector[seg].wayID] == "secondary"){
            gSegmentVector[seg].streetSegClass = SECONDARY;
            gSegmentVector[seg].drawScaleFactor = 3; 
            gSegmentVector[seg].segmentColour = ezgl::WHITE;
            gSecondarySegs.push_back(seg);
        }
        else if(getStreetName(gSegmentVector[seg].myStreet) == "<unknown>" && gStreetClassificationMap[gSegmentVector[seg].wayID] == "service"){  // all the tiny roads
            gSegmentVector[seg].streetSegClass = SUBMINOR;
            gSegmentVector[seg].drawScaleFactor = 0.75;
            gSegmentVector[seg].segmentColour = ezgl::WHITE;
            gSubMinorSegs.push_back(seg);
        }
        else {
            gSegmentVector[seg].streetSegClass = MINOR;
            gSegmentVector[seg].drawScaleFactor = 1.5;
            gSegmentVector[seg].segmentColour = ezgl::WHITE;
            
            gMinorSegs.push_back(seg);
        }

        LatLon latlonFrom = getIntersectionPosition(StrSegInfo.from); //returns LatLon of 'from' starting intersection
        LatLon latlonTo = getIntersectionPosition(StrSegInfo.to); //returns LatLon of 'to' ending intersection
        LatLon latlonCurve; // will be used for LatLon of curve points between from and to

        gSegmentVector[seg].segLength = 0; //initialize segLength to 0

        gSegmentVector[seg].curvePoints.resize(gSegmentVector[seg].numOfCurvePoints);
        for (int i = 0; i < gSegmentVector[seg].numOfCurvePoints; i++) {
            latlonCurve = getStreetSegmentCurvePoint(seg, i);
            gSegmentVector[seg].curvePoints[i] = {projectX(latlonCurve.longitude()), projectY(latlonCurve.latitude())};
            gSegmentVector[seg].segLength += findDistanceBetweenTwoPoints(latlonFrom, latlonCurve); //calculate distance between from and in-between curve points

            latlonFrom = latlonCurve; //update From coordinates to iterate along the segment towards latlonTo
        }
        gSegmentVector[seg].segLength += findDistanceBetweenTwoPoints(latlonFrom, latlonTo); //calculate the last curve point to the ending intersection

        // update the travel time for the segment
        gSegmentVector[seg].segTravelTime = gSegmentVector[seg].segLength / gSegmentVector[seg].segSpeedLimit;

        /* append to StreetStruct's segsOfStreet */
        StreetIdx containgStreet = gSegmentVector[seg].myStreet;
        // this street segment is associated with a street id, which we can reference and append into that street's segsOfStreet vector
        gStreetsVector[containgStreet].segsOfStreet.push_back(seg);

        // update the length of the street in StreetStrust
        gStreetsVector[containgStreet].streetLength += gSegmentVector[seg].segLength;

        // add the intersections to the intersectsOfStreet vector forgoing duplicate checks.
        gStreetsVector[containgStreet].intersectsOfStreet.push_back(gSegmentVector[seg].from);
        gStreetsVector[containgStreet].intersectsOfStreet.push_back(gSegmentVector[seg].to);
    }
}

/* Initialize OSM data structure with helper function */
void initializeOSM(){
    // Predefine the size for data structures
    gOSMNodeHash.rehash(getNumberOfNodes());

    // loop through all nodes of the city to initialize gOSMNodeHash
    for (int nodeIdx = 0; nodeIdx < getNumberOfNodes(); nodeIdx++) {

        // Get the OSM Node
        const OSMNode* node = getNodeByIndex(nodeIdx); // collect the node at the index

        // Get the OSMID
        OSMID _osmID = node->id(); // get the node's actual OSMID

        // Get the # of tags for that OSM Node
        int _tagCount = getTagCount(node); // obtain the node's number of tags

        std::vector<std::pair<std::string, std::string>> osmTagVector;

        // Predefine the size for osmTagVector
        osmTagVector.resize(_tagCount);

        for (int i = 0; i < _tagCount; i++){
            osmTagVector[i] = getTagPair(node, i);
        }

        // populate the subway vector.
        for (int i = 0; i < _tagCount; i++){
            if (getTagPair(node, i).first == "station" && getTagPair(node, i).second == "subway") {
                osmSubwayStations.emplace_back(node);
                break;
            }
        }

        std::sort(osmTagVector.begin(), osmTagVector.end());
        gOSMNodeHash[_osmID] = osmTagVector;
    }
}

/* Initialize feature data structures with helper function */
void initializeFeatures(){
//    Timer timer;

    // Predefine the size for data structures
    gFeatureVector.resize(getNumFeatures());
    gFeaturesNameVector.resize(getNumFeatures());

    // Predefine the memory for gClosedPoly, gLinePoly, and gSinglePoint
    gClosedPoly.reserve(getNumFeatures());
    gLinePoly.reserve(getNumFeatures());
    gSinglePoint.reserve(getNumFeatures());

    // loop through all feature ids
    for (FeatureIdx feature = 0; feature < getNumFeatures(); feature++){
        std::string featureName = getFeatureName(feature);
        gFeatureVector[feature].featureName = getFeatureName(feature);
        FeatureType featureType = getFeatureType(feature);

        gFeatureVector[feature].numOfPoints = getNumFeaturePoints(feature);

        // setup the featurePoints and featureXYPoints vectors
        gFeatureVector[feature].featurePoints.resize(getNumFeaturePoints(feature));
        gFeatureVector[feature].featureXYPoints.resize(getNumFeaturePoints(feature));

        double _length = 0;
        std::vector<double> xCoords, yCoords;
        xCoords.resize(gFeatureVector[feature].numOfPoints);
        yCoords.resize(gFeatureVector[feature].numOfPoints);

        for (int i = 0; i < gFeatureVector[feature].numOfPoints; i++) {
            gFeatureVector[feature].featurePoints[i] = getFeaturePoint(feature, i);
            gFeatureVector[feature].featureXYPoints[i] = {projectX(gFeatureVector[feature].featurePoints[i].longitude()),
                                                          projectY(gFeatureVector[feature].featurePoints[i].latitude())};
            xCoords[i] = gFeatureVector[feature].featureXYPoints[i].x;
            yCoords[i] = gFeatureVector[feature].featureXYPoints[i].y;
            if (i > 1){
            _length += sqrt(pow((gFeatureVector[feature].featureXYPoints[i].x - gFeatureVector[feature].featureXYPoints[i-1].x), 2)
                           + pow((gFeatureVector[feature].featureXYPoints[i].y - gFeatureVector[feature].featureXYPoints[i-1].y), 2));
            }
        }
        gFeatureVector[feature].centroid = getCentroid(gFeatureVector[feature].featureXYPoints);
        gFeatureVector[feature].min.x = *(std::min_element(xCoords.begin(), xCoords.end()));
        gFeatureVector[feature].min.y = *(std::min_element(yCoords.begin(), yCoords.end()));
        gFeatureVector[feature].max.x = *(std::max_element(xCoords.begin(), xCoords.end()));
        gFeatureVector[feature].max.y = *(std::max_element(yCoords.begin(), yCoords.end()));

        featureName.erase(std::remove_if(featureName.begin(), featureName.end(), [](unsigned char x) {
            return std::isspace(x); }), featureName.end());
        for (int i = 0; i < featureName.length(); i++) {
            featureName[i] = tolower(featureName[i]);
        }

        gFeaturesNameVector[feature] = std::make_pair(featureName, feature);

        gFeatureVector[feature].featureArea = 0; //0 if not something we care about
        if (gFeatureVector[feature].featurePoints.size() == 1) gSinglePoint.push_back(feature);
        else if (gFeatureVector[feature].featurePoints[0] == gFeatureVector[feature].featurePoints[gFeatureVector[feature].numOfPoints - 1]){
            gFeatureVector[feature].featureArea = findFeatureArea(feature); //assign to closed polys an area
            gClosedPoly.push_back(std::make_pair(gFeatureVector[feature].featureArea, feature));
        }
        else{
            gLinePoly.push_back(feature);
            _length += sqrt(pow((gFeatureVector[feature].featureXYPoints[1].x - gFeatureVector[feature].featureXYPoints[0].x), 2)
                           + pow((gFeatureVector[feature].featureXYPoints[1].y - gFeatureVector[feature].featureXYPoints[0].y), 2));
            gFeatureVector[feature].featureLength = _length;
        }

        switch (featureType) {
            case 1:
                gFeatureVector[feature].featureColour = {187, 224, 197}; //park - pastel green
                break;
            case 2:
                gFeatureVector[feature].featureColour = {255, 255, 224}; //beach - pastel light yellow
                break;
            case 3:
                gFeatureVector[feature].featureColour = {167, 199, 231}; //lake - pastel blue
                break;
            case 4:
                gFeatureVector[feature].featureColour = {167, 199, 231}; //river - pastel blue
                break;
            case 5:
                gFeatureVector[feature].featureColour = {222,223,224}; //island - light grey (same as map background)
                break;
            case 6:
                gFeatureVector[feature].featureColour = {190, 190, 190}; //building - darker grey
                break;
            case 7:
                gFeatureVector[feature].featureColour = {160, 213, 174}; //greenspace - pastel dark green
                break;
            case 8:
                gFeatureVector[feature].featureColour = {180, 220, 190}; //golfcourse
                break;
            case 9:
                gFeatureVector[feature].featureColour = {167, 199, 231}; //stream - pastel blue
                break;
            case 10:
                gFeatureVector[feature].featureColour = {240, 248, 255}; //glacier - icy blue
                break;
            default:
                gFeatureVector[feature].featureColour = ezgl::GREY_75; //unknown
                break;
        }
        gFeatureVector[feature].featureTypeName = asString(featureType);
    }
    //sort closedPoly features by their area
    std::sort(gClosedPoly.begin(), gClosedPoly.end(), std::greater<>());
    // sort gFeaturesNameVector to use with lower_bound
    std::sort(gFeaturesNameVector.begin(), gFeaturesNameVector.end());
}

/* Initialize classification map for streets with helper function */
void initializeWays(){
    gStreetClassificationMap.reserve(getNumberOfWays());

    for (int i = 0; i < getNumberOfWays(); i++){
        const OSMWay* osmStreetSeg = getWayByIndex(i);  // each OSMWay
        for(int k = 0; k < getTagCount(osmStreetSeg); k++){
            std::pair<std::string, std::string> tagPair = getTagPair(osmStreetSeg, k);  // get actual tag pairs.
            //if it is, then we put it in the street classification map.
            if((tagPair.first == "highway" && tagPair.second == "motorway")
               || (tagPair.first == "highway" && tagPair.second == "motorway_link")
               || (tagPair.first == "highway" && tagPair.second == "trunk")
               || (tagPair.first == "highway" && tagPair.second == "trunk_link")
               || (tagPair.first == "highway" && tagPair.second == "primary")
               || (tagPair.first == "highway" && tagPair.second == "primary_link")
               || (tagPair.first == "highway" && tagPair.second == "secondary")
               || (tagPair.first == "highway" && tagPair.second == "secondary_link")
               || (tagPair.first == "highway" && tagPair.second == "unclassified")
               || (tagPair.first == "highway" && tagPair.second == "residential")
               || (tagPair.first == "highway" && tagPair.second == "pedestrian")
               || (tagPair.first == "highway" && tagPair.second == "busway")
               || (tagPair.first == "highway" && tagPair.second == "service")
               || (tagPair.first == "highway" && tagPair.second == "tertiary")
                    ){

                OSMID _osmid = osmStreetSeg->id();
                
                gStreetClassificationMap.emplace(_osmid, tagPair.second);
            }
        }
    }
}

/********** DEFINE THE CLOSEMAP() FUNCTION **********/
/* every global vector/map (including ones nested within structs) are freed in this function
 * through a method of swapping with an empty vector/map
 * This automatically calls the STL vector/map destructor to deallocate the memory associated 
 * with the vector/map (both size and capacity)
 * this method was chosen, as using the alternative vec.clear() failed to produce desired results 
 * during testing with memory de-allocation
 */
void closeMap() {
//    Timer timer;

    // de-allocate gIntersectVector
    for (int i = 0; i < gIntersectVector.size(); i++) {
        std::vector<IntersectionIdx>().swap(gIntersectVector[i].adjacentIntersections);
        std::vector<StreetSegmentIdx>().swap(gIntersectVector[i].connectedStreetSegments);
    }
    std::vector<IntersectStruct>().swap(gIntersectVector);

    // de-allocate gSegmentVector
    for (int i = 0; i < gSegmentVector.size(); i++){
        std::vector<ezgl::point2d>().swap(gSegmentVector[i].curvePoints);
    }
    std::vector<SegStruct>().swap(gSegmentVector);

    // de-allocate gStreetsVector
    for (int i = 0; i < gStreetsVector.size(); i++) {
        std::vector<StreetSegmentIdx>().swap(gStreetsVector[i].segsOfStreet);
        std::vector<IntersectionIdx>().swap(gStreetsVector[i].intersectsOfStreet);
    }
    std::vector<StreetStruct>().swap(gStreetsVector);

    // de-allocate gPOIVector
    std::vector<POIStruct>().swap(gPOIVector);

    // de-allocate gStreetsNameVector
    std::vector<std::pair<std::string, StreetIdx>>().swap(gStreetsNameVector);

    // de-allocate gOSMNodeHash
    std::unordered_map<OSMID, std::vector<std::pair<std::string, std::string>>>().swap(gOSMNodeHash);

    // de-allocate gFeatureVector
    for(int i = 0; i < gFeatureVector.size(); i++){
        std::vector<LatLon>().swap(gFeatureVector[i].featurePoints);
        std::vector<ezgl::point2d>().swap(gFeatureVector[i].featureXYPoints);
    }
    std::vector<FeatureStruct>().swap(gFeatureVector);

    // de-allocate the individual type of feature vectors
    std::vector<std::pair<double, FeatureIdx>>().swap(gClosedPoly);
    std::vector<FeatureIdx>().swap(gLinePoly);
    std::vector<FeatureIdx>().swap(gSinglePoint);
    
    // de-allocate the individual type of street segment vectors
    std::vector<StreetSegmentIdx>().swap(gPrimarySegs);
    std::vector<StreetSegmentIdx>().swap(gMotorwaySegs);
    std::vector<StreetSegmentIdx>().swap(gLinkSegs);
    std::vector<StreetSegmentIdx>().swap(gSecondarySegs);
    std::vector<StreetSegmentIdx>().swap(gSubMinorSegs);
    std::vector<StreetSegmentIdx>().swap(gMinorSegs);

    // de-allocate osmSubwayStations
    std::vector<const OSMNode*>().swap(osmSubwayStations);

    // de-allocate gStreetClassificationMap
    std::unordered_map<OSMID, std::string>().swap(gStreetClassificationMap);

    // de-allocate name vectors
    std::vector<std::pair<std::string, FeatureIdx>>().swap(gFeaturesNameVector);
    std::vector<std::pair<std::string, POIIdx>>().swap(gPOINameVector);

    // de-allocate gAdjacentSegments
    std::vector<std::vector<std::pair<StreetSegmentIdx, IntersectionIdx>>>().swap(gAdjacentSegments);

    // de-allocate path instructions
    std::vector<std::string>().swap(pathInstruction);


    //free all surfaces for PNG drawings
    m1_rend->free_surface(surface_education);
    m1_rend->free_surface(surface_entertainment);
    m1_rend->free_surface(surface_financial);
    m1_rend->free_surface(surface_food);
    m1_rend->free_surface(surface_general);
    m1_rend->free_surface(surface_healthcare);
    m1_rend->free_surface(surface_publicService);
    m1_rend->free_surface(surface_transportation);
    m1_rend->free_surface(surface_waste);
    m1_rend->free_surface(find_poi_1);
    m1_rend->free_surface(find_poi_2);
    m1_rend->free_surface(find_feature_1);
    m1_rend->free_surface(find_feature_2);
    m1_rend->free_surface(intersection_highlight);
    m1_rend->free_surface(surface_subway);

    // close the streets database binary
    closeStreetDatabase();

    // close the OSM database binary
    closeOSMDatabase();
}

/* AUTHOR: Nicholas Mutlak
 * returns the distance between two latitude and longitude set of coordinates
 * Speed Requirement: Moderate
 */
double findDistanceBetweenTwoPoints(LatLon point_1, LatLon point_2) {
    double distance = 0;
    
    // get latitude and longitude coordinates and convert them into radians
    double lat_1 = point_1.latitude() * kDegreeToRadian,
            lon_1 = point_1.longitude() * kDegreeToRadian,
            lat_2 = point_2.latitude() * kDegreeToRadian,
            lon_2 = point_2.longitude() * kDegreeToRadian; 

    double lat_avg = (lat_1 + lat_2) / 2.0; // get the average of the latitudes for the projection formula

    // project the two coordinates into the XY plane, saved into x1,y1 and x2,y2
    double x1 = kEarthRadiusInMeters * lon_1 * cos(lat_avg),
            y1 = kEarthRadiusInMeters * lat_1,

            x2 = kEarthRadiusInMeters * lon_2 * cos(lat_avg),
            y2 = kEarthRadiusInMeters * lat_2;

    return distance = sqrt(pow((y2 - y1), 2.0) + pow((x2 - x1), 2.0)); // calculate distance formula with XY coordinates
}

/* AUTHOR: Nicholas Mutlak
 * returns the distance of a street segment stored in gSegmentVector
 * Speed Requirement: Moderate
 */
double findStreetSegmentLength(StreetSegmentIdx street_segment_id) {
    return gSegmentVector[street_segment_id].segLength;
}

/* AUTHOR: Nicholas Mutlak
 * returns the length of a street stored in gStreetsVector
 * Speed Requirement: High
 */
double findStreetLength(StreetIdx street_id) {
    return gStreetsVector[street_id].streetLength;
}


/* AUTHOR: Nicholas Mutlak
 * returns the time it would take to travel through a street segment
 * TravelTime = Length of Segment / Speed Limit of Segment
 * Speed Requirement: High
 */
double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id) {
    return gSegmentVector[street_segment_id].segTravelTime;
}

/* AUTHOR: Nicholas Mutlak
 * returns the area of a closed polygon feature, using the Shoelace Algorithm
 * Shoelace Algorithm: sum of (yB - yA) * (xB + xA) * 0.5 for all vertices of a polygon in the XYplane
 * Speed Requirement: Moderate
 */
double findFeatureArea(FeatureIdx feature_id) {
    double FeatureArea = 0;

    // loop through feature points and store their LatLon values into a vector
    std::vector<LatLon> latlon_Fpoints;
    int NumFeaturePoints = getNumFeaturePoints(feature_id);
    for (int i = 0; i < NumFeaturePoints; i++) {
        latlon_Fpoints.push_back(getFeaturePoint(feature_id, i));
    }
    
    // check if the feature is not a closed polygon (initial point != final point)
    // LatLon objects have a defined == operator
    if (!(latlon_Fpoints[0] == latlon_Fpoints[NumFeaturePoints - 1])) return 0;

    //calculate the average latitude of all the feature points in radians to be used for the projection formula into the XYplane
    double lat_avg = 0;
    for (int i = 0; i < NumFeaturePoints; i++) {
        lat_avg += latlon_Fpoints[i].latitude() * kDegreeToRadian;
    }
    lat_avg = lat_avg / NumFeaturePoints;
    
    double lat_1, lon_1,
           lat_2, lon_2; //working values to use when running area algorithm
    
    // loop through all the points in the feature for the Shoelace Algorithm
    for (int i = 0; i < NumFeaturePoints - 1; i++) {
        
        // assign initial lat and lon point + convert to radians
        lat_1 = latlon_Fpoints[i].latitude() * kDegreeToRadian;
        lon_1 = latlon_Fpoints[i].longitude() * kDegreeToRadian;

        // assign secondary lat and lon point based on index position in algorithm + convert to radians
        if (i == NumFeaturePoints - 1 - 1) {
            lat_2 = latlon_Fpoints[0].latitude() * kDegreeToRadian;
            lon_2 = latlon_Fpoints[0].longitude() * kDegreeToRadian;
        } else {
            lat_2 = latlon_Fpoints[i + 1].latitude() * kDegreeToRadian;
            lon_2 = latlon_Fpoints[i + 1].longitude() * kDegreeToRadian;
        }
        
        // project radian-converted lat and lon values into XYplane
        double x1 = kEarthRadiusInMeters * lon_1 * cos(lat_avg),
                y1 = kEarthRadiusInMeters * lat_1,

                x2 = kEarthRadiusInMeters * lon_2 * cos(lat_avg),
                y2 = kEarthRadiusInMeters * lat_2;

        FeatureArea += (y2 - y1) * (x2 + x1) * 0.5; // sum (yB - yA) * (xB + xA) * 0.5 into FeatureArea (Shoelace Algorithm)
    }
    if (FeatureArea < 0) FeatureArea *= -1; // take absolute value of the area, if the algorithm outputs it as a negative value

    return FeatureArea;
}

/* AUTHOR: Ridvik Pal
 * returns a vector that contains all the intersections reachable by traveling down 
 * one street segment from the given intersection
 * Speed Requirement: High
 */
std::vector<IntersectionIdx> findAdjacentIntersections(IntersectionIdx intersection_id) {
    return gIntersectVector[intersection_id].adjacentIntersections;
}

/* AUTHOR: Adam Zhang
 * returns the index of the intersection closest to a given LatLon location.
 * Speed Requirement: None
 */
IntersectionIdx findClosestIntersection(LatLon my_position) {
    
    // initialize the minimum distance between a random intersection and the given position (and its index) as the first intersection in the intersection vector
    double minIntersectDist = findDistanceBetweenTwoPoints(my_position, (gIntersectVector[0]).intersectLoc);
    int minDistIndex = 0;

    // loop through all intersections in the intersections vector
    for (std::vector<IntersectStruct>::iterator intersectIT = gIntersectVector.begin(); intersectIT != gIntersectVector.end(); intersectIT++) {
        double distance = findDistanceBetweenTwoPoints(my_position, intersectIT->intersectLoc);

        // track the current minimum distance as we loop over all intersections, and keep track of its index as the minDistIndex
        if (distance < minIntersectDist) {
            minIntersectDist = distance;
            minDistIndex = intersectIT->interIndex;
        }
    }

    // return the result as the intersection index of the intersection vector
    IntersectionIdx interDistResult = (gIntersectVector[minDistIndex]).interIndex;
    return interDistResult;

}

/** AUTHOR: Ridvik Pal
 * Returns a vector that contains all the street segments that connect to an intersection
 * Directly gets the vector from gIntersectVector
 * Speed Requirement: High
 */
std::vector<StreetSegmentIdx> findStreetSegmentsOfIntersection(IntersectionIdx intersection_id) {
    return gIntersectVector[intersection_id].connectedStreetSegments;
}

/*  AUTHOR: Ridvik Pal
 * Returns a vector that contains all the intersections along a given street
 * Directly gets the vector from gStreetsVector
 * Speed Requirement: High
 */
std::vector<IntersectionIdx> findIntersectionsOfStreet(StreetIdx street_id) {
    return gStreetsVector[street_id].intersectsOfStreet;
}

/* AUTHOR: Ridvik Pal
 * Returns a vector that contains all the intersections at which two given streets intersect at
 * The algorithm sorts all the intersections in each street and then uses set_intersection
 * to find the common ones
 * Speed Requirement: High
 */
std::vector<IntersectionIdx> findIntersectionsOfTwoStreets(StreetIdx street_id1, StreetIdx street_id2) {
    std::vector<IntersectionIdx> result;

    // use the findIntersectionsOfStreet() function to get all intersections for street_id1 and street_id2
    std::vector<IntersectionIdx> intersectionsStreet1 = findIntersectionsOfStreet(street_id1);
    std::vector<IntersectionIdx> intersectionsStreet2 = findIntersectionsOfStreet(street_id2);

    // sort the two intersection arrays as a prerequisite for set_intersection()
    std::sort(intersectionsStreet1.begin(), intersectionsStreet1.end());
    std::sort(intersectionsStreet2.begin(), intersectionsStreet2.end());

    // find the common intersections using set_intersection
    std::set_intersection(intersectionsStreet1.begin(), intersectionsStreet1.end(), intersectionsStreet2.begin(), intersectionsStreet2.end(), std::back_inserter(result));

    return result;
}

/* AUTHOR: Adam Zhang and Ridvik Pal
 * returns a vector containing the street indices of the streets that start with the same letters as a given prefix.
 * Global Structures used: StreetStruct, gStreetsNameMap (Multimap)
 * High level description: find the lowest value string in gStreetsNameMap starting with the prefix,  then linear search downwards until we run out of matches
 * Speed Requirement: High
 */
std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix) {
    std::vector<StreetIdx> partialNamesVector; // return value

    int len = street_prefix.length(); // length of prefix parameter

    // if the user enters an empty string, return with 0
    if (len == 0) {
        return partialNamesVector;
    }

    // get a no space version of the prefix parameter
    street_prefix.erase(std::remove_if(street_prefix.begin(), street_prefix.end(), [](unsigned char x) {    
        return std::isspace(x); }), street_prefix.end());
    len = street_prefix.length();

    // get lower case and no space version of street_prefix
    for (int i = 0; i < len; i++) { 
        street_prefix[i] = tolower(street_prefix[i]);
    }
    len = street_prefix.length();

    auto startItr = std::lower_bound(gStreetsNameVector.begin(), gStreetsNameVector.end(), std::make_pair(street_prefix, 0));

    if (startItr == gStreetsNameVector.end()) { // no street names match, so return empty vector.
        return partialNamesVector;
    } else {
        for (; startItr != gStreetsNameVector.end(); startItr++) {  // otherwise start a linear search downwards, looking for the correct prefix.
            if (startItr->first.substr(0, len) != street_prefix) { // ran out of prefix matches
                return partialNamesVector;  // we have the full vector with all prefixed street names.
            } else {
                partialNamesVector.push_back(startItr->second); // add street id to vector
            }
        }
        return partialNamesVector;
    }
}

/* AUTHOR: Adam Zhang
 * returns the index of the Point Of Interest closest to a given LatLon location.
 * Speed Requirement: None
 */
POIIdx findClosestPOI(LatLon my_position, std::string POItype) {
    std::vector<POIStruct> correctTypePOI;  // initialize a POI struct vector of the correct POI Type (from the function parameter)
    
    // assign vector with all type-matching POIs
    for (int idx = 0; idx < getNumPointsOfInterest(); idx++) {
        if ((gPOIVector[idx]).type == POItype) { 
            correctTypePOI.emplace_back(gPOIVector[idx]);
        }
    }
    
    int correctVecLength = correctTypePOI.size(); // size of vector with correct type 
    double minDist = findDistanceBetweenTwoPoints(my_position, (correctTypePOI[0]).poiLoc); // initialize a minimum distance from list
    double minDistIndex = 0; // initialize the index of the min dist POI

    for (int idx = 1; idx < correctVecLength; idx++) { // loop through the rest of the correct type POIs
        double distance = findDistanceBetweenTwoPoints(my_position, (correctTypePOI[idx]).poiLoc); // distance function
        
        // finding a new minimum distance and tracking its index to be returned
        if (distance < minDist) { 
            minDist = distance;
            minDistIndex = idx;
        }
    }

    POIIdx result = (correctTypePOI[minDistIndex]).index; // result is the POIIdx of the correct struct in the correct type vector.
    return result;
}

/* AUTHOR: Adam Zhang and Ridvik Pal
 * returns the OSM Node Tag's value (std::string, i.e. type = "restaurant", "tourist trap")
 * Speed Requirement: None
 */
std::string getOSMNodeTagValue(OSMID OSMid, std::string key) {
    // find the OSMNodeStruct with the matching OSMid
    auto targetPair = gOSMNodeHash.find(OSMid);

    // check for OSMID not found condition
    if (targetPair == gOSMNodeHash.end()) {
        return ""; // return empty string if not found
    }

    // Find the matching pair in the vector stored in gOSMNodeHash->second
    auto targetValue = std::find_if( //find() ask for a value, use find_if() for condition
            targetPair->second.begin(),
            targetPair->second.end(),
            [key](const std::pair<std::string, std::string>& input) //you want to compare an item
            {return input.first == key; }
    );

    // if we find the actual key we're looking for in the vector, return its type value
    if (targetValue != targetPair->second.end()) {
        return targetValue->second;
    }

    return ""; // if any other key value, return an empty string as a failsafe
}