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

private:
    struct Particle{
        Particle(glm::vec2 p) : pos(p){

        }

        bool active = true;

        glm::ivec2 pos;
        glm::vec2 velocity = glm::vec2(0.0f);

        float sediment = 0.0f;

    };
};


#endif //APPLICATION_EROSIONSIMULATOR_H
