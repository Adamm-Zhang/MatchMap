# MatchMap
> *"The perfect match for your routing needs"*
## IT IS AN ACADEMIC OFFENSE TO COPY CODE. THIS IS SIMPLY FOR REFERENCE
MatchMap is a GIS written in C++ that is optimized for viewing destinations and routing that runs natively on Linux (was tested on Debian 11). It is built off the OpenStreetMap Database via their OSMDatabaseAPI.h and StreetsDatabaseAPI.h C++ APIs. It was created by a three person team: Nicholas Mutlak, Adam Zhang, and Ridvik Pal for the ECE297 course at the University of Toronto.

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

The user interface is extremely responsive, getting between 40-100 fps on average depending on the map selected. The map to view can be selected using the select map button:  
<p align="center">
  <img src="https://user-images.githubusercontent.com/105998663/235361937-5a556ce7-ff6b-43b3-aaf1-e9319e7de2e3.png" />
</p>

## Searching
MatchMap has excellent searching features, with almost every map element being searchable. This includes streets, features, intersections, and POIs. For example, searching for Tim Hortons:
![image](https://user-images.githubusercontent.com/105998663/235360314-430c3694-8adf-4b27-9980-96a7a7c56f9c.png)

Every search dialog box also includes autocomplete:
![image](https://user-images.githubusercontent.com/105998663/235361038-21d3af72-7682-46aa-aa51-0d2b8a15f13a.png)

## Routing
MatchMap has great routing features, with two input options: either the search bars, or simply clicking on two intersections on the map. When you click or search for an intersection a useful dialog box appears on the right that shows the intersection location, connected streets, their speed limits, and their one way statuses.
![image](https://user-images.githubusercontent.com/105998663/235361345-7599030b-1910-40d2-8380-fe0426a85af7.png)

Additionally, MatchMap supports making multiple routes at once, by continuing to provide intersection inputs. These routes are stored until the "Clear All Intersections" button is pressed. This is useful to create a continuous path from one location to another:
![image](https://user-images.githubusercontent.com/105998663/235361443-f58c3369-2ed7-4a87-9c8d-665978b5a9f9.png)

MatchMap supports routing across almost all map elements, including intersections, streets, POIs and Features. Once a route is provided, the directions are provided in an easy to use dialog box to the left along with the estimated car travel time:
![image](https://user-images.githubusercontent.com/105998663/235361616-7ee844ce-5b3a-4869-a468-39082060cbe3.png)

Routing utilizes the Dijkstra Algorithm, the industry standard for single source single destination graph problems (also used by Google Maps). This leads to optimal routes with the shortes travel times.

## Other
In addition to the actual graphical program, in the ECE297 course we were also asked to solve the travelling courier problem (extension of travelling salesman problem). Our team utilized mutliple source Dijkstra, Greedy Algorithms, 3-OPT and Simulated Annealing to recieve excellent QoR (Quality of Route) scores from the autotester. The goal was to get as close as possible to the optimal solution. Our scores ranged from 88290.3 (Hard difficulty) to 128671.8 (Extreme difficulty) with an average score of 105995.6.
