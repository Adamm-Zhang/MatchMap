/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */


#include "globals.h"

/* Define helper functions for projecting onto the canvas */
double projectX(double pointX){
    pointX = pointX * kDegreeToRadian; //convert lon to radians
    pointX = kEarthRadiusInMeters * pointX * averageLatitude; //project lon into X plane
    return pointX;
}

double projectY(double pointY){
    pointY = pointY * kDegreeToRadian; //convert lat to radians
    pointY = kEarthRadiusInMeters * pointY;  //project lat into Y plane
    return pointY;
}


/* lon to x */
double projectLon(double pointLon){
    pointLon = pointLon/(kEarthRadiusInMeters * averageLatitude); // project X into longitude plane
    pointLon = pointLon/kDegreeToRadian;    // convert back into degrees
    return pointLon;
}

/* lat to y */
double projectLat(double pointLat){
    pointLat = pointLat/kEarthRadiusInMeters; // project Y into latitude
    pointLat = pointLat/kDegreeToRadian; // convert back into degrees
    return pointLat;
}

/* Define helper function to get the centroid of an ezgl::point2d */
ezgl::point2d getCentroid(std::vector<ezgl::point2d> projectedXYPoints){
    ezgl::point2d centroid = {0.0, 0.0};
    double signedArea = 0.0;
    double x0 = 0.0; // Current vertex X
    double y0 = 0.0; // Current vertex Y
    double x1 = 0.0; // Next vertex X
    double y1 = 0.0; // Next vertex Y
    double a = 0.0;  // Partial signed area

    int i = 0;
    for (i = 0; i < projectedXYPoints.size() - 1; i++){
        x0 = projectedXYPoints[i].x;
        y0 = projectedXYPoints[i].y;
        x1 = projectedXYPoints[i+1].x;
        y1 = projectedXYPoints[i+1].y;
        a = x0*y1 - x1*y0;
        signedArea += a;
        centroid.x += (x0 + x1)*a;
        centroid.y += (y0 + y1)*a;
    }

    // Do last vertex separately to avoid performing an expensive
    // modulus operation in each iteration.
    x0 = projectedXYPoints[i].x;
    y0 = projectedXYPoints[i].y;
    x1 = projectedXYPoints[0].x;
    y1 = projectedXYPoints[0].y;
    a = x0*y1 - x1*y0;
    signedArea += a;
    centroid.x += (x0 + x1)*a;
    centroid.y += (y0 + y1)*a;

    signedArea *= 0.5;
    centroid.x /= (6.0*signedArea);
    centroid.y /= (6.0*signedArea);

    return centroid;
}