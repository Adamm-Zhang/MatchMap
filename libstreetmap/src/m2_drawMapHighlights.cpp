/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */
#include "globals.h"

/* function that draws intersection points based on whether or not the specific intersection point's
 * highlight boolean is true. Highlights are purple circles that scale with zoom based on a scale formula. */
void drawIntersectHighlights(ezgl::renderer *rend){
    rend->set_color(ezgl::PURPLE);
    for (int i = 0; i < gIntersectVector.size(); i++) {

        double x = gIntersectVector[i].intersectXYLoc.x;
        double y = gIntersectVector[i].intersectXYLoc.y;

        if (rend->get_visible_world().contains(x, y)){
            if(gIntersectVector[i].intersectHighlight){ //check if this is a highlighted intersection (been clicked on)
                // prevent the display from happening twice to same highlighted intersection when drawmaincanvas is called again.
                if(!gIntersectVector[i].displayed) {
                    gIntersectVector[i].displayed = true;
                }
                rend->draw_surface(intersection_highlight, {x,y}, 0.18);
            }
            else{
                gIntersectVector[i].displayed = false;
            }
        }
    }
}

/* function that draws POI resulting from search entries in the POI search bars as dots on the map.
 * if the point is within the map, we will draw the points based on a scaling formula.*/
void drawFeatureHighlights(ezgl::renderer *rend){
//    double width; //define intersection circle size (in meters)
            
    for (int i = 0; i < gFeatureVector.size(); i++) {
        if(gFeatureVector[i].featureHighlight){ //check if this is a highlighted intersection
            if (rend->get_visible_world().contains(gFeatureVector[i].centroid.x, gFeatureVector[i].centroid.y)){
                if(gFeatureVector[i].whichFeatureEntry) {
                    rend->draw_surface(find_feature_2, gFeatureVector[i].centroid, 0.15);
                }else {
                    rend->draw_surface(find_feature_1, gFeatureVector[i].centroid, 0.15);
                }
            }
        }
    }
}

/* function that draws POI resulting from search entries in the POI search bars as dots on the map.
 * if the point is within the map, we will draw the points based on a scaling formula.*/
void drawPOIHighlights(ezgl::renderer *rend, int i){
    double x = gPOIVector[i].poiXYLoc.x;
    double y = gPOIVector[i].poiXYLoc.y;

    if(gPOIVector[i].poiHighlight){ //check if this is a highlighted intersection (been clicked on)
        if(gPOIVector[i].whichPOIEntry) {
            rend->draw_surface(find_poi_2, {x, y}, 0.15);
        }
        else {
            rend->draw_surface(find_poi_1, {x,y}, 0.15);
        }

    }

}

