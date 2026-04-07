//
// Created by ronan2140 on 4/7/26.
//

#ifndef LOSTINSPACE_STARTRACKERUI_HPP
#define LOSTINSPACE_STARTRACKERUI_HPP
#include <future>
#include <Eigen/Core>
#include <SFML/Graphics.hpp>
#include <vector>
#include "CameraSimulator.hpp"
#include "StarMatcher.hpp"


struct GeoPos { float lat; float lon; };


class StarTrackerUI {

public:
    StarTrackerUI();
    ~StarTrackerUI();

    void run();

private:


    void processEvents();
    void updateLogic();
    void renderImGui();

    GeoPos calculateNadir(const Eigen::Matrix3f& R);

    // Données d'état
    sf::RenderWindow window;
    sf::Clock deltaClock;

    std::vector<Pixel> currentPhoto;
    Eigen::Matrix3f R_real;
    Eigen::Matrix3f R_calculated;

    std::future<SolveResult> solverFuture;
    uint32_t matchedIdA = 0;
    uint32_t matchedIdB = 0;
    uint32_t matchedIdC = 0;
    float certainty = 0.0f;

    std::vector<std::vector<uint32_t>> allTriangles;
    float animationTimer = 0.0f;
    int currentTriangleIdx = -1;
    bool isScanning = false;

    GeoPos nadir = {0.0f, 0.0f};
    bool isSolved = false;

    // texture
    sf::Texture earthTexture;
    sf::Sprite earthSprite;
    bool earthMapLoaded = false;
};



#endif //LOSTINSPACE_STARTRACKERUI_HPP
