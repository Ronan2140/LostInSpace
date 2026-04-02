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
};


class CameraSimulator {
public :
    static constexpr int RESOLUTION = 1024;
    static constexpr float FOV_DEG = 20.0f;
    static constexpr float CENTER = RESOLUTION / 2.0f;
    // Focal pixels : f = (W/2) / tan(FOV/2)
    // tan(10°) approx = 0.176326.
    static constexpr float FOCAL_LENGTH = CENTER / 0.17632698f;


    static Eigen::Matrix3f generateRandomAttitude();
    static std::vector<Pixel> takePicture(const Eigen::Matrix3f& R);
    static void saveImagePPM(const std::string& filename, const std::vector<Pixel>& detectedStars);



};



#endif //LOSTINSPACE_CAMERASIMULATOR_HPP
