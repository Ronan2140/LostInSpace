//
// Created by ronan2140 on 4/7/26.
//

#ifndef LOSTINSPACE_CONFIG_HPP
#define LOSTINSPACE_CONFIG_HPP
#include "imgui-SFML.h"


namespace Config {

    // Camera Simulator
    static constexpr int RESOLUTION = 1024;
    static constexpr float FOV_DEG = 20.0f;
    static constexpr float CENTER = RESOLUTION / 2.0f;
    inline const float FOV_RAD_HALF = (FOV_DEG * std::numbers::pi_v<float> / 180.0f) / 2.0f;
    inline const float FOCAL_LENGTH = CENTER / std::tan(FOV_RAD_HALF);

    // StarCatalog
    static constexpr auto CATALOG_PATH = "data/stars.csv";
    constexpr float CATALOGUE_MAG_LIMIT = 6.0f;

    // Triangle catalog
    constexpr float DB_MAG_LIMIT = 5.0f;
    constexpr float MAX_ANGULAR_DIST = FOV_DEG;
    constexpr float MATCH_TOLERANCE_DEG = 0.08f;
    // Matcher
    constexpr int MAX_STARS_TESTED = 15;
    constexpr int STARS_CERTAINTY = 8;

    // UI

    // Window
    constexpr int WINDOW_WIDTH = 1400;
    constexpr int WINDOW_HEIGHT = 850;

    constexpr float SENSOR_RES = 1024.0f;


    // Triangle animation
    constexpr float ANIMATION_SCAN_DURATION = 2.0f;
    constexpr int TRIANGLES_PER_FRAME = 5;

    inline const sf::Color COLOR_BG(15, 15, 20);




}
#endif //LOSTINSPACE_CONFIG_HPP
