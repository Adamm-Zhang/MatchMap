/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   m2_drawMapNames.h
 * Author: mutlakni
 *
 * Created on March 29, 2023, 11:37 p.m.
 */

#ifndef M2_DRAWMAPNAMES_H
#define M2_DRAWMAPNAMES_H

void displayStreetName(int i, double xFrom, double yFrom, double xTo, double yTo, double lineWidth, ezgl::renderer *rend);
void displayClosedFeatureName(ezgl::renderer *rend);
void displayPointFeatureName(ezgl::renderer *rend);
void displayLineFeatureName(ezgl::renderer *rend);
void displayPOIName(ezgl::renderer *rend);


#endif /* M2_DRAWMAPNAMES_H */

