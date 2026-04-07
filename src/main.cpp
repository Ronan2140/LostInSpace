#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <format>

#include "../include/StarCatalogue.hpp"
#include "../include/CameraSimulator.hpp"
#include "StarMatcher.hpp"
#include "TriangleDatabse.hpp"

std::string getTimestampFilename() {
    auto now = std::chrono::system_clock::now();
    auto zoned = std::chrono::zoned_time{std::chrono::current_zone(), now};
    return std::format("../images/capture_{:%Y-%m-%d_%H-%M-%S}.ppm", zoned);
}

struct CamNode {
    Eigen::Vector3f vec;
    float opp_dist;
};

int main() {
    StarCatalogue::loadFromCSV("../data/stars.csv");
    std::cout << "Stars db imported " << StarCatalogue::size() << std::endl;

    TriangleDatabase db;
    std::cout << "Triangles generation" << std::endl;
    db.generate();
    std::cout << "Triangle generated : " << TriangleDatabase::triangles.size() << " triangles." << std::endl;

    Eigen::Matrix3f R_vraie = CameraSimulator::generateRandomAttitude();
    auto photo = CameraSimulator::takePicture(R_vraie);
    CameraSimulator::saveImagePPM(getTimestampFilename(), photo);

    if (photo.size() < 3) {
        std::cout << "Not enough stars (min 3) !" << std::endl;
        return 0;
    }

    std::cout << "Looking for a valid triangle" << std::endl;

    bool attitude_found = false;
    int max_stars = std::min((int)photo.size(), 15); // On teste les 15 premières étoiles max

    for (int i = 0; i < max_stars && !attitude_found; ++i) {
        for (int j = i + 1; j < max_stars && !attitude_found; ++j) {
            for (int k = j + 1; k < max_stars && !attitude_found; ++k) {

                Eigen::Vector3f v1 = StarMatcher::pixelToVector(photo[i].u, photo[i].v);
                Eigen::Vector3f v2 = StarMatcher::pixelToVector(photo[j].u, photo[j].v);
                Eigen::Vector3f v3 = StarMatcher::pixelToVector(photo[k].u, photo[k].v);

                float opp_v1 = TriangleDatabase::calculateAngularDistance(v2, v3);
                float opp_v2 = TriangleDatabase::calculateAngularDistance(v1, v3);
                float opp_v3 = TriangleDatabase::calculateAngularDistance(v1, v2);

                CamNode cam_nodes[3] = { {v1, opp_v1}, {v2, opp_v2},
                    {v3, opp_v3} };

                std::sort(cam_nodes, cam_nodes + 3, [](const CamNode& a, const CamNode& b) {
                    return a.opp_dist < b.opp_dist;
                });

                float d1 = cam_nodes[0].opp_dist;
                float d2 = cam_nodes[1].opp_dist;
                float d3 = cam_nodes[2].opp_dist;

                StarTriangle* match = StarMatcher::findMatch(d1, d2, d3, 0.08f);

                if (match) {
                    std::cout << ">>> Triangle found with pixel " << i << ", " << j << ", " << k << " ! <<<" << std::endl;

                    std::vector<Eigen::Vector3f> v_cams = {
                        cam_nodes[0].vec,
                        cam_nodes[1].vec,
                        cam_nodes[2].vec
                    };

                    const Star* starA = StarCatalogue::getStarById(match->idA);
                    const Star* starB = StarCatalogue::getStarById(match->idB);
                    const Star* starC = StarCatalogue::getStarById(match->idC);

                    std::vector<Eigen::Vector3f> v_refs = {
                        Eigen::Vector3f(starA->x, starA->y, starA->z),
                        Eigen::Vector3f(starB->x, starB->y, starB->z),
                        Eigen::Vector3f(starC->x, starC->y, starC->z)
                    };

                    Eigen::Matrix3f R_calculee = StarMatcher::calculateAttitude(v_cams, v_refs);

                    std::cout << "\n=== Rotation Matrice Random (Goal) ===" << std::endl;
                    std::cout << R_vraie << std::endl;

                    std::cout << "\n=== Rotation Matrice Found (Star Tracker) ===" << std::endl;
                    std::cout << R_calculee << std::endl;
                    const float certainity = StarMatcher::calculateCertainty(R_calculee, photo, match->idA, match->idB, match->idC);
                    std::cout << "Indice de confiance : " << certainity << "%" << std::endl;
                    attitude_found = true;
                }
            }
        }
    }

    if (!attitude_found) {
        std::cout << "Failure : No combination found" << std::endl;
    }

    return 0;
}