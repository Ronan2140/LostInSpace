//
// Created by ronan2140 on 4/2/26.
//

#include "StarCatalogue.hpp"
#include <iostream>
#include <fstream>
#include <ranges>
#include <charconv>
#include <cmath>
#include <complex>

#include "Config.hpp"
#include "Timer.hpp"


/* index of cols
*  "id","hip","hd","hr","gl","bf","proper","ra","dec","dist","pmra","pmdec","rv","mag","absmag","spect","ci","x","y","z"
*  "vx","vy","vz","rarad","decrad","pmrarad","pmdecrad","bayer","flam","con","comp","comp_primary","base","lum","var",
*  "var_min","var_max","pos_src","dist_src","mag_src","pm_src","rv_src","spect_src" */
std::vector<Star> StarCatalogue::stars;

bool StarCatalogue::loadFromCSV(const std::string& filepath) {
    Timer timer("Load CSV");

    std::cout << "Loading : " << filepath << std::endl;
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) {return false;}
    std::string line;
    std::getline(ifs, line);

    while (std::getline(ifs, line)) {
        auto cols = line | std::views::split(',');
        auto it = cols.begin();
        // get the hip
        std::advance(it, 1);
        auto col_hip = std::string_view(*it);
        uint32_t hip_id = 0;
        if (std::from_chars(col_hip.data(), col_hip.data() + col_hip.size(), hip_id).ec != std::errc()) {
            continue;
        }

        // get magnitude (mag, index 13, so we go from index 1 to 13 -> 12)
        std::advance(it, 12);
        auto col_mag = std::string_view(*it);
        float mag = 0.0f;
        std::from_chars(col_mag.data(), col_mag.data() + col_mag.size(), mag);

        // filter noise
        if (mag > Config::CATALOGUE_MAG_LIMIT) continue;

        // get x y z (index 17 18 19 -> 4 jump then 1 and 1)

        std::advance(it, 4);
        double rx = 0, ry = 0, rz = 0;
        std::from_chars(std::string_view(*it).data(), std::string_view(*it).data() +
            std::string_view(*it).size(), rx);
        ++it;
        std::from_chars(std::string_view(*it).data(), std::string_view(*it).data() +
            std::string_view(*it).size(), ry);
        ++it;
        std::from_chars(std::string_view(*it).data(), std::string_view(*it).data() +
            std::string_view(*it).size(), rz);

        double norm = std::sqrt(rx * rx + ry * ry + rz * rz);
        if (norm > 0) {
            stars.push_back({
                hip_id,
                mag,
                static_cast<float>(rx / norm),
                static_cast<float>(ry / norm),
                static_cast<float>(rz / norm)
            });
        }


    }
    std::cout << "Importation completed" << std::endl;
    return true;
}

size_t StarCatalogue::size() {
    return stars.size();
}

Star StarCatalogue::getStar(const size_t index) {
    return stars[index];
}

std::vector<Star> StarCatalogue::filterBrightStars(const float threshold) {
    std::vector<Star> result;
    for (auto & star : stars) {
        if (star.v_mag < threshold) {
            result.push_back(star);
        }
    }
    return result;
}

const Star* StarCatalogue::findNearestStar(const Eigen::Vector3f& v_pred) {
    const Star* bestStar = nullptr;
    float maxDot = -1.0f;

    // Linear search through the entire catalog
    for (const auto& s : stars) {
        // Dot product between predicted vector and catalog star vector
        float dot = v_pred.x() * s.x + v_pred.y() * s.y + v_pred.z() * s.z;

        if (dot > maxDot) {
            maxDot = dot;
            bestStar = &s;
        }
    }

    return bestStar;
}

const Star* StarCatalogue::getStarById(uint32_t target_id) {

    for (const auto& star : stars) {
        if (star.id == target_id) {
            return &star;
        }
    }
    return nullptr;
}
