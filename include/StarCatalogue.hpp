//
// Created by ronan2140 on 4/2/26.
//

#ifndef LOSTINSPACE_STARCATALOGUE_HPP
#define LOSTINSPACE_STARCATALOGUE_HPP
#include <cstdint>
#include <vector>
#include <string>
#include <Eigen/Core>

// Data structure for the Stars
struct Star {
    uint32_t id{};
    float v_mag{};
    float x{}, y{}, z{};
};

class StarCatalogue {
private:
    static std::vector<Star> stars;
public:
    static bool loadFromCSV(const std::string& filepath);
    static size_t size() ;
    static Star getStar(size_t index);
    static const std::vector<Star>& getStars() { return stars; }
    static const Star* getStarById(uint32_t id);

    static std::vector<Star> filterBrightStars(float threshold);
    static const Star* findNearestStar(const Eigen::Vector3f& v_pred);
};

#endif //LOSTINSPACE_STARCATALOGUE_HPP
