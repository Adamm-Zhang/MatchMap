/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   m2_Project.h
 * Author: mutlakni
 *
 * Created on March 30, 2023, 9:03 a.m.
 */

#ifndef M2_PROJECT_H
#define M2_PROJECT_H

double projectX(double pointX);
double projectY(double pointY);
double projectLon(double pointLon);
double projectLat(double pointLat);
ezgl::point2d getCentroid(std::vector<ezgl::point2d> projectedXYPoints);

#endif /* M2_PROJECT_H */

