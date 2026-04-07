//
// Created by ronan2140 on 4/7/26.
//

#include "../include/StarTrackerUI.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cmath>
#include <iostream>
#include <cstdio>

#include "Config.hpp"
#include "StarMatcher.hpp"
#include "StarCatalogue.hpp"

// Use constexpr instead of #define for constants

StarTrackerUI::StarTrackerUI() : window(sf::VideoMode(1400, 850), "Star Tracker OS v1.0") {
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    R_real.setIdentity();
    R_calculated.setIdentity();
    StarCatalogue::loadFromCSV(Config::CATALOG_PATH);

    TriangleDatabase db;
    db.generate();

    if (earthTexture.loadFromFile("data/MapOfEarth.jpg")) {
        earthSprite.setTexture(earthTexture);
        earthMapLoaded = true;
    }
}

StarTrackerUI::~StarTrackerUI() {
    ImGui::SFML::Shutdown();
}

void StarTrackerUI::run() {
    while (window.isOpen()) {
        processEvents();
        updateLogic();

        window.clear(sf::Color(15, 15, 20));
        renderImGui();
        ImGui::SFML::Render(window);

        window.display();
    }
}

void StarTrackerUI::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(window, event);
        if (event.type == sf::Event::Closed) {
            window.close();
        }
    }
}

