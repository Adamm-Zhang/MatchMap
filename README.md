# MatchMap
> *"The perfect match for your routing needs"*
## IT IS AN ACADEMIC OFFENSE TO COPY CODE. THIS IS SIMPLY FOR REFERENCE
MatchMap is a GIS software that is optimized for viewing destinations and routing that runs natively on Linux (was tested on Debian 11). It is built off the OpenStreetMap Database via their OSMDatabaseAPI.h and StreetsDatabaseAPI.h C++ APIs. It was created by a three person team: Nicholas Mutlak, Adam Zhang, and Ridvik Pal for the ECE297 course at the University of Toronto.

![image](https://user-images.githubusercontent.com/105998663/235358316-f7a49aef-5f65-4284-932b-e96cba51972c.png)


## User Interface
It has a user interface desinged in GTK and utilizes UofT's EZGL renderer. The primary motive behind the UI was to create a simple, intuitive, and non-cluttered UI, where elements can be revealed at the user's discretion. For example, the search elements are hidden behind the "Search for Map Elements" dialog box, so as to not clutter the main screen:

![image](https://user-images.githubusercontent.com/105998663/235359556-a03ac9d5-5afe-4567-beaa-ec5fb820b9e2.png)

MatchMap includes a custom icon set as well:
<p align="center">
  <img src="https://user-images.githubusercontent.com/105998663/235360092-0795b0bf-adfa-4118-a357-a7d9bf229855.png" />
</p>

The colour scheme is pastel to reduce eye strain and well differentiated to provide easy distinction between map elements:
![image](https://user-images.githubusercontent.com/105998663/235359975-1838ee1e-19b9-4eed-b57a-f08b8cd8447f.png)

## Searching
MatchMap has excellent searching features, with almost every map element being searchable. This includes streets, features, intersections, and POIs. For example, searching for Tim Hortons:
![image](https://user-images.githubusercontent.com/105998663/235360314-430c3694-8adf-4b27-9980-96a7a7c56f9c.png)

Every search dialog box also includes autocomplete:
![image](https://user-images.githubusercontent.com/105998663/235361038-21d3af72-7682-46aa-aa51-0d2b8a15f13a.png)

## Routing
