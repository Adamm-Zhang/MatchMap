/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */
#include "globals.h"

//helper functions for drawing
void drawOneWayArrow(double startX, double startY, double toX, double toY, double scaler, bool isLong, ezgl::renderer *rend);
void drawStreetClassification(ezgl::renderer *rend, StreetSegType streetSegClass, 
        std::vector<StreetSegmentIdx>::iterator segItr, 
        std::vector<StreetSegmentIdx>::iterator segEnd);

void drawClosedFeatures(ezgl::renderer *rend){
    rend->set_line_width(1);
        
    for(int i = 0; i < gClosedPoly.size(); i++){
        // Only draw the feature if the area is 1% of the screen size
        if(gClosedPoly[i].first > (worldArea * kClosedPolygonScaleFactor * kClosedPolygonScaleFactor)){ //1% of the screen then draw
            if (rend->get_visible_world().contains(gFeatureVector[gClosedPoly[i].second].min) || rend->get_visible_world().contains(gFeatureVector[gClosedPoly[i].second].max)
            || gClosedPoly[i].first > ((3.19342e+16)/area)){
                rend->set_color(gFeatureVector[gClosedPoly[i].second].featureColour);
                rend->fill_poly(gFeatureVector[gClosedPoly[i].second].featureXYPoints);
            }
        }
    }
}

void drawLineFeatures(ezgl::renderer *rend){
    double lineWidth;
    if (scale > initialScale){
        lineWidth = 1;
    }else{
        lineWidth = 2;
    }
    for (int i = 0; i < gLinePoly.size(); i++){
        rend->set_line_width(lineWidth);
        // Draw all line feature
        for (int j = 0; j < gFeatureVector[gLinePoly[i]].numOfPoints - 1; j++){
            if (gFeatureVector[gLinePoly[i]].featureLength > (worldArea * 0.001 * 0.001)){
                // only draw if the feature points are actually being displayed in the world
                if (rend->get_visible_world().contains(gFeatureVector[gLinePoly[i]].featureXYPoints[j]) || rend->get_visible_world().contains(gFeatureVector[gLinePoly[i]].featureXYPoints[j+1])){
                    // Set the colour for the feature
                    rend->set_color(gFeatureVector[gLinePoly[i]].featureColour);
                    // Draw the line feature
                    rend->draw_line({gFeatureVector[gLinePoly[i]].featureXYPoints[j].x, gFeatureVector[gLinePoly[i]].featureXYPoints[j].y}, {gFeatureVector[gLinePoly[i]].featureXYPoints[j+1].x, gFeatureVector[gLinePoly[i]].featureXYPoints[j+1].y});
                }
            }
        }
    }
}

/* function that draws point features on the map when we reach a certain zoom level. */
void drawPointFeatures(ezgl::renderer *rend){
    if(kPointFeatureArea > (worldArea * kPointScaleFactor * kPointScaleFactor)) { //1% of the screen then draw
        for (int i = 0; i < gSinglePoint.size(); i++){
            if (rend->get_visible_world().contains(gFeatureVector[gSinglePoint[i]].featureXYPoints[0])){
                // If the point feature is 1% of the screen then draw
                rend->set_color(gFeatureVector[gSinglePoint[i]].featureColour);
                rend->fill_rectangle({gFeatureVector[gSinglePoint[i]].featureXYPoints[0].x - 5, gFeatureVector[gSinglePoint[i]].featureXYPoints[0].y - 5},
                                     {gFeatureVector[gSinglePoint[i]].featureXYPoints[0].x + 5, gFeatureVector[gSinglePoint[i]].featureXYPoints[0].y + 5});
            }
        }
    }
}

/* function that draws all streets on the map.
* streets are separated by their type; primary/motorways, which are in orange, secondary in yellow
* and everything else (minor streets) in white. streets also scale on zoom according to a scale function.
* function takes into account curved streets and other lengths consistently updating the starts and froms. */

