/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.cc to edit this template
 */

#include "globals.h"

double getManHattenDistance(ezgl::point2d destination, ezgl::point2d source);

double getManHattenDistance(ezgl::point2d destination, ezgl::point2d source){
    return abs(destination.y - source.y) + abs(destination.x - source.x);
}

bool checkLegal(std::vector<encodedDeliveryNode>);

bool checkLegal(std::vector<encodedDeliveryNode> intersectionList){     // function to check legality of a path.
    std::vector<IntersectionIdx> visitedPickups;

    for(int i = 0; i < intersectionList.size(); i++){
        if(intersectionList[i].isPickup){
            visitedPickups.push_back(intersectionList[i].intersectionID);
        }else{  // hit a drop off, so look for exists pickup
            if(find(visitedPickups.begin(), visitedPickups.end(), intersectionList[i].pickUpID) == visitedPickups.end()) {   // pickup no exists
                return false;
            }
        }
    }
    return true;
}


std::vector<double> multiDijkstra(const IntersectionIdx &source, const std::vector<IntersectionIdx> &intersections,
                                      const double &turn_penalty){

    std::vector<double> times;
    int intersectionCounter = 0;
    times.resize(getNumIntersections());
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
    waveFront.push({source, -1, 0});
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

            // check if we are done finding the path if we have arrived at any of the destination intersections
            for (int i = 0; i < intersections.size(); i++){
                if (currentNodeID == intersections[i]){
                    // create a vector result to hold street segments used in the path
                    times[currentNodeID] = wave.travelTime;
                    intersectionCounter++;
                    if (intersectionCounter == intersections.size()){
                        return times;
                    }
                }
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

    // de allocate the nodes vector
    std::vector<Node>().swap(NodesVector);

    return times;
}

/*
 This routine takes in a vector of D deliveries (pickUp, dropOff
 intersection pairs), another vector of N intersections that
 are legal start and end points for the path (depots), and a turn
 penalty in seconds (see m3.h for details on turn penalties).

 The first vector 'deliveries' gives the delivery information. Each delivery
 in this vector has pickUp and dropOff intersection ids.
 A delivery can only be dropped-off after the associated item has been picked-up.

 The second vector 'depots' gives the intersection ids of courier company
 depots containing trucks; you start at any one of these depots and end at
 any one of the depots.

 This routine returns a vector of CourierSubPath objects that form a delivery route.
 The CourierSubPath is as defined above. The first street segment id in the
 first subpath is connected to a depot intersection, and the last street
 segment id of the last subpath also connects to a depot intersection.
 A package will not be dropped off if you haven't picked it up yet.

 The start_intersection of each subpath in the returned vector should be
 at least one of the following (a pick-up and/or drop-off can only happen at
 the start_intersection of a CourierSubPath object):
 1- A start depot.
 2- A pick-up location
 3- A drop-off location.

 You can assume that D is always at least one and N is always at least one
 (i.e. both input vectors are non-empty).

 It is legal for the same intersection to appear multiple times in the pickUp
 or dropOff list (e.g. you might have two deliveries with a pickUp
 intersection id of #50). The same intersection can also appear as both a
 pickUp location and a dropOff location.

 If you have two pickUps to make at an intersection, traversing the
 intersection once is sufficient to pick up both packages. Additionally,
 one traversal of an intersection is sufficient to drop off all the
 (already picked up) packages that need to be dropped off at that intersection.

 Depots will never appear as pickUp or dropOff locations for deliveries.

 If no valid route to make *all* the deliveries exists, this routine must
 return an empty (size == 0) vector.
*/


std::vector<CourierSubPath> travelingCourier(const std::vector<DeliveryInf>& deliveries,
                                 const std::vector<IntersectionIdx>& depots,
                                 const float turn_penalty){
    
    //declare chrono time check variables
    std::chrono::time_point<std::chrono::high_resolution_clock> pathComputeStart = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> pathComputeEnd;
    std::chrono::duration<double> pathComputeDiff;
    
    //stores the results of all paths from greedy + permutation algorithms
    std::vector<double> threadedTimes;
    threadedTimes.resize(depots.size());
    
    std::vector<std::vector<CourierSubPath>> threadedResults;
    threadedResults.resize(depots.size());
    for(int i = 0; i < threadedResults.size(); i++){
        threadedResults[i].reserve(2*deliveries.size() + 1);
    }

    std::vector<IntersectionIdx> importantIntersections;
    // create an important intersections that stores which intersections we need to hit
    for (DeliveryInf i : deliveries){
        importantIntersections.push_back(i.pickUp);
        importantIntersections.push_back(i.dropOff);
    }
    
    // make a version of importantIntersections that is unique (doesn't contain duplicates) for the timeMatrix
    std::vector<IntersectionIdx> uniqueImportantIntersections(importantIntersections);
    
    for(int threadedDepot = 0; threadedDepot < depots.size(); threadedDepot++ ){
        uniqueImportantIntersections.push_back(depots[threadedDepot]);        
    }
    std::sort(uniqueImportantIntersections.begin(), uniqueImportantIntersections.end());
    uniqueImportantIntersections.erase(std::unique(uniqueImportantIntersections.begin(), uniqueImportantIntersections.end()), uniqueImportantIntersections.end());
    
    // create the time matrix
    std::vector<std::vector<double>> timeMatrix;
    timeMatrix.resize(getNumIntersections());
    #pragma omp parallel for
    for(int i = 0; i < uniqueImportantIntersections.size(); i++){ 
        timeMatrix[uniqueImportantIntersections[i]] = ((multiDijkstra(uniqueImportantIntersections[i], uniqueImportantIntersections, turn_penalty)));
    }
    
    pathComputeEnd = std::chrono::high_resolution_clock::now();
    pathComputeDiff = pathComputeEnd - pathComputeStart;
    //std::cout << "Precomputing Time Matrix Finishes: " << pathComputeDiff.count() << "\n";
        
    std::vector<std::vector<CourierSubPath>> result;
    result.resize(depots.size());
    
    std::vector<std::vector<encodedDeliveryNode>> resultIntersects; // encoded path vector containing only intersection nodes
    resultIntersects.resize(depots.size());
    
    //// GREEDY ALGORITHM ////
    #pragma omp for
    for(int threadedDepot = 0; threadedDepot < depots.size(); threadedDepot++){
        
        // picked up deliveries stored as
        std::vector<DeliveryInf> pickedUpDeliveries;
        std::vector<DeliveryInf> notPickedUpDeliveries(deliveries);
        
        // start at depot[0] for convenience
        IntersectionIdx currentIntersection = depots[threadedDepot];
        IntersectionIdx nextIntersection = -1;

        while (!(importantIntersections.empty())){
            // create a priority queue of the travelTimes from currentIntersections to intersection i
            std::priority_queue<std::pair<double, IntersectionIdx>, std::vector<std::pair<double, IntersectionIdx>>, std::greater<std::pair<double, IntersectionIdx>>> travelTimes;
            for (int i = 0; i < importantIntersections.size(); i++){
//                while(timeMatrix[currentIntersection][importantIntersections[i]]){
//                    std::cout << "running\n";
//                    #pragma omp flush(timeMatrix)
//                }
                //std::cout << "passing\n";
                travelTimes.push(std::make_pair(timeMatrix[currentIntersection][importantIntersections[i]], importantIntersections[i]));
            }

            bool isPickup = false;
            do {
                // get the intersection with the least travel time
                nextIntersection = (travelTimes.top()).second;
                travelTimes.pop();

                // check if it is a pickup
                for (int i = 0; i < notPickedUpDeliveries.size(); i++){
                    if (notPickedUpDeliveries[i].pickUp == nextIntersection){
                        isPickup = true;
                        pickedUpDeliveries.push_back(notPickedUpDeliveries[i]);
                        notPickedUpDeliveries.erase(notPickedUpDeliveries.begin() + i);
                        break;
                    }
                }
            }while (isPickup == false && (std::find_if( //find() ask for a value, use find_if() for condition
                    pickedUpDeliveries.begin(),
                    pickedUpDeliveries.end(),
                    [nextIntersection](const DeliveryInf& element) //you want to compare an item
                    {return element.dropOff == nextIntersection; }) == pickedUpDeliveries.end()));
            // we keep looping as long as pickedUpDeliveries does contain nextIntersersection and it is a dropoff.

            // at this point, we either have a pickup or a valid drop off, so modify the pickedUpDeliveries vector accordingly
            if (!isPickup){
                auto associatedPickup = std::find_if( //find() ask for a value, use find_if() for condition
                        pickedUpDeliveries.begin(),
                        pickedUpDeliveries.end(),
                        [nextIntersection](const DeliveryInf& element) //you want to compare an item
                        {return element.dropOff == nextIntersection; });
                resultIntersects[threadedDepot].emplace_back(encodedDeliveryNode(nextIntersection, associatedPickup->pickUp, false));
                pickedUpDeliveries.erase(associatedPickup);

            }else{
                resultIntersects[threadedDepot].emplace_back(encodedDeliveryNode(nextIntersection, -1, true));
            }

            if (currentIntersection != nextIntersection){
                // now find the path between the currentIntersection and the nextIntersection, and make a courier path
                std::vector<StreetSegmentIdx> path = findPathBetweenIntersections(std::make_pair(currentIntersection, nextIntersection), turn_penalty);
                result[threadedDepot].push_back({currentIntersection, nextIntersection, path});
            }

            // remove nextIntersection from the important intersections because we have visited it
            importantIntersections.erase(std::find(importantIntersections.begin(), importantIntersections.end(), nextIntersection));

            // set the currentIntersection to the nextIntersection for the next iteration
            currentIntersection = nextIntersection;
        }

        // find the final path from the last delivery intersection to the depot        
        std::vector<StreetSegmentIdx> finalPath = findPathBetweenIntersections(std::make_pair(currentIntersection, depots[threadedDepot]), turn_penalty);
        result[threadedDepot].push_back({currentIntersection, depots[threadedDepot], finalPath});
        
        //reinsert the nodes into important intersections
        for (DeliveryInf i : deliveries){
            importantIntersections.push_back(i.pickUp);
            importantIntersections.push_back(i.dropOff);
        }
    }
    pathComputeEnd = std::chrono::high_resolution_clock::now();
    pathComputeDiff = pathComputeEnd - pathComputeStart;
    //std::cout << "Greedy Algorithm Finishes: " << pathComputeDiff.count() << "\n";
    
    #pragma omp parallel for
    for(int threadedDepot = 0; threadedDepot < depots.size(); threadedDepot++){
            
        // have a set of mid intersections here.
        // we'll keep it and run thru every single start depot here:
        double globalOptimalTime = 0;
        for(int i = 0; i < result[threadedDepot].size(); i++){
            globalOptimalTime += computePathTravelTime(result[threadedDepot][i].subpath, turn_penalty);
        }
        threadedTimes[threadedDepot] = globalOptimalTime;
        
        std::vector<CourierSubPath> currentBestPath;
        for(int l = 0; l < result[threadedDepot].size(); l++){
            currentBestPath.push_back(result[threadedDepot][l]);
        }
        
        for(int l = 0; l < result[threadedDepot].size(); l++){
            threadedResults[threadedDepot].push_back(result[threadedDepot][l]);
        }
        
        std::vector<encodedDeliveryNode> bestMidPath;
        for(int l = 0; l < resultIntersects[threadedDepot].size(); l++){
            bestMidPath.push_back(resultIntersects[threadedDepot][l]);
        }
        
        std::vector<encodedDeliveryNode> currentBestIntersectPath = resultIntersects[threadedDepot];   // original encoded intersections
        std::vector<CourierSubPath> resultPathTEST; // THIS IS JUST A TESTER********
        
        ///***** 3 OPT *****///
        for(int h = 2, z = resultIntersects[threadedDepot].size() - 2; h < resultIntersects[threadedDepot].size() - 2 && z > 2; h++, z--){   // 0 to h, h+1 to i, i+1 to end.
            for(int k = h + 1, y = z - 1; k < resultIntersects[threadedDepot].size() - 1 && y > 1; k++, y--) {
                std::vector<encodedDeliveryNode> path1;
                std::vector<encodedDeliveryNode> path2;
                std::vector<encodedDeliveryNode> path3;
                std::vector<encodedDeliveryNode> path4;
                std::vector<encodedDeliveryNode> path5;
                std::vector<encodedDeliveryNode> path6;
                path1.reserve(resultIntersects[threadedDepot].size());
                path2.reserve(resultIntersects[threadedDepot].size());
                path3.reserve(resultIntersects[threadedDepot].size());
                path4.reserve(resultIntersects[threadedDepot].size());
                path5.reserve(resultIntersects[threadedDepot].size());
                path6.reserve(resultIntersects[threadedDepot].size());


    //            #pragma omp parallel for
                for (int a = 0; a < h; a++) {
                    path1.push_back(resultIntersects[threadedDepot][a]);
                }

    //            #pragma omp parallel for
                for (int b = h; b < k; b++) {
                    path2.push_back(resultIntersects[threadedDepot][b]);
                }

    //            #pragma omp parallel for
                for (int c = k; c < resultIntersects[threadedDepot].size(); c++) {
                    path3.push_back(resultIntersects[threadedDepot][c]);
                }

    //            #pragma omp parallel for
                for (int d = resultIntersects[threadedDepot].size() - 1; d > z; d--) {
                    path4.push_back(resultIntersects[threadedDepot][d]);
                }

    //            #pragma omp parallel for
                for (int e = z; e > y; e--) {
                    path5.push_back(resultIntersects[threadedDepot][e]);
                }

    //            #pragma omp parallel for
                for (int f = y; f >= 0; f--) {
                    path6.push_back(resultIntersects[threadedDepot][f]);
                }

                std::vector<encodedDeliveryNode> newPath;
                std::vector<encodedDeliveryNode> reversePath;
                // have all path segments, need to connect them now:
                newPath = path1;
                newPath.insert(newPath.end()-1, path3.begin(), path3.end());
                newPath.insert(newPath.end()-1, path2.begin(), path2.end());

                reversePath = path4;
                reversePath.insert(reversePath.end()-1, path6.begin(), path6.end());
                reversePath.insert(reversePath.end()-1, path5.begin(), path5.end());
                
                if(checkLegal(newPath)){
                    
                    
                    double currentTime = 0;
                    for (int i = 0; i < newPath.size() - 1; i++){
                        currentTime += timeMatrix[newPath[i].intersectionID][newPath[i+1].intersectionID];
                    }
                    
                    if(currentTime < globalOptimalTime){
                        
                        std::vector<CourierSubPath>().swap(currentBestPath);
                        std::vector<CourierSubPath>().swap(threadedResults[threadedDepot]);
                        //std::vector<encodedDeliveryNode>().swap(bestMidPath);
                        std::vector<encodedDeliveryNode>().swap(resultIntersects[threadedDepot]);
                        //bestMidPath = newPath;
                        resultIntersects[threadedDepot] = newPath;
                        
                        //make new best path
                        resultPathTEST.clear();
                        
                        //get the best starting depot
                        IntersectionIdx bestDepot = depots[threadedDepot];
                        double endTime = infinity;
                        double bestTime = infinity;
                        for(int i = 0; i < depots.size(); i++){
                            endTime = timeMatrix[newPath[0].intersectionID][depots[i]];
                            if(endTime < bestTime) {
                                bestDepot = depots[i];
                                bestTime = endTime;
                            }
                        }
                        
                        resultPathTEST.push_back({bestDepot, newPath[0].intersectionID,
                                                  findPathBetweenIntersections(std::make_pair(bestDepot, newPath[0].intersectionID), turn_penalty)});

                        for(int i = 0; i < newPath.size() - 1; i++){   // actually create the mid path
                            resultPathTEST.push_back({newPath[i].intersectionID, newPath[i + 1].intersectionID,
                                                      findPathBetweenIntersections(std::make_pair(newPath[i].intersectionID, newPath[i+1].intersectionID), turn_penalty)});
                        }
                        
                        //get the best end depot                          
                        bestDepot = depots[threadedDepot];
                        endTime = infinity;
                        bestTime = infinity;
                        for(int i = 0; i < depots.size(); i++){
                            endTime = timeMatrix[newPath[newPath.size() - 1].intersectionID][depots[i]];
                            if(endTime < bestTime) {
                                bestDepot = depots[i];
                                bestTime = endTime;
                            }
                        }
                        resultPathTEST.push_back({newPath[newPath.size() - 1].intersectionID, bestDepot,
                                                  findPathBetweenIntersections(std::make_pair(newPath[newPath.size() - 1].intersectionID, bestDepot), turn_penalty)});

                        //currentBestPath.clear();
                        globalOptimalTime = currentTime;
                        for(int l = 0; l < resultPathTEST.size(); l++){
                            currentBestPath.push_back(resultPathTEST[l]);
                            threadedResults[threadedDepot].push_back(resultPathTEST[l]);
                        }
                        threadedTimes[threadedDepot] = currentTime;
                        //std::cout << "forward current Time: " << currentTime << std::endl;
                    }
                }
                    
                if(checkLegal(reversePath)){
                    
                    double currentTime = 0;
                    for (int i = 0; i < reversePath.size() - 1; i++){
                        currentTime += timeMatrix[reversePath[i].intersectionID][reversePath[i+1].intersectionID];
                    }
       
                    if(currentTime < globalOptimalTime){
                        std::vector<CourierSubPath>().swap(currentBestPath);
                        std::vector<CourierSubPath>().swap(threadedResults[threadedDepot]);
                        //std::vector<encodedDeliveryNode>().swap(bestMidPath);
                        std::vector<encodedDeliveryNode>().swap(resultIntersects[threadedDepot]);
                        //bestMidPath = newPath;
                        resultIntersects[threadedDepot] = newPath;
                        
                        resultPathTEST.clear();
                        //get the best starting depot
                        IntersectionIdx bestDepot = depots[threadedDepot];
                        double endTime = infinity;
                        double bestTime = infinity;
                        for(int i = 0; i < depots.size(); i++){
                            endTime = timeMatrix[reversePath[0].intersectionID][depots[i]];
                            if(endTime < bestTime) {
                                bestDepot = depots[i];
                                bestTime = endTime;
                            }
                        }
                        
                        resultPathTEST.push_back({bestDepot, reversePath[0].intersectionID,
                                                  findPathBetweenIntersections(std::make_pair(bestDepot, reversePath[0].intersectionID), turn_penalty)});

                        for(int i = 0; i < reversePath.size() - 1; i++){   // actually create the mid path
                            resultPathTEST.push_back({reversePath[i].intersectionID, reversePath[i + 1].intersectionID,
                                                      findPathBetweenIntersections(std::make_pair(reversePath[i].intersectionID, reversePath[i+1].intersectionID), turn_penalty)});
                        }
                        
                        //get the best end depot                          
                        bestDepot = depots[threadedDepot];
                        endTime = infinity;
                        bestTime = infinity;
                        for(int i = 0; i < depots.size(); i++){
                            endTime = timeMatrix[reversePath[reversePath.size() - 1].intersectionID][depots[i]];
                            if(endTime < bestTime) {
                                bestDepot = depots[i];
                                bestTime = endTime;
                            }
                        }
                        resultPathTEST.push_back({reversePath[reversePath.size() - 1].intersectionID, bestDepot,
                                                  findPathBetweenIntersections(std::make_pair(reversePath[reversePath.size() - 1].intersectionID, bestDepot), turn_penalty)});


                        //currentBestPath.clear();
                        globalOptimalTime = currentTime;
                        for(int l = 0; l < resultPathTEST.size(); l++){
                            currentBestPath.push_back(resultPathTEST[l]);
                            threadedResults[threadedDepot].push_back(resultPathTEST[l]);
                        }
                        threadedTimes[threadedDepot] = currentTime;
                        //std::cout << "reverse current Time: " << currentTime << std::endl;
                    }
                }
            }        
        }
        
        //// SIMULATED ANNEALING ////
        //global
        //current best
        //bestmid path
        //incorrectly defined
//        std::vector<encodedDeliveryNode> bestMidPath;
//        for(int l = 0; l < resultIntersects.size(); l++){
//            bestMidPath.push_back(resultIntersects[l]);
//        }
        // loop through all intersections in resultIntersects
        while(pathComputeDiff.count() < 47){
            for (int i = 1; i < resultIntersects[threadedDepot].size() - 1; i++){
                resultIntersects[threadedDepot] = bestMidPath;
                // get the node to move
                encodedDeliveryNode movedNode = resultIntersects[threadedDepot][i];

                for (int j = i+1; j < resultIntersects[threadedDepot].size() - 2; j++){
                    resultIntersects[threadedDepot].erase(std::find_if(resultIntersects[threadedDepot].begin(), resultIntersects[threadedDepot].end(), [movedNode](const encodedDeliveryNode& rhs){
                        return movedNode.intersectionID == rhs.intersectionID && movedNode.pickUpID == rhs.pickUpID && movedNode.isPickup == rhs.isPickup;
                    }));
                    resultIntersects[threadedDepot].insert(resultIntersects[threadedDepot].begin()+j, movedNode);

                    if (!movedNode.isPickup){ // if movedNode is a dropOff then find the associated pickup
                        auto pickup = std::find_if(resultIntersects[threadedDepot].begin(), resultIntersects[threadedDepot].end(), [movedNode](const encodedDeliveryNode& rhs){
                            return rhs.isPickup && rhs.intersectionID == movedNode.pickUpID;
                        });
                        // compare the location of the pickup
                        if (pickup > resultIntersects[threadedDepot].begin()+j) {
                            encodedDeliveryNode legalizedPickup = *pickup;
                            resultIntersects[threadedDepot].erase(pickup);
                            resultIntersects[threadedDepot].insert(std::find_if(resultIntersects[threadedDepot].begin(), resultIntersects[threadedDepot].end(), [movedNode](const encodedDeliveryNode& rhs){
                                        return movedNode.intersectionID == rhs.intersectionID && movedNode.pickUpID == rhs.pickUpID && movedNode.isPickup == rhs.isPickup;
                                    })-1, legalizedPickup);
                        }
                    }else{ // else movedNode is a pickup, the find the associated dropoff
                        auto dropoff = std::find_if(resultIntersects[threadedDepot].begin(), resultIntersects[threadedDepot].end(), [movedNode](const encodedDeliveryNode& rhs){
                            return !(rhs.isPickup) && rhs.pickUpID == movedNode.intersectionID;
                        });
                        if (dropoff < resultIntersects[threadedDepot].begin()+j) {
                            encodedDeliveryNode legalizedDropoff = *dropoff;
                            resultIntersects[threadedDepot].erase(dropoff);
                            resultIntersects[threadedDepot].insert(std::find_if(resultIntersects[threadedDepot].begin(), resultIntersects[threadedDepot].end(), [movedNode](const encodedDeliveryNode& rhs){
                                return movedNode.intersectionID == rhs.intersectionID && movedNode.pickUpID == rhs.pickUpID && movedNode.isPickup == rhs.isPickup;
                            })+1, legalizedDropoff);
                        }
                    }

                    //if (checkLegal(resultIntersects[threadedDepot])){
                        double currentTime = 0;
                        for(int n = 0; n < resultIntersects[threadedDepot].size() - 1; n++){
                            currentTime += timeMatrix[resultIntersects[threadedDepot][n].intersectionID][resultIntersects[threadedDepot][n+1].intersectionID];
                        }

                        if (currentTime < globalOptimalTime){
                            globalOptimalTime = currentTime;

                            bestMidPath.clear();
                            bestMidPath = resultIntersects[threadedDepot];
                        }
                    //}
                }
            }
            pathComputeEnd = std::chrono::high_resolution_clock::now();
            pathComputeDiff = pathComputeEnd - pathComputeStart;
            //std::cout << "time check " << pathComputeDiff.count() << "\n";
        }

        IntersectionIdx startPoint = currentBestPath[0].start_intersection;
        IntersectionIdx endPoint = currentBestPath[currentBestPath.size()-1].end_intersection;
        currentBestPath.clear();

        // find path from the first startPoint to the fist resultIntersection
        currentBestPath.push_back({startPoint, resultIntersects[threadedDepot][0].intersectionID, findPathBetweenIntersections(
                std::make_pair(startPoint, resultIntersects[threadedDepot][0].intersectionID),
                turn_penalty)});

        for (int i = 0; i < resultIntersects[threadedDepot].size() - 1; i++){
            currentBestPath.push_back({resultIntersects[threadedDepot][i].intersectionID, resultIntersects[threadedDepot][i+1].intersectionID, findPathBetweenIntersections(
                    std::make_pair(resultIntersects[threadedDepot][i].intersectionID, resultIntersects[threadedDepot][i+1].intersectionID),
                    turn_penalty)});
        }

        currentBestPath.push_back({resultIntersects[threadedDepot][resultIntersects[threadedDepot].size() - 1].intersectionID, endPoint, findPathBetweenIntersections(
                std::make_pair(resultIntersects[threadedDepot][resultIntersects[threadedDepot].size() - 1].intersectionID, endPoint),
                turn_penalty)});
        
                
        // ASSIGN threadedResults HERE!
        std::vector<CourierSubPath>().swap(threadedResults[threadedDepot]);
        for(int l = 0; l < currentBestPath.size(); l++){
            threadedResults[threadedDepot].push_back(currentBestPath[l]);
        }
        threadedTimes[threadedDepot] = globalOptimalTime;    
    }
    pathComputeEnd = std::chrono::high_resolution_clock::now();
    pathComputeDiff = pathComputeEnd - pathComputeStart;
    //std::cout << "Permutations Finish: " << pathComputeDiff.count() << "\n";

    //find the path that produced the best time
    double bestTime = *(std::min_element(threadedTimes.begin(), threadedTimes.end()));
    for(int i = 0; i < threadedTimes.size(); i++){
        if(threadedTimes[i] == bestTime){
            pathComputeEnd = std::chrono::high_resolution_clock::now();
            pathComputeDiff = pathComputeEnd - pathComputeStart;
            //std::cout << "Returned Best Path: " << pathComputeDiff.count() << "\n";
            
//            std::cout << "end of test\n\n";
            return threadedResults[i];
        }
//        std::cout << i << " | " << threadedTimes[i] << "\n";
    }
    return threadedResults[0]; //fail-safe
}
