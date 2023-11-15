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
        Particle(glm::ivec2 p, float r, glm::vec2 d) : pos(p), prevPos(p), radius(r), dir(d) {

        }

        int stepsTaken = 0;

        glm::vec2 prevPos;
        glm::vec2 pos;

        glm::vec2 dir;

        float velocity = 0.0f;
        float sediment = 0.0f;
        float water = 0.0f;

        float radius;
    };
};


#endif //APPLICATION_EROSIONSIMULATOR_H