void drawStreetPath(ezgl::renderer *rend){
    ezgl::color lineColour = {7,157,243};
    double lineWidth = 5;
    rend->set_line_width(lineWidth);
    rend->set_line_cap(ezgl::line_cap::round); // Round ends

    for (int i = 0; i < gSegmentVector.size(); i++){
        if(gSegmentVector[i].pathHighlight){
            bool drawStreetName = true;
            //check for average length / per curvepoint
            if(gSegmentVector[i].segLength / gSegmentVector[i].numOfCurvePoints < 15) {
                    drawStreetName = false;
            }
            double xUpdater = gIntersectVector[gSegmentVector[i].from].intersectXYLoc.x;
            double yUpdater = gIntersectVector[gSegmentVector[i].from].intersectXYLoc.y;

            for(int j = 0; j < gSegmentVector[i].numOfCurvePoints; j++){
                //assign curve points coords and plot to junction
                double xBetween = gSegmentVector[i].curvePoints[j].x;
                double yBetween = gSegmentVector[i].curvePoints[j].y;
                if (rend->get_visible_world().contains(gSegmentVector[i].curvePoints[j]) ||
                        rend->get_visible_world().contains(gSegmentVector[i].curvePoints[j])){
                        rend->set_color(lineColour);
                        rend->set_line_width(lineWidth);
                        rend->draw_line({xUpdater,yUpdater},{xBetween,yBetween});
                        if(drawStreetName) displayStreetName(i, xUpdater, yUpdater, xBetween, yBetween, lineWidth, rend);
                        if(scale/initialScale < 0.012){    // if currentscale/init < something
                            // draw an arrow for one way streets.
                            if(gSegmentVector[i].oneWay && (fabs(xBetween - xUpdater) > 7 || fabs(yBetween - yUpdater) > 7) && gSegmentVector[i].segLength < 25){
                                drawOneWayArrow(xUpdater, yUpdater, xBetween, yBetween, scale, false, rend);
                            }
                            else if(gSegmentVector[i].oneWay && (fabs(xBetween - xUpdater) > 15 || fabs(yBetween - yUpdater) > 15) && gSegmentVector[i].segLength >= 25){
                                drawOneWayArrow(xUpdater, yUpdater, xBetween, yBetween, scale, true, rend);
                            }
                        }
                    }
                    //update coords to continue along street segment
                    xUpdater = xBetween;
                    yUpdater = yBetween;
                }
                rend->set_color(lineColour);
                rend->draw_line({xUpdater,yUpdater},{gIntersectVector[gSegmentVector[i].to].intersectXYLoc.x,gIntersectVector[gSegmentVector[i].to].intersectXYLoc.y}); //draw last line to destination intersection coords

                // draw the last name for the street
                if(drawStreetName) displayStreetName(i, xUpdater, yUpdater, gIntersectVector[gSegmentVector[i].to].intersectXYLoc.x,
                                                     gIntersectVector[gSegmentVector[i].to].intersectXYLoc.y,
                                                     lineWidth,
                                                     rend);
                else displayStreetName(i,gIntersectVector[gSegmentVector[i].from].intersectXYLoc.x,
                                       gIntersectVector[gSegmentVector[i].from].intersectXYLoc.y,
                                       gIntersectVector[gSegmentVector[i].to].intersectXYLoc.x,
                                       gIntersectVector[gSegmentVector[i].to].intersectXYLoc.y,
                                       lineWidth,
                                       rend);
        }
    }
}

void drawStreets(ezgl::renderer *rend){
    std::vector<StreetSegmentIdx>::iterator segItr, segEnd;
    if(scale <= 600){
        //Draw MINOR
        segItr = gMinorSegs.begin();
        segEnd = gMinorSegs.end();
        drawStreetClassification(rend, MINOR, segItr, segEnd);
        
        segItr = gSubMinorSegs.begin();
        segEnd = gSubMinorSegs.end();
        drawStreetClassification(rend, SUBMINOR, segItr, segEnd);
    }
    if(scale <= 2000){
        //Draw LINKS and SECONDARY
        segItr = gLinkSegs.begin();
        segEnd = gLinkSegs.end();
        drawStreetClassification(rend, MOTORWAY_LINK, segItr, segEnd);
        
        segItr = gSecondarySegs.begin();
        segEnd = gSecondarySegs.end();
        drawStreetClassification(rend, SECONDARY, segItr, segEnd);    
    }
    //Draw PRIMARY AND MOTORWAYS
    segItr = gMotorwaySegs.begin();
    segEnd = gMotorwaySegs.end();
    drawStreetClassification(rend, MOTORWAY, segItr, segEnd);
    
    segItr = gPrimarySegs.begin();
    segEnd = gPrimarySegs.end();    
    drawStreetClassification(rend, PRIMARY, segItr, segEnd);
}

