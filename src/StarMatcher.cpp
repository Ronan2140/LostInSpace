//
// Created by ronan2140 on 4/2/26.
//

#include <cmath>

#include "../include/StarMatcher.hpp"

#include <iostream>

#include "CameraSimulator.hpp"
#include "Config.hpp"
#include "Timer.hpp"


Eigen::Vector3f StarMatcher::pixelToVector(const float u,const float v) {
    const float x = (u - Config::CENTER) / Config::FOCAL_LENGTH;
    const float y = (v - Config::CENTER) / Config::FOCAL_LENGTH;

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

float StarMatcher::calculateCertainty(const Eigen::Matrix3f& R, std::vector<Pixel> photo) {
    if (photo.size() < 3) return 0.0f;

    int validMatches = 0;
    const float toleranceDeg = 0.05f;
    const float cosThreshold = std::cos(toleranceDeg * M_PI / 180.0f);

    for (const auto& p : photo) {
        Eigen::Vector3f v_cam = StarMatcher::pixelToVector(p.u, p.v);

        Eigen::Vector3f v_world_pred = R.transpose() * v_cam;

        const Star* s = StarCatalogue::findNearestStar(v_world_pred);

        if (s) {
            Eigen::Vector3f v_world_real(s->x, s->y, s->z);
            float alignment = v_world_pred.dot(v_world_real);

            if (alignment > cosThreshold) {
                validMatches++;
            }
        }
    }

    int extraMatches = std::max(0, validMatches - 3);
    float score = 33.3f + (extraMatches * (66.7f / Config::STARS_CERTAINTY));

    return std::min(100.0f, score);
}

SolveResult StarMatcher::solvePicture(const std::vector<Pixel> &photo) {
    Timer timer("Finding Solution");

    SolveResult bestResult = {false, Eigen::Matrix3f::Identity(), 0, 0, 0};
    float maxConfidence = -1.0f;

    std::vector<Pixel> sorted_photo = photo;
    std::sort(sorted_photo.begin(), sorted_photo.end(), [](const Pixel& a, const Pixel& b) {
        return a.v_mag < b.v_mag;
    });

    int max_stars = std::min((int)sorted_photo.size(), Config::MAX_STARS_TESTED);

    for (int i = 0; i < max_stars; ++i) {
        for (int j = i + 1; j < max_stars; ++j) {
            for (int k = j + 1; k < max_stars; ++k) {

                Eigen::Vector3f v1 = pixelToVector(sorted_photo[i].u, sorted_photo[i].v);
                Eigen::Vector3f v2 = pixelToVector(sorted_photo[j].u, sorted_photo[j].v);
                Eigen::Vector3f v3 = pixelToVector(sorted_photo[k].u, sorted_photo[k].v);

                float opp_v1 = TriangleDatabase::calculateAngularDistance(v2, v3);
                float opp_v2 = TriangleDatabase::calculateAngularDistance(v1, v3);
                float opp_v3 = TriangleDatabase::calculateAngularDistance(v1, v2);

                CamNode cam_nodes[3] = { {v1, opp_v1}, {v2, opp_v2}, {v3, opp_v3} };
                std::sort(cam_nodes, cam_nodes + 3, [](const CamNode& a, const CamNode& b) {
                    return a.opp_dist < b.opp_dist;
                });

                float d1 = cam_nodes[0].opp_dist;
                float d2 = cam_nodes[1].opp_dist;
                float d3 = cam_nodes[2].opp_dist;

                // Find potential match in database
                if (const StarTriangle* match = findMatch(d1, d2, d3, Config::MATCH_TOLERANCE_DEG)) {

                    const std::vector<Eigen::Vector3f> v_cams = {
                        cam_nodes[0].vec, cam_nodes[1].vec, cam_nodes[2].vec
                    };

                    const Star* starA = StarCatalogue::getStarById(match->idA);
                    const Star* starB = StarCatalogue::getStarById(match->idB);
                    const Star* starC = StarCatalogue::getStarById(match->idC);

                    const std::vector<Eigen::Vector3f> v_refs = {
                        Eigen::Vector3f(starA->x, starA->y, starA->z),
                        Eigen::Vector3f(starB->x, starB->y, starB->z),
                        Eigen::Vector3f(starC->x, starC->y, starC->z)
                    };

                    Eigen::Matrix3f R_temp = calculateAttitude(v_cams, v_refs);

                    float currentConfidence = calculateCertainty(R_temp, sorted_photo);
                    std::cout << "Max : " << maxConfidence << " Found " << currentConfidence << std::endl ;
                    if (currentConfidence > maxConfidence) {
                        maxConfidence = currentConfidence;
                        bestResult = {true, R_temp, match->idA, match->idB, match->idC, maxConfidence};

                        // Optimization: if we are 100% sure, we can stop early
                        if (maxConfidence >= 99.9f) return bestResult;
                    }
                }
            }
        }
    }
    return bestResult;
}