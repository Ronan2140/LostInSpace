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

float StarMatcher::calculateCertainty(const Eigen::Matrix3f &R_calc, const std::vector<Pixel> &photo, const uint32_t idA,
    const uint32_t idB, const uint32_t idC) {
    if (photo.size() < 3) return 0.0f;
    if (photo.size() == 3) return 33.3f;

    int valid_matches = 0;
    int extra_stars_tested = 0;
    constexpr float tolerance_deg = 0.05f;
    const auto cos_threshold = static_cast<float>(std::cos(tolerance_deg * M_PI / 180.0f));

    for (const auto& p : photo) {
        if (p.star_id == idA || p.star_id == idB || p.star_id == idC) continue;

        extra_stars_tested++;

        // pixel in cam
        Eigen::Vector3f v_cam = StarMatcher::pixelToVector(p.u, p.v);

        // universe -> camera
        Eigen::Vector3f v_world_pred = R_calc.transpose() * v_cam;

        // real position in catalog
        const Star* s = StarCatalogue::getStarById(p.star_id);
        if (!s) continue;

        Eigen::Vector3f v_world_real(s->x, s->y, s->z);

        // check if aligned
        float alignment = v_world_pred.dot(v_world_real);

        if (alignment > cos_threshold) {
            valid_matches++;
        }

        // stop after 5 (consider 10%)
        if (extra_stars_tested >= 5) break;
    }

    // + 13.4% per confirmed stars
    const float score = 33.3f + (static_cast<float>(valid_matches) * 13.4f);

    return std::min(100.0f, score);
}

