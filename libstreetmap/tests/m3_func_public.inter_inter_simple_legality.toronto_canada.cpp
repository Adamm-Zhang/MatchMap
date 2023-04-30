#include <random>
#include <UnitTest++/UnitTest++.h>

#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "m3.h"

#include "unit_test_util.h"
#include "path_verify.h"

using ece297test::relative_error;
using ece297test::path_is_legal;


SUITE(inter_inter_simple_path_func_legality_public) {
struct MapFixture {
    MapFixture() {
        rng = std::minstd_rand(4);
        rand_intersection = std::uniform_int_distribution<IntersectionIdx>(0, getNumIntersections()-1);
        rand_street = std::uniform_int_distribution<StreetIdx>(1, getNumStreets()-1);
        rand_segment = std::uniform_int_distribution<StreetSegmentIdx>(0, getNumStreetSegments()-1);
        rand_poi = std::uniform_int_distribution<POIIdx>(0, getNumPointsOfInterest()-1);
        rand_lat = std::uniform_real_distribution<double>(43.479999542, 43.919982910);
        rand_lon = std::uniform_real_distribution<double>(-79.789985657, -79.000000000);
        rand_turn_penalty = std::uniform_real_distribution<double>(0., 30.);
        rand_walking_speed = std::uniform_real_distribution<double>(0.8, 5);
        rand_walking_time_limit = std::uniform_real_distribution<double>(0.0, 300);
    }

    std::minstd_rand rng;
    std::uniform_int_distribution<IntersectionIdx> rand_intersection;
    std::uniform_int_distribution<StreetSegmentIdx> rand_street;
    std::uniform_int_distribution<StreetSegmentIdx> rand_segment;
    std::uniform_int_distribution<POIIdx> rand_poi;
    std::uniform_real_distribution<double> rand_lat;
    std::uniform_real_distribution<double> rand_lon;
    std::uniform_real_distribution<double> rand_turn_penalty;
    std::uniform_real_distribution<double> rand_walking_speed;
    std::uniform_real_distribution<double> rand_walking_time_limit;
};
    TEST(findPathBetweenIntersections_simple_legality) {
        std::vector<StreetSegmentIdx> path;

        path = findPathBetweenIntersections(std::make_pair(3237, 3238), 0.00000000000000000);
        CHECK(path_is_legal(3237, 3238, path));

        path = findPathBetweenIntersections(std::make_pair(10035, 10034), 0.00000000000000000);
        CHECK(path_is_legal(10035, 10034, path));

        path = findPathBetweenIntersections(std::make_pair(11644, 11645), 0.00000000000000000);
        CHECK(path_is_legal(11644, 11645, path));

        path = findPathBetweenIntersections(std::make_pair(17764, 17760), 0.00000000000000000);
        CHECK(path_is_legal(17764, 17760, path));

        path = findPathBetweenIntersections(std::make_pair(17, 17908), 0.00000000000000000);
        CHECK(path_is_legal(17, 17908, path));

        path = findPathBetweenIntersections(std::make_pair(25311, 25313), 0.00000000000000000);
        CHECK(path_is_legal(25311, 25313, path));

        path = findPathBetweenIntersections(std::make_pair(36625, 37678), 0.00000000000000000);
        CHECK(path_is_legal(36625, 37678, path));

        path = findPathBetweenIntersections(std::make_pair(45454, 3674), 0.00000000000000000);
        CHECK(path_is_legal(45454, 3674, path));

        path = findPathBetweenIntersections(std::make_pair(49360, 49361), 0.00000000000000000);
        CHECK(path_is_legal(49360, 49361, path));

        path = findPathBetweenIntersections(std::make_pair(63940, 63979), 0.00000000000000000);
        CHECK(path_is_legal(63940, 63979, path));

        path = findPathBetweenIntersections(std::make_pair(30935, 66059), 0.00000000000000000);
        CHECK(path_is_legal(30935, 66059, path));

        path = findPathBetweenIntersections(std::make_pair(69630, 26106), 0.00000000000000000);
        CHECK(path_is_legal(69630, 26106, path));

        path = findPathBetweenIntersections(std::make_pair(71474, 71475), 0.00000000000000000);
        CHECK(path_is_legal(71474, 71475, path));

        path = findPathBetweenIntersections(std::make_pair(71854, 71857), 0.00000000000000000);
        CHECK(path_is_legal(71854, 71857, path));

        path = findPathBetweenIntersections(std::make_pair(72634, 72640), 0.00000000000000000);
        CHECK(path_is_legal(72634, 72640, path));

        path = findPathBetweenIntersections(std::make_pair(88305, 88301), 0.00000000000000000);
        CHECK(path_is_legal(88305, 88301, path));

        path = findPathBetweenIntersections(std::make_pair(15925, 15924), 0.00000000000000000);
        CHECK(path_is_legal(15925, 15924, path));

        path = findPathBetweenIntersections(std::make_pair(118968, 118966), 0.00000000000000000);
        CHECK(path_is_legal(118968, 118966, path));

        path = findPathBetweenIntersections(std::make_pair(128077, 128069), 0.00000000000000000);
        CHECK(path_is_legal(128077, 128069, path));

        path = findPathBetweenIntersections(std::make_pair(135706, 135709), 0.00000000000000000);
        CHECK(path_is_legal(135706, 135709, path));

        path = findPathBetweenIntersections(std::make_pair(148411, 148408), 0.00000000000000000);
        CHECK(path_is_legal(148411, 148408, path));

        path = findPathBetweenIntersections(std::make_pair(154631, 154628), 0.00000000000000000);
        CHECK(path_is_legal(154631, 154628, path));

        path = findPathBetweenIntersections(std::make_pair(98468, 156042), 0.00000000000000000);
        CHECK(path_is_legal(98468, 156042, path));

        path = findPathBetweenIntersections(std::make_pair(88679, 88689), 0.00000000000000000);
        CHECK(path_is_legal(88679, 88689, path));

        path = findPathBetweenIntersections(std::make_pair(38054, 133738), 0.00000000000000000);
        CHECK(path_is_legal(38054, 133738, path));

        path = findPathBetweenIntersections(std::make_pair(173042, 114109), 0.00000000000000000);
        CHECK(path_is_legal(173042, 114109, path));

        path = findPathBetweenIntersections(std::make_pair(173873, 66005), 0.00000000000000000);
        CHECK(path_is_legal(173873, 66005, path));

        path = findPathBetweenIntersections(std::make_pair(175897, 88508), 0.00000000000000000);
        CHECK(path_is_legal(175897, 88508, path));

        path = findPathBetweenIntersections(std::make_pair(182800, 182798), 0.00000000000000000);
        CHECK(path_is_legal(182800, 182798, path));

        path = findPathBetweenIntersections(std::make_pair(169478, 190092), 0.00000000000000000);
        CHECK(path_is_legal(169478, 190092, path));

    } //findPathBetweenIntersections_simple_legality

} //inter_inter_simple_path_func_legality_public

