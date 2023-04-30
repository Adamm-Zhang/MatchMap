/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */

#include "globals.h"

std::vector<FeatureIdx> findFeatureIdsFromPartialFeatureName(std::string feature_prefix);
std::vector<POIIdx> findPOIIdsFromPartialPOIName(std::string poi_prefix);

// function that gives the first 3 street names and index in a pair corresponding to a string search entry.
std::vector<std::pair<StreetIdx, std::string>> getStreetSuggestions (std::string searchEntry){
    std::vector<std::pair<StreetIdx, std::string>> result;
    for(int i = 0; i < findStreetIdsFromPartialStreetName(searchEntry).size(); i++){

        StreetIdx currentIdx = findStreetIdsFromPartialStreetName(searchEntry)[i];
        result.emplace_back(std::pair<StreetIdx, std::string> (currentIdx, getStreetName(currentIdx)));
    }

    return result;
}

/* function that returns a list of feature suggestions for the features search box */
std::vector<std::string> getFeatureSuggestions (std::string searchEntry){
    std::vector<std::string> result;

    for(int i = 0; i < findFeatureIdsFromPartialFeatureName(searchEntry).size(); i++){
        FeatureIdx currentIdx = findFeatureIdsFromPartialFeatureName(searchEntry)[i];
        result.push_back(getFeatureName(currentIdx));
    }
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

/* function that gets a list of POI suggestions */
std::vector<std::string> getPOISuggestions (std::string searchEntry){
    std::vector<std::string> result;

    for(int i = 0; i < findPOIIdsFromPartialPOIName(searchEntry).size(); i++){

        POIIdx currentIdx = findPOIIdsFromPartialPOIName(searchEntry)[i];
        result.push_back(getPOIName(currentIdx));
    }
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

/* function that returns a vector of feature indexes of features starting with the same
 * prefix as the one inputted. */
std::vector<FeatureIdx> findFeatureIdsFromPartialFeatureName(std::string feature_prefix){
    std::vector<FeatureIdx> partialNamesVector;

    int len = feature_prefix.length(); // length of prefix parameter

    // if the user enters an empty string, return with 0
    if (len == 0) {
        return partialNamesVector;
    }

    // get a no space version of the prefix parameter
    feature_prefix.erase(std::remove_if(feature_prefix.begin(), feature_prefix.end(), [](unsigned char x) {
        return std::isspace(x); }), feature_prefix.end());
    len = feature_prefix.length();

    // get lower case and no space version of street_prefix
    for (int i = 0; i < len; i++) {
        feature_prefix[i] = tolower(feature_prefix[i]);
    }
    len = feature_prefix.length();

    auto startItr = std::lower_bound(gFeaturesNameVector.begin(), gFeaturesNameVector.end(), std::make_pair(feature_prefix, 0));

    if (startItr == gFeaturesNameVector.end()) { // no street names match, so return empty vector.
        return partialNamesVector;
    } else {
        for (; startItr != gFeaturesNameVector.end(); startItr++) {  // otherwise start a linear search downwards, looking for the correct prefix.
            if (startItr->first.substr(0, len) != feature_prefix) { // ran out of prefix matches
                return partialNamesVector;  // we have the full vector with all prefixed street names.
            } else {
                partialNamesVector.push_back(startItr->second); // add street id to vector
            }
        }
        return partialNamesVector;
    }
}

/* function that returns a vector of POI indexes of POI's starting with the same
 * prefix as the one inputted. */
std::vector<POIIdx> findPOIIdsFromPartialPOIName(std::string poi_prefix){
    std::vector<FeatureIdx> partialNamesVector;

    int len = poi_prefix.length(); // length of prefix parameter

    // if the user enters an empty string, return with 0
    if (len == 0) {
        return partialNamesVector;
    }

    // get a no space version of the prefix parameter
    poi_prefix.erase(std::remove_if(poi_prefix.begin(), poi_prefix.end(), [](unsigned char x) {
        return std::isspace(x); }), poi_prefix.end());
    len = poi_prefix.length();

    // get lower case and no space version of street_prefix
    for (int i = 0; i < len; i++) {
        poi_prefix[i] = tolower(poi_prefix[i]);
    }
    len = poi_prefix.length();

    auto startItr = std::lower_bound(gPOINameVector.begin(), gPOINameVector.end(), std::make_pair(poi_prefix, 0));

    if (startItr == gPOINameVector.end()) { // no street names match, so return empty vector.
        return partialNamesVector;
    } else {
        for (; startItr != gPOINameVector.end(); startItr++) {  // otherwise start a linear search downwards, looking for the correct prefix.
            if (startItr->first.substr(0, len) != poi_prefix) { // ran out of prefix matches
                return partialNamesVector;  // we have the full vector with all prefixed street names.
            } else {
                partialNamesVector.push_back(startItr->second); // add street id to vector
            }
        }
        return partialNamesVector;
    }
}