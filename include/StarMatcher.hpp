//
// Created by ronan2140 on 4/2/26.
//

#ifndef LOSTINSPACE_STARMATCHER_HPP
#define LOSTINSPACE_STARMATCHER_HPP
#include <Eigen/Core>
#include "TriangleDatabse.hpp"


class StarMatcher {
public :
    static Eigen::Vector3f pixelToVector(float u, float v);
    static StarTriangle* findMatch(float d1, float d2, float d3, float epsilon);
    static Eigen::Matrix3f calculateAttitude(const std::vector<Eigen::Vector3f>& v_cams,
    const std::vector<Eigen::Vector3f>& v_refs);
};



#endif //LOSTINSPACE_STARMATCHER_HPP
