//
// Created by ronan2140 on 4/2/26.
//

#include <cmath>

#include "../include/StarMatcher.hpp"

#include <iostream>

#include "CameraSimulator.hpp"


Eigen::Vector3f StarMatcher::pixelToVector(const float u,const float v) {
    const float x = (u - 512.0f) / CameraSimulator::FOCAL_LENGTH;
    const float y = (v - 512.0f) / CameraSimulator::FOCAL_LENGTH;

    Eigen::Vector3f vec(x, y, 1.0f);
    return vec.normalized();
};
StarTriangle* StarMatcher::findMatch(float d1, float d2, float d3, float epsilon) {

    auto it = std::lower_bound(TriangleDatabase::triangles.begin(), TriangleDatabase::triangles.end(),
        d1 - epsilon, [](const StarTriangle& a, float val) { return a.dist1 < val; });

    StarTriangle* best_match = nullptr;
    float min_error = 9999.0f;

    for (; it != TriangleDatabase::triangles.end() && it->dist1 < d1 + epsilon; ++it) {
        if (std::abs(it->dist2 - d2) < epsilon && std::abs(it->dist3 - d3) < epsilon) {

            float error = std::abs(it->dist1 - d1) +
                          std::abs(it->dist2 - d2) +
                          std::abs(it->dist3 - d3);

            if (error < min_error) {
                min_error = error;
                best_match = &(*it);
            }
        }
    }

    return best_match;
}

Eigen::Matrix3f StarMatcher::calculateAttitude(const std::vector<Eigen::Vector3f>& v_cams,
const std::vector<Eigen::Vector3f>& v_refs) {
    Eigen::Matrix3f B = Eigen::Matrix3f::Zero();

    for (size_t i = 0; i < v_cams.size(); ++i) {
        B += v_cams[i] * v_refs[i].transpose();
    }

    const Eigen::JacobiSVD svd(B, Eigen::ComputeFullU | Eigen::ComputeFullV);
    const Eigen::Matrix3f U = svd.matrixU();
    const Eigen::Matrix3f V = svd.matrixV();

    Eigen::Matrix3f R = U * V.transpose();

    if (R.determinant() < 0) {
        Eigen::Matrix3f M = Eigen::Matrix3f::Identity();
        M(2, 2) = -1;
        R = U * M * V.transpose();
    }

    return R;
}

