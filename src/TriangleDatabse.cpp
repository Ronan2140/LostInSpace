//
// Created by ronan2140 on 4/2/26.
//

#include "../include/TriangleDatabse.hpp"

#include <algorithm>
#include <math.h>
#include <Eigen/Core>

std::vector<StarTriangle> TriangleDatabase::triangles;

void TriangleDatabase::generate() {
    auto brightStars = StarCatalogue::filterBrightStars(5.0f);

    for (size_t i = 0; i < brightStars.size(); i++) {

        for (size_t j = i+1; j < brightStars.size(); j++){
            const float dij = calculateAngularDistance(brightStars[i], brightStars[j]);
            // check if seeable with focal
            if (dij > 20.f) {
                continue;
            }
            for (size_t k = j+1; k < brightStars.size(); k++) {
                const float dki = calculateAngularDistance(brightStars[i], brightStars[k]);
                const float djk = calculateAngularDistance(brightStars[j], brightStars[k]);
                if (dki < 20.f && djk < 20.0f) {
                    triangles.push_back(makeSortedTriangle(
                                        brightStars[i].id,
                                        brightStars[j].id,
                                        brightStars[k].id,
                                        dij, djk, dki
                                    ));                }
            }
        }
    }
    std::ranges::sort(TriangleDatabase::triangles,
                     [](const StarTriangle& a, const StarTriangle& b) {return a.dist1 < b.dist1;});

};


float TriangleDatabase::calculateAngularDistance(const Star &a, const Star &b){
    float dot = a.x * b.x + a.y * b.y + a.z * b.z;

    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;

    return std::acos(dot) * 180.0f / M_PI;
};

float TriangleDatabase::calculateAngularDistance(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2) {
    float dot = v1.dot(v2);

    if (dot > 1.0f) dot = 1.0f;
    if (dot < -1.0f) dot = -1.0f;

    return std::acos(dot) * 180.0f / M_PI;
}
StarTriangle TriangleDatabase::makeSortedTriangle(uint32_t i, uint32_t j, uint32_t k, float dij, float djk, float dki) {

    struct Node { uint32_t id; float opp_dist; };
    Node nodes[3] = {
        {i, djk},
        {j, dki},
        {k, dij}
    };

    if (nodes[0].opp_dist > nodes[1].opp_dist) std::swap(nodes[0], nodes[1]);
    if (nodes[1].opp_dist > nodes[2].opp_dist) std::swap(nodes[1], nodes[2]);
    if (nodes[0].opp_dist > nodes[1].opp_dist) std::swap(nodes[0], nodes[1]);

    StarTriangle t{};
    t.idA = nodes[0].id;
    t.idB = nodes[1].id;
    t.idC = nodes[2].id;

    t.dist1 = nodes[0].opp_dist;
    t.dist2 = nodes[1].opp_dist;
    t.dist3 = nodes[2].opp_dist;

    return t;
}