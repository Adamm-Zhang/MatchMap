/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/* 
 * File:   m2_PartialNames.h
 * Author: mutlakni
 *
 * Created on March 30, 2023, 12:54 a.m.
 */

#ifndef M2_PARTIALNAMES_H
#define M2_PARTIALNAMES_H

std::vector<std::pair<StreetIdx, std::string>> getStreetSuggestions (std::string searchEntry);
std::vector<std::string> getFeatureSuggestions (std::string searchEntry);
std::vector<std::string> getPOISuggestions (std::string searchEntry);

#endif /* M2_PARTIALNAMES_H */

