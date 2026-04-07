//
// Created by ronan2140 on 4/2/26.
//

#ifndef LOSTINSPACE_CAMERASIMULATOR_HPP
#define LOSTINSPACE_CAMERASIMULATOR_HPP

#include <Eigen/Dense>
#include "StarCatalogue.hpp"


struct Pixel {
    float u;
    float v;
    uint32_t star_id;
    float v_mag;
};


class CameraSimulator {
public :

    static Eigen::Matrix3f generateRandomAttitude();
    static std::vector<Pixel> takePicture(const Eigen::Matrix3f& R);
    static void saveImagePPM(const std::string& filename, const std::vector<Pixel>& detectedStars);
};



#endif //LOSTINSPACE_CAMERASIMULATOR_HPP