void drawPOIs(ezgl::renderer *rend){
    for(int i = 0; i < gPOIVector.size(); i++){
        if (rend->get_visible_world().contains(gPOIVector[i].poiXYLoc)){
            if( kPoiArea > (worldArea * kPointScaleFactor * kPointScaleFactor)) { //1% of the screen then draw
            // If the poi area is greater than 1% of the screen then draw
                // depending on the type of POI, place the correct icon
                //food
                if(gPOIVector[i].type == "cafe" || gPOIVector[i].type == "restaurant" || 
                    gPOIVector[i].type == "fast_food" || gPOIVector[i].type == "internet_cafe" ||
                    gPOIVector[i].type == "bar" || gPOIVector[i].type == "biergarten" ||
                    gPOIVector[i].type == "food_court" || gPOIVector[i].type == "ice_cream"||
                    gPOIVector[i].type == "pub" ){
                    rend->draw_surface(surface_food,gPOIVector[i].poiXYLoc,0.12);  
                }
                //education
                else if(gPOIVector[i].type == "library" || gPOIVector[i].type == "college" ||
                        gPOIVector[i].type == "driving_school" || gPOIVector[i].type == "kindergarten" ||
                        gPOIVector[i].type == "language_school" || gPOIVector[i].type == "toy_library" ||
                        gPOIVector[i].type == "training" || gPOIVector[i].type == "music_school" ||
                        gPOIVector[i].type == "school" || gPOIVector[i].type == "traffic_park" ||
                        gPOIVector[i].type == "university"){
                    rend->draw_surface(surface_education,gPOIVector[i].poiXYLoc,0.12);
                }
                //transportation
                else if(gPOIVector[i].type == "bicycle_parking" || gPOIVector[i].type == "bicycle_repair_station" ||
                        gPOIVector[i].type == "bicycle_rental" || gPOIVector[i].type == "boat_rental" || 
                        gPOIVector[i].type == "boat_sharing" || gPOIVector[i].type == "bus_station" ||
                        gPOIVector[i].type == "car_rental" || gPOIVector[i].type == "car_sharing" ||
                        gPOIVector[i].type == "car_wash" || gPOIVector[i].type == "compressed_air" ||
                        gPOIVector[i].type == "vehicle_inspection" || gPOIVector[i].type == "charging_station" ||
                        gPOIVector[i].type == "driver_training" || gPOIVector[i].type == "ferry_terminal" ||
                        gPOIVector[i].type == "fuel" || gPOIVector[i].type == "motorcycle_parking" ||
                        gPOIVector[i].type == "parking" || gPOIVector[i].type == "taxi"){
                    rend->draw_surface(surface_transportation,gPOIVector[i].poiXYLoc,0.12);
                }
                //financial
                else if(gPOIVector[i].type == "atm" || gPOIVector[i].type == "bank" ||
                        gPOIVector[i].type == "bureau_de_change"){
                    rend->draw_surface(surface_financial,gPOIVector[i].poiXYLoc,0.12);
                }
                //healthcare
                else if(gPOIVector[i].type == "clinic" || gPOIVector[i].type == "pharmacy" || 
                        gPOIVector[i].type == "dentist" || gPOIVector[i].type == "doctors" ||
                        gPOIVector[i].type == "emergency_access_point" || gPOIVector[i].type == "hospital" ||
                        gPOIVector[i].type == "baby_hatch" || gPOIVector[i].type == "nursing_home" ||
                        gPOIVector[i].type == "social_facility" || gPOIVector[i].type == "veterinary"){
                    rend->draw_surface(surface_healthcare,gPOIVector[i].poiXYLoc,0.12);
                }
                //tourism and entertainment
                else if(gPOIVector[i].type == "caravan_site" || gPOIVector[i].type == "chalet" ||
                        gPOIVector[i].type == "guest_house" || gPOIVector[i].type == "hostel" ||
                        gPOIVector[i].type == "hotel" || gPOIVector[i].type == "motel" ||
                        gPOIVector[i].type == "gallery" || gPOIVector[i].type == "museum" ||
                        gPOIVector[i].type == "aquarium" || gPOIVector[i].type == "theme_park" ||
                        gPOIVector[i].type == "zoo" || gPOIVector[i].type == "artwork" ||
                        gPOIVector[i].type == "attraction" || gPOIVector[i].type == "information" ||
                        gPOIVector[i].type == "picnic site" || gPOIVector[i].type == "viewpoint" ||
                        gPOIVector[i].type == "arts_centre" || gPOIVector[i].type == "brothel" ||
                        gPOIVector[i].type == "casino" || gPOIVector[i].type == "cinema" ||
                        gPOIVector[i].type == "community_centre" || gPOIVector[i].type == "conference_centre"  ||
                        gPOIVector[i].type == "events_venue" || gPOIVector[i].type == "exhibition_centre" || 
                        gPOIVector[i].type == "fountain" || gPOIVector[i].type == "gambling" ||
                        gPOIVector[i].type == "love_hotel" || gPOIVector[i].type == "music_venue" ||
                        gPOIVector[i].type == "stripclub" || gPOIVector[i].type == "swingerclub" ||
                        gPOIVector[i].type == "nightclub" || gPOIVector[i].type == "planetarium" ||
                        gPOIVector[i].type == "public_bookcase" || gPOIVector[i].type == "social_centre" ||
                        gPOIVector[i].type == "studio" || gPOIVector[i].type == "theatre"){
                    
                    rend->draw_surface(surface_entertainment,gPOIVector[i].poiXYLoc,0.12);
                }
                //public service
                else if(gPOIVector[i].type == "courthouse" || gPOIVector[i].type == "fire_station" || 
                        gPOIVector[i].type == "police" || gPOIVector[i].type == "post_box" ||
                        gPOIVector[i].type == "post_depot" || gPOIVector[i].type == "post_office" ||
                        gPOIVector[i].type == "prison" || gPOIVector[i].type == "ranger_station" ||
                        gPOIVector[i].type == "townhall"){
                    
                    rend->draw_surface(surface_publicService,gPOIVector[i].poiXYLoc,0.12);
                }
                //waste management
                else if(gPOIVector[i].type == "sanitary_dump_station" || gPOIVector[i].type == "recycling" ||
                        gPOIVector[i].type == "waste_basket" || gPOIVector[i].type == "waste_disposal" ||
                        gPOIVector[i].type == "waste_transfer_station"){
                        
                    rend->draw_surface(surface_waste,gPOIVector[i].poiXYLoc,0.12);
                }
                //general POIs
                else{
                    rend->draw_surface(surface_general,gPOIVector[i].poiXYLoc,0.12);
                }
            }
            drawPOIHighlights(rend,i); // draw any highlights

        }
    }
}

