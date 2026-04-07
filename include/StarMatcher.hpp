//
// Created by ronan2140 on 4/2/26.
//

#ifndef LOSTINSPACE_STARMATCHER_HPP
#define LOSTINSPACE_STARMATCHER_HPP
#include <Eigen/Core>

#include "CameraSimulator.hpp"
#include "TriangleDatabase.hpp"

struct CamNode {
    Eigen::Vector3f vec;
    float opp_dist;
};

struct SolveResult {
    bool success;
    Eigen::Matrix3f R;
    uint32_t idA;
    uint32_t idB;
    uint32_t idC;
    float certainty;
};

class StarMatcher {
public :
    static Eigen::Vector3f pixelToVector(float u, float v);
    static StarTriangle* findMatch(float d1, float d2, float d3, float epsilon);
    static Eigen::Matrix3f calculateAttitude(const std::vector<Eigen::Vector3f>& v_cams,
    const std::vector<Eigen::Vector3f>& v_refs);
    static float calculateCertainty(const Eigen::Matrix3f& R_calc,
                         const std::vector<Pixel>& photo,
                         uint32_t idA, uint32_t idB, uint32_t idC);

    static SolveResult solvePicture(const std::vector<Pixel> &photo);

    static float calculateCertainty(const Eigen::Matrix3f& R, std::vector<Pixel> photo);
};



#endif //LOSTINSPACE_STARMATCHER_HPP
