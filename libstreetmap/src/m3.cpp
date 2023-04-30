
#include "globals.h"

/* This function returns the path travel time for a path of street segments, including travel time */
double computePathTravelTime(const std::vector<StreetSegmentIdx>& path, 
                             const double turn_penalty){
    double totalTime = 0;
    int numOfSegs = path.size();
    if (numOfSegs == 0 || numOfSegs == 1){ // no segs or only one seg in the path
        return totalTime;
    }
    auto segItr = path.begin(); 
    auto segItrNext = path.begin() + 1; 
    auto segEnd = path.end();
    
    double turned;
    for(; segItr < segEnd - 1; segItr++, segItrNext++){
        turned = 0;
        if(gSegmentVector[(*segItr)].myStreet != gSegmentVector[(*segItrNext)].myStreet){
            turned = turn_penalty;
        }
        totalTime = totalTime + gSegmentVector[(*segItr)].segTravelTime + turned;   
    }
    totalTime += gSegmentVector[(*segItr)].segTravelTime;
    
    return totalTime;
}

/* This function finds the path between intersections using Dijkstra's Algorithm:
 * 1. Mark all nodes (intersections) as unvisited
 * 2. Mark starting node with travel time of 0, and the rest of the nodes with travel time of infinity
 * 3. Set the starting node as the current node
 * 4. For each adjacent node and edge (street segment), calculate the travel time by adding current travel time of
 * the node to the travel time of the edge.
 * 5. Compare the new travel time to the old stored one. If it is smaller, then replace the stored travel time with
 * the new one
 * 6. When done analyzing each adjacent node and edge, mark the node as visited.
 * 7. Select the next adjacent node to analyze with the smallest distance, set it as current node, and go to step 4.
 * */
std::vector<StreetSegmentIdx> findPathBetweenIntersections(
                  const std::pair<IntersectionIdx, IntersectionIdx> intersect_ids,
                  const double turn_penalty){
    // if the source and destination are the same, then return an empty vector as path
    // setup our nodes vector
    std::vector<Node> NodesVector;
    NodesVector.resize(gIntersectVector.size());
//    if (intersect_ids.first == intersect_ids.second) return result;

    // reset NodesVector for the entire map
    for (int i = 0; i < gIntersectVector.size(); i++){
        NodesVector[i].reachingEdge = 0;
        NodesVector[i].bestTime = infinity;
    }

    // setup our priority queue waveFront that will hold waveElem
    std::priority_queue<waveElem, std::vector<waveElem>, compareTravelTime> waveFront;
    // initialize our waveFront with the source node (note travel time starts off as 0)
    waveFront.push({intersect_ids.first, -1, 0});
    // initialize currentNodeID
    int currentNodeID = -1;

    // loop until the waveFront is empty
    while (!waveFront.empty()){

        waveElem wave = waveFront.top(); // get the next node to check with the minimum travelTime
        waveFront.pop(); // remove the node we are checking to prevent it from being checked again on next iteration

        currentNodeID = wave.nodeID; // get the intersection id of the current node in our wave

        // if the travelTime up until this intersection is less than the bestTime at that intersection, then we choose
        // it to proceed
        if (wave.travelTime < NodesVector[currentNodeID].bestTime){
            // get the reaching edge (edge that joins it) and best time so far
            NodesVector[currentNodeID].reachingEdge = wave.edgeID;
            NodesVector[currentNodeID].bestTime = wave.travelTime;

            // check if we are done finding the path if we have arrived at the destination intersection
            if (currentNodeID == intersect_ids.second){
                break;
            }

            // loop through each connected street segment (outgoing edge) and add the corresponding adjacent
            // wave elements to the wavefront for the next iteration
            for (int i = 0; i < gAdjacentSegments[currentNodeID].size(); i++){
                IntersectionIdx toNodeID = gAdjacentSegments[currentNodeID][i].second;

                // turning logic
                double turned = 0.0; // we have not turned yet
                if (!(NodesVector[currentNodeID].reachingEdge == -1)){
                    if (gSegmentVector[gAdjacentSegments[currentNodeID][i].first].myStreet != gSegmentVector[NodesVector[currentNodeID].reachingEdge].myStreet){
                        turned = turn_penalty; // add a turn penalty if the streets change
                    }
                }
                // push to the wavefront the actual node (intersection) and edge (street segment) waveElem that we care about.
                waveFront.push(waveElem(toNodeID, (gAdjacentSegments[currentNodeID][i].first), (NodesVector[currentNodeID].bestTime + gSegmentVector[gAdjacentSegments[currentNodeID][i].first].segTravelTime + turned)));
            }
        }
    }

    // create a vector result to hold street segments used in the path
    std::vector<StreetSegmentIdx> result;
    StreetSegmentIdx prevEdge = NodesVector[currentNodeID].reachingEdge;

    // backtrack using prevEdge along the path found by Dijkstra
    while (prevEdge != -1){
        result.push_back(prevEdge);
        if (gSegmentVector[prevEdge].to == currentNodeID){
            currentNodeID = gSegmentVector[prevEdge].from;
        }else{
            currentNodeID = gSegmentVector[prevEdge].to;
        }
        if(clearBool){
            gIntersectVector[currentNodeID].intersectHighlight = false;
            gSegmentVector[prevEdge].pathHighlight = false;
        
        }else{
//            gIntersectVector[currentNodeID].intersectHighlight = true;
            gSegmentVector[prevEdge].pathHighlight = true;
        }
        prevEdge = NodesVector[currentNodeID].reachingEdge;
    }

    // reverse the result vector because the path was found using backtracking.
    std::reverse(result.begin(), result.end());

    // de allocate the nodes vector
    std::vector<Node>().swap(NodesVector);

    // return all streetsegments connected to the intersections in visited nodes.
    return result;
}