/* function that draws all subway stations of a city as blue dots on the map. subway stations
 * based on OSMNodes are stored inside a vector initialized in m1.cpp OSM. */
void drawSubWayStations(ezgl::renderer *rend){
    rend->set_color(ezgl::color(90, 106, 210)); // blue
    for(int i = 0; i < osmSubwayStations.size(); i++){
        double x = projectX(osmSubwayStations[i]->coords().longitude());
        double y = projectY(osmSubwayStations[i]->coords().latitude());

        rend->draw_surface(surface_subway, {x,y}, 0.12);
    }
}


/* function that draws one way arrows on streets that scale with the zoom level */
void drawOneWayArrow(double startX, double startY, double toX, double toY, double scaler, bool isLong, ezgl::renderer *rend){
    if (rend->get_visible_world().contains(startX, startY) && rend->get_visible_world().contains(toX, toY)){
        // draw the arrow head. if the segment is short (defined by segment length < 10), then we draw one arrow starting at the from point
        // lengths of arrow parameters
        double arrowLength = 2.5;
        double headLength = 1.2;
        double headWidth = 1 * (scaler / (scaler + 5));     // bigger values for smaller scalers.

        double lineWidth1;

        if(scale > 50){
            lineWidth1 = 1;
        }else{
            lineWidth1 = 2;
        }

        // get the trajectory of the street segment
        double deltaX = toX - startX;
        double deltaY = toY - startY;

        double angle = atan2(deltaY, deltaX);

        // find all parameter lengths of the arrow head and arrow body to be drawn.
        double xLength = arrowLength * cos(angle);
        double yLength = arrowLength * sin(angle);
        double headXLength = headLength * cos(angle);
        double headYLength = headLength * sin(angle);
        double headXWidth = headWidth * sin(angle);
        double headYWidth = headWidth * cos(angle);

        // color and line width
        if(!toggle_bg) rend->set_color(ezgl::color {130, 130, 130});
        else rend->set_color(ezgl::color {200, 200, 200});

        rend->set_line_width(lineWidth1);

        double arrowXStart = startX + xLength;
        double arrowYStart = startY + yLength;

        // shift on line from the from point.
        double shiftX = 2 * cos(angle);
        double shiftY = 2 * sin(angle);

        if(!isLong){
            rend->draw_line({startX, startY}, {startX + xLength, startY + yLength});

            rend->fill_poly({{arrowXStart  - headXLength - headXWidth, arrowYStart - headYLength + headYWidth},
                             {arrowXStart, arrowYStart},
                             {arrowXStart - headXLength + headXWidth, arrowYStart - headYLength - headYWidth}});

            // if segment is long, then we draw arrow starting at a constant offset from the from point to make it look more natural. only if long to avoid collision with text
        }else{
            rend->draw_line({startX + shiftX,  startY + shiftY}, { startX + shiftX + xLength, startY + shiftY + yLength});

            rend->fill_poly({{arrowXStart + shiftX - headXLength - headXWidth, arrowYStart + shiftY - headYLength + headYWidth},    // maybe we draw 2 arrow for very long streets using the shift.
                             {arrowXStart + shiftX , arrowYStart + shiftY},
                             {arrowXStart + shiftX - headXLength + headXWidth, arrowYStart + shiftY - headYLength - headYWidth}});
        }
    }
}

