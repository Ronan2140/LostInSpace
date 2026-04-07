//
// Created by ronan2140 on 4/2/26.
//

#ifndef LOSTINSPACE_TRIANGLEDATABSE_HPP
#define LOSTINSPACE_TRIANGLEDATABSE_HPP
#include <vector>
#include <Eigen/Core>

#include "StarCatalogue.hpp"

struct StarTriangle {
    uint32_t idA, idB, idC;
    float dist1, dist2, dist3;
};

class TriangleDatabase {
    public :
        static std::vector<StarTriangle> triangles;

    static float calculateAngularDistance(const Star& a, const Star& b);
    static float calculateAngularDistance(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2);

    void generate();
    static StarTriangle makeSortedTriangle(uint32_t i, uint32_t j, uint32_t k, float dij, float djk, float dik);
};





#endif //LOSTINSPACE_TRIANGLEDATABSE_HPP
