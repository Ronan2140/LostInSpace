//
// Created by ronan2140 on 4/2/26.
//

#include "../include/CameraSimulator.hpp"

#include <ctime>
#include <fstream>



Eigen::Matrix3f CameraSimulator::generateRandomAttitude() {
        // Eigen generates a random quaternion
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        Eigen::Quaternionf q = Eigen::Quaternionf::UnitRandom();

        // convert it to 3x3
        return q.toRotationMatrix();
    }

std::vector<Pixel> CameraSimulator::takePicture(const Eigen::Matrix3f& R) {
        std::vector<Pixel> image;
        image.reserve(60);
        auto brightStars = StarCatalogue::filterBrightStars(5.0f);
        for (const auto& star : brightStars) {
            Eigen::Vector3f starPosition(star.x, star.y, star.z);

            Eigen::Vector3f V_cam = R * starPosition;

            // is the star in front of me
            if (V_cam.z() > 0) {

                const float u = (FOCAL_LENGTH * (V_cam.x() / V_cam.z())) + 512.0f;
                const float v = (FOCAL_LENGTH * (V_cam.y() / V_cam.z())) + 512.0f;

                if (u >= 0 && u < RESOLUTION && v >= 0 && v < RESOLUTION) {
                    image.push_back({u, v, star.id});
                }
            }

        }
    return image;
    }


void CameraSimulator::saveImagePPM(const std::string& filename, const std::vector<Pixel>& detectedStars) {
    std::ofstream file(filename);

    file << "P3\n" << 1024 << " " << 1024 << "\n255\n";

    std::vector<int> grid(1024 * 1024, 0);

    for (const auto& p : detectedStars) {
        int u = static_cast<int>(p.u);
        int v = static_cast<int>(p.v);

        if (u >= 0 && u < 1024 && v >= 0 && v < 1024) {
            grid[v * 1024 + u] = 255;

        }
    }

    for (int i = 0; i < 1024 * 1024; ++i) {
        file << grid[i] << " " << grid[i] << " " << grid[i] << " ";
        if (i % 1024 == 0) file << "\n";  }
}
