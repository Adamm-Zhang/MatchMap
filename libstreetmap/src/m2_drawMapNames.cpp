/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */

#include "globals.h"

void drawMultipleLinesText(ezgl::point2d point, std::string const &text, int font_size, ezgl::renderer *rend);

/*
 * Displays all street names
 */
void displayStreetName(int i, double xFrom, double yFrom, double xTo, double yTo, double lineWidth, ezgl::renderer *rend){
    //display the name of a street overtop of its line
    if(gStreetsVector[gSegmentVector[i].myStreet].streetName[0] != '<'){
        if (rend->get_visible_world().contains(xFrom, yFrom)){
            //display the name of the street on a street segment
            //calculate rotation
            double rotation = 0;
            if(xTo - xFrom != 0){
                rotation = atan((yTo - yFrom)/(xTo - xFrom)) / kDegreeToRadian; // calculate angle of rotation
                if(rotation < 0) rotation += 360;
            }
            rend->set_text_rotation(rotation);

            //calculate center of street segment (not sure how to deal with curves just yet - doesn't handle them well currently)
            double centerX = (xFrom + xTo) / 2;
            double centerY = (yFrom + yTo) / 2;

            //calculate the length of the segment across which we are writing the name
            double length = sqrt(pow((xTo - xFrom),2) + pow(yTo - yFrom,2));
            double augmentedLength = 0.9 * length;

            //set to bold, size 12, green font
            ezgl::font_slant slant {CAIRO_FONT_SLANT_NORMAL}; //need to reference ezgl and not ezgl::renderer for this to work
            ezgl::font_weight weight {CAIRO_FONT_WEIGHT_NORMAL};
            rend->format_font("OpenSans",slant,weight);
            rend->set_font_size(12);
            if(toggle_bg) rend->set_color(ezgl::WHITE);
            else rend->set_color(ezgl::BLACK);

            //FORMAT STREETNAME TO LOOK NICE HERE

            //output the streetname
            if(gSegmentVector[i].oneWay) rend->draw_text({centerX,centerY},gStreetsVector[gSegmentVector[i].myStreet].streetName,augmentedLength,lineWidth);
            else rend->draw_text({centerX,centerY},gStreetsVector[gSegmentVector[i].myStreet].streetName, length,lineWidth);
        }
    }
}

/*
 * Displays all closed feature names using a multiple line text algorithm
 */
void displayClosedFeatureName(ezgl::renderer *rend){
    // no text rotation required, because this is a closed feature
    rend->set_text_rotation(0.0);
    // Format the text
    ezgl::font_slant slant {CAIRO_FONT_SLANT_NORMAL}; //need to reference ezgl and not ezgl::renderer for this to work
    ezgl::font_weight weight {CAIRO_FONT_WEIGHT_NORMAL};
    rend->format_font("OpenSans",slant,weight);
    if(toggle_bg) rend->set_color(ezgl::WHITE);
    else rend->set_color(ezgl::BLACK);
    int font_size = 12;

    for (int i = 0; i < gClosedPoly.size();i++){
        // Only draw the name for the feature if the feature area is greater than 4% of the screen
        if (gClosedPoly[i].first > (worldArea * kClosedPolygonTextScaleFactor * kClosedPolygonTextScaleFactor)){
            if (gFeatureVector[gClosedPoly[i].second].featureName[0] != '<'){// Check if the name exists
                if (rend->get_visible_world().contains(gFeatureVector[gClosedPoly[i].second].centroid)){
                    drawMultipleLinesText(gFeatureVector[gClosedPoly[i].second].centroid, gFeatureVector[gClosedPoly[i].second].featureName, font_size, rend);
                }
            }
        }
    }
}

/*
 * Display all line feature names
 */