void drawStreetClassification(ezgl::renderer *rend, StreetSegType streetSegClass, 
        std::vector<StreetSegmentIdx>::iterator segItr, 
        std::vector<StreetSegmentIdx>::iterator segEnd
        ){
    rend->set_line_cap(ezgl::line_cap::round); // Round ends
    for(; segItr < segEnd; segItr++){
        
        // color and width parameters for segments
        ezgl::color segmentColour = ezgl::BLACK;
        double lineWidth = 1;

        switch(streetSegClass){
            case 5: //minor
                gSegmentVector[*segItr].drawMe = (scale <= 800) ? true : false;
                if(gSegmentVector[*segItr].drawMe){
                    segmentColour = (toggle_bg) ? ezgl::color{52,62,74}  : gSegmentVector[*segItr].segmentColour;
                    lineWidth = (zoom_level >= 6) ? 600/scale : gSegmentVector[*segItr].drawScaleFactor;
                }
                break;
            case 6: // sub minor
                gSegmentVector[*segItr].drawMe = (scale <= 600) ? true : false;
                if(gSegmentVector[*segItr].drawMe){
                    segmentColour = (toggle_bg) ? ezgl::color{52,62,74} : gSegmentVector[*segItr].segmentColour;
                    lineWidth = (zoom_level >= 6) ? 400/scale : gSegmentVector[*segItr].drawScaleFactor;
                }
                break;
            case 4: //secondary
                gSegmentVector[*segItr].drawMe = (scale <= 8000) ? true : false;
                if(gSegmentVector[*segItr].drawMe){
                    segmentColour = (toggle_bg) ? ezgl::color{52,62,74} : gSegmentVector[*segItr].segmentColour;
                    lineWidth = (zoom_level >= 6) ? 800/scale : gSegmentVector[*segItr].drawScaleFactor;
                }
                break;
            case 2: //motorways and trunks
                segmentColour = (toggle_bg) ? ezgl::color{72,87,130} : gSegmentVector[*segItr].segmentColour;
                lineWidth = (zoom_level >= 6) ? 1600/scale: gSegmentVector[*segItr].drawScaleFactor;
                break;
            case 1: //primary
                segmentColour = (toggle_bg) ? ezgl::color{72,87,150} : gSegmentVector[*segItr].segmentColour;
                lineWidth = (zoom_level >= 6) ? 1200/scale : gSegmentVector[*segItr].drawScaleFactor;
                break;
            case 3: //motorway and trunk links
                gSegmentVector[*segItr].drawMe = (scale <= 2500) ? true : false;
                if(gSegmentVector[*segItr].drawMe){
                    segmentColour = (toggle_bg) ? ezgl::color{52,62,74} : gSegmentVector[*segItr].segmentColour;
                    lineWidth = (zoom_level >= 6) ? 800/scale : gSegmentVector[*segItr].drawScaleFactor;
                }
                break;
            default: //Should never happen
                std::cout << "BRUH\n";
                break;
        }

        if(gStreetsVector[gSegmentVector[*segItr].myStreet].streetHighlight){
            if(gStreetsVector[gSegmentVector[*segItr].myStreet].whichStreetEntry) segmentColour = ezgl::BLUE;
            else segmentColour = ezgl::CYAN;
        }

        if(gSegmentVector[*segItr].drawMe){
            if(scale > initialScale){
                lineWidth = 1;
            }

            bool drawStreetName = 1;
            //check for average length / per curvepoint
            if(gSegmentVector[*segItr].segLength / gSegmentVector[*segItr].numOfCurvePoints < 15) {
                    drawStreetName = 0;
                }

            double xUpdater = gIntersectVector[gSegmentVector[*segItr].from].intersectXYLoc.x;
            double yUpdater = gIntersectVector[gSegmentVector[*segItr].from].intersectXYLoc.y;

            for(int j = 0; j < gSegmentVector[*segItr].numOfCurvePoints; j++){
                    double xBetween = gSegmentVector[*segItr].curvePoints[j].x;
                    double yBetween = gSegmentVector[*segItr].curvePoints[j].y;
                    if (rend->get_visible_world().contains(gSegmentVector[*segItr].curvePoints[j])
                        || rend->get_visible_world().contains(gSegmentVector[*segItr].curvePoints[j])){
                        //assign curve points coords and plot to junction

                        rend->set_color(segmentColour);
                        rend->set_line_width(lineWidth);

                        rend->draw_line({xUpdater,yUpdater},{xBetween,yBetween});
                        if(drawStreetName) displayStreetName(*segItr, xUpdater, yUpdater, xBetween, yBetween, lineWidth, rend);
                        if(scale/initialScale < 0.012){    // if currentscale/init < something
                            // draw an arrow for one way streets.
                            if(gSegmentVector[*segItr].oneWay && (fabs(xBetween - xUpdater) > 7 || fabs(yBetween - yUpdater) > 7) && gSegmentVector[*segItr].segLength < 25){
                                drawOneWayArrow(xUpdater, yUpdater, xBetween, yBetween, scale, false, rend);
                            }
                            else if(gSegmentVector[*segItr].oneWay && (fabs(xBetween - xUpdater) > 15 || fabs(yBetween - yUpdater) > 15) && gSegmentVector[*segItr].segLength >= 25){
                                drawOneWayArrow(xUpdater, yUpdater, xBetween, yBetween, scale, true, rend);
                            }
                        }
                    }
                    //update coords to continue along street segment
                    xUpdater = xBetween;
                    yUpdater = yBetween;
                }
                rend->set_color(segmentColour);
                rend->set_line_width(lineWidth);
                rend->draw_line({xUpdater,yUpdater},{gIntersectVector[gSegmentVector[*segItr].to].intersectXYLoc.x,gIntersectVector[gSegmentVector[*segItr].to].intersectXYLoc.y}); //draw last line to destination intersection coords

                // draw the last name for the street
                if(drawStreetName) displayStreetName(*segItr, xUpdater, yUpdater, gIntersectVector[gSegmentVector[*segItr].to].intersectXYLoc.x,
                        gIntersectVector[gSegmentVector[*segItr].to].intersectXYLoc.y,
                        lineWidth,
                        rend);
                else displayStreetName(*segItr,gIntersectVector[gSegmentVector[*segItr].from].intersectXYLoc.x,
                                        gIntersectVector[gSegmentVector[*segItr].from].intersectXYLoc.y,
                                        gIntersectVector[gSegmentVector[*segItr].to].intersectXYLoc.x,
                                        gIntersectVector[gSegmentVector[*segItr].to].intersectXYLoc.y,
                                        lineWidth,
                                        rend);
        }
    }
}
