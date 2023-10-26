//
// Created by apgra on 10/25/2023.
//

#ifndef APPLICATION_EROSIONSIMULATOR_H
#define APPLICATION_EROSIONSIMULATOR_H


#include "TerrainChunk.h"

class ErosionSimulator {
public:
    //Values in map vector are modified directly
    static void SimulateErosion2D(std::vector<std::vector<float>> &map, ErosionConfig& config, int seed);

    struct Particle{
        Particle(glm::vec2 p) : pos(p), prevPos(pos) {

        }

        bool active = true;
        int stepsTaken = 0;

        glm::ivec2 pos;
        glm::ivec2 prevPos;
        glm::vec2 dir = glm::vec2(0.0f);

        float velocity = 1.0f;

        float water = 1.0f;
        float sediment = 0.0f;
    };
};


#endif //APPLICATION_EROSIONSIMULATOR_H