void StarTrackerUI::updateLogic() {
    float dt = deltaClock.restart().asSeconds();
    ImGui::SFML::Update(window, sf::seconds(dt));

    if (isScanning && solverFuture.valid()) {
        // Check if the background thread has finished without blocking the UI
        if (solverFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            SolveResult result = solverFuture.get();
            isScanning = false;
            isSolved = result.success;

            if (result.success) {
                R_calculated = result.R;
                matchedIdA = result.idA;
                matchedIdB = result.idB;
                matchedIdC = result.idC;
                certainty = result.certainty;
                nadir = calculateNadir(R_calculated);
            } else {
                certainty = 0.0f;
            }
        }
    }
}
void StarTrackerUI::renderImGui() {
    // --- TELEMETRY & CONTROL WINDOW ---
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(380, 460), ImGuiCond_Always);
    ImGui::Begin("Telemetry & Control", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    if (ImGui::Button("NEW CAPTURE (RANDOM ATTITUDE)", ImVec2(-1, 45))) {
        R_real = CameraSimulator::generateRandomAttitude();
        currentPhoto = CameraSimulator::takePicture(R_real);
        isSolved = false;
        isScanning = false;
        certainty = 0.0f;
        allTriangles.clear();
    }

    ImGui::Spacing();

    if (ImGui::Button("SOLVE LIS ATTITUDE", ImVec2(-1, 45)) && !currentPhoto.empty()) {
        allTriangles.clear();
        const size_t n = currentPhoto.size();
        if (n >= 3) {
            for (size_t i = 0; i < n - 2; ++i) {
                for (size_t j = i + 1; j < n - 1; ++j) {
                    for (size_t k = j + 1; k < n; ++k) {
                        allTriangles.push_back({currentPhoto[i].star_id, currentPhoto[j].star_id, currentPhoto[k].star_id});
                    }
                }
            }
        }
        isScanning = true;
        isSolved = false;
        solverFuture = std::async(std::launch::async, [this]() {
            return StarMatcher::solvePicture(currentPhoto);
        });
    }

    ImGui::Separator();

    if (isScanning) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "SYSTEM STATUS: SCANNING...");
        ImGui::ProgressBar(std::min(animationTimer / 2.0f, 1.0f), ImVec2(-1, 30), "COMPUTING...");
    }
    else if (isSolved) {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "SYSTEM STATUS: LOCK OK");

        ImVec4 barColor = ImVec4(1.0f - (certainty/100.f), certainty/100.f, 0.1f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
        char overlay[64];
        std::snprintf(overlay, sizeof(overlay), "%.1f%% RELIABILITY", certainty);
        ImGui::ProgressBar(certainty / 100.0f, ImVec2(-1, 30), overlay);
        ImGui::PopStyleColor();

        ImGui::Spacing();
        ImGui::Text("CURRENT NADIR");
        ImGui::BulletText("Lat: %.3f deg", nadir.lat);
        ImGui::BulletText("Lon: %.3f deg", nadir.lon);

        ImGui::Separator();
        ImGui::Text("ATTITUDE VALIDATION (SO3)");

        auto renderMatrix = [](const char* id, const Eigen::Matrix3f& mat) {
            if (ImGui::BeginTable(id, 3, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame)) {
                for (int i = 0; i < 3; i++) {
                    ImGui::TableNextRow();
                    for (int j = 0; j < 3; j++) {
                        ImGui::TableNextColumn();
                        ImGui::Text("%.4f", mat(i, j));
                    }
                }
                ImGui::EndTable();
            }
        };

        ImGui::TextDisabled("Real Matrix:");
        renderMatrix("RealMat", R_real);
        ImGui::TextDisabled("Calculated Matrix:");
        renderMatrix("CalcMat", R_calculated);

        float error = (R_calculated - R_real).norm();
        ImGui::Text("Matrix Error (Norm): %.6f", error);
    }
    ImGui::End(); // CRITICAL: Ends Telemetry window

    // --- GROUND TRACKING WINDOW ---
    ImGui::SetNextWindowPos(ImVec2(10, 480), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(380, 220), ImGuiCond_Always);
    ImGui::Begin("Ground Tracking", nullptr, ImGuiWindowFlags_NoResize);

    ImVec2 mapSize(360, 180);
    ImVec2 mapPos = ImGui::GetCursorScreenPos();
    ImDrawList* mapDL = ImGui::GetWindowDrawList();

    if (earthMapLoaded) {
        mapDL->AddImage((void*)(intptr_t)earthTexture.getNativeHandle(), mapPos, ImVec2(mapPos.x + mapSize.x, mapPos.y + mapSize.y));
    }

    if (isSolved) {
        float x = ((nadir.lon + 180.0f) / 360.0f) * mapSize.x;
        float y = ((90.0f - nadir.lat) / 180.0f) * mapSize.y;
        ImVec2 satPos = ImVec2(mapPos.x + x, mapPos.y + y);
        mapDL->AddCircleFilled(satPos, 4.0f, IM_COL32(255, 0, 0, 255));
        mapDL->AddCircle(satPos, 8.0f, IM_COL32(255, 0, 0, 150), 12, 1.5f);
    }
    ImGui::End();

    // --- CCD SENSOR VIEW WINDOW ---
    ImGui::SetNextWindowPos(ImVec2(400, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(990, 830), ImGuiCond_Always);
    ImGui::Begin("CCD Sensor View", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize(970, 780);

    drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), IM_COL32(5, 5, 10, 255));
    drawList->PushClipRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), true);

    if (isScanning && !allTriangles.empty()) {
        for (int i = 0; i < Config::TRIANGLES_PER_FRAME; ++i) {
            auto& tri = allTriangles[rand() % allTriangles.size()];
            std::vector<ImVec2> pts;
            for (uint32_t id : tri) {
                for (const auto& p : currentPhoto) {
                    if (p.star_id == id) {
                        pts.push_back(ImVec2(canvasPos.x + (p.u / Config::SENSOR_RES) * canvasSize.x, canvasPos.y + (p.v / Config::SENSOR_RES) * canvasSize.y));
                        break;
                    }
                }
            }
            if (pts.size() == 3) drawList->AddPolyline(pts.data(), 3, IM_COL32(0, 200, 255, 80), ImDrawFlags_Closed, 1.0f);
        }
    }

    if (isSolved) {
        std::vector<ImVec2> winningPts;
        uint32_t ids[3] = {matchedIdA, matchedIdB, matchedIdC};
        for (uint32_t id : ids) {
            for (const auto& p : currentPhoto) {
                if (p.star_id == id) {
                    winningPts.push_back(ImVec2(canvasPos.x + (p.u / Config::SENSOR_RES) * canvasSize.x, canvasPos.y + (p.v / Config::SENSOR_RES) * canvasSize.y));
                    break;
                }
            }
        }
        if (winningPts.size() == 3) {
            drawList->AddPolyline(winningPts.data(), 3, IM_COL32(50, 255, 50, 255), ImDrawFlags_Closed, 2.5f);
            drawList->AddTriangleFilled(winningPts[0], winningPts[1], winningPts[2], IM_COL32(50, 255, 50, 40));
            for(int i=0; i<3; ++i) {
                char idBuf[16];
                std::snprintf(idBuf, sizeof(idBuf), "HIP %u", ids[i]);
                drawList->AddText(ImVec2(winningPts[i].x + 5, winningPts[i].y - 15), IM_COL32(50, 255, 50, 255), idBuf);
            }
        }
    }

    for (const auto& p : currentPhoto) {
        ImVec2 screenPos = ImVec2(canvasPos.x + (p.u / Config::SENSOR_RES) * canvasSize.x, canvasPos.y + (p.v / Config::SENSOR_RES) * canvasSize.y);
        float b = std::clamp((8.0f - p.v_mag) / 5.0f, 0.5f, 3.0f);
        drawList->AddCircleFilled(screenPos, 1.2f * b, IM_COL32(255, 255, 255, 255));

        if (ImGui::IsMouseHoveringRect(ImVec2(screenPos.x-5, screenPos.y-5), ImVec2(screenPos.x+5, screenPos.y+5))) {
            ImGui::BeginTooltip();
            ImGui::Text("Coord: (%.1f, %.1f)\nMag: %.2f", p.u, p.v, p.v_mag);
            ImGui::EndTooltip();
        }
    }

    drawList->PopClipRect();
    ImGui::End();
}


GeoPos StarTrackerUI::calculateNadir(const Eigen::Matrix3f& R) {
    Eigen::Vector3f gaze = R.transpose() * Eigen::Vector3f(0.0f, 0.0f, 1.0f);

    float lat = std::asin(gaze.z()) * 180.0f / 3.14159f;
    float lon = std::atan2(gaze.y(), gaze.x()) * 180.0f / 3.14159f;

    return {lat, lon};
}