void displayLineFeatureName(ezgl::renderer *rend){
    // Format the text
    ezgl::font_slant slant {CAIRO_FONT_SLANT_NORMAL}; //need to reference ezgl and not ezgl::renderer for this to work
    ezgl::font_weight weight {CAIRO_FONT_WEIGHT_NORMAL};
    rend->format_font("OpenSans",slant,weight);
    rend->set_font_size(12);
    if(toggle_bg) rend->set_color(ezgl::WHITE);
    else rend->set_color(ezgl::BLACK);

    for (int i = 0; i < gLinePoly.size(); i++){
        
        for (int j = 0; j < gFeatureVector[gLinePoly[i]].numOfPoints - 1; j++){
            if (rend->get_visible_world().contains(gFeatureVector[gLinePoly[i]].featureXYPoints[j])){
                if (gFeatureVector[gLinePoly[i]].featureName[0] != '<'){
                    // if it is defined, then draw the name
                    double xFrom = gFeatureVector[gLinePoly[i]].featureXYPoints[j].x;
                    double yFrom = gFeatureVector[gLinePoly[i]].featureXYPoints[j].y;
                    double xTo = gFeatureVector[gLinePoly[i]].featureXYPoints[j+1].x;
                    double yTo = gFeatureVector[gLinePoly[i]].featureXYPoints[j+1].y;

                    double rotation = 0;
                    if(xTo - xFrom != 0){
                        rotation = atan((yTo - yFrom)/(xTo - xFrom)) / kDegreeToRadian; // calculate angle of rotation
                        if(rotation < 0) rotation += 360;
                    }
                    // Set the rotation of the text
                    rend->set_text_rotation(rotation);

                    // Determine the center of the line feature where text will be drawn
                    ezgl::point2d pos = {((xFrom + xTo) / 2), ((yFrom + yTo) / 2)};

                    // Calculate the length of the line feature
                    double length = sqrt(pow((xTo - xFrom),2) + pow(yTo - yFrom,2));

                    // Draw the line feature text
                    rend->draw_text(pos, gFeatureVector[gLinePoly[i]].featureName, length, yTo);
                }
            }
        }
    }
}

/*
 * Displays all point feature names
 */
void displayPointFeatureName(ezgl::renderer *rend){
    rend->set_text_rotation(0.0);

    // Format the text
    ezgl::font_slant slant {CAIRO_FONT_SLANT_NORMAL}; //need to reference ezgl and not ezgl::renderer for this to work
    ezgl::font_weight weight {CAIRO_FONT_WEIGHT_NORMAL};
    rend->format_font("OpenSans",slant,weight);
    if(toggle_bg) rend->set_color(ezgl::WHITE);
    else rend->set_color(ezgl::BLACK);

    int font_size = 12;

    if (kPointFeatureArea > (worldArea * kPointTextScaleFactor * kPointTextScaleFactor)) {
        for (int i = 0; i < gSinglePoint.size(); i++) {
            if (rend->get_visible_world().contains(gFeatureVector[gSinglePoint[i]].featureXYPoints[0])){
                // Check if the point feature name is defined
                if (gFeatureVector[gSinglePoint[i]].featureName[0] != '<') {
                    // if it is defined, then draw the name
                    drawMultipleLinesText(gFeatureVector[gSinglePoint[i]].featureXYPoints[0], gFeatureVector[gSinglePoint[i]].featureName, font_size, rend);
                }
            }
        }
    }
}

/* 
 * Display all POIs names
 */
void displayPOIName(ezgl::renderer *rend){
    rend->set_text_rotation(0);
    // format the text
    ezgl::font_slant slant {CAIRO_FONT_SLANT_NORMAL}; //need to reference ezgl and not ezgl::renderer for this to work
    ezgl::font_weight weight {CAIRO_FONT_WEIGHT_NORMAL};
    rend->format_font("OpenSans",slant,weight);
    int font_size = 9;
    if(toggle_bg) rend->set_color(ezgl::WHITE);
    else rend->set_color(ezgl::BLACK);

    if (kPoiArea > (worldArea * kPointTextScaleFactor * kPointTextScaleFactor)){
        for (int i = 0; i < gPOIVector.size(); i++){
            if (rend->get_visible_world().contains(gPOIVector[i].poiXYLoc)){
                if (gPOIVector[i].name[0] != '<'){
                    drawMultipleLinesText({gPOIVector[i].poiXYLoc.x, gPOIVector[i].poiXYLoc.y - 3}, gPOIVector[i].name, font_size, rend);
                }
            }
        }
    }
}

/* function that draws multiple lines of text if we exceed a certain horizontal length on drawing names */
void drawMultipleLinesText(ezgl::point2d point, std::string const &text, int font_size, ezgl::renderer *rend){
    // create a string stream object iss
    std::istringstream iss{text};
    std::string word;

    rend->set_font_size(font_size);

    // Determine the point where the text will be drawn
    ezgl::point2d draw_point = point;

    // Read by word from the stringstream
    while (iss >> word){
        // determine the position for each point

        draw_point.y -= (worldHeight/screenHeight)*font_size;
        rend->draw_text({draw_point.x,draw_point.y + (worldHeight/screenHeight)*10}, word);
    }
}
