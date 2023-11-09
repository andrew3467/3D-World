//
// Created by apgra on 10/25/2023.
//

#include <random>
#include <queue>
#include "ErosionSimulator.h"

#define GRAVITY 9.81

int randMToN(int M, int N)
{
    return M + (rand() / (RAND_MAX / (N-M)));
}


void ErosionSimulator::SimulateErosion2D(std::vector<std::vector<float>> &map, ErosionConfig &config, int seed) {
    std::vector<Particle> particles;

    int mapWidth = map.size();
    int mapHeight = map[0].size();

    std::vector<std::vector<float>> results(mapWidth, std::vector<float>(mapHeight, 0.0f));

    srand(seed);

    //Initialize particles
    for (int i = 0; i < config.numDroplets; i++) {
        particles.emplace_back(glm::vec2(randMToN(0, mapWidth - 1), randMToN(0, mapHeight - 1)), config.particleRadius);
    }

    for(auto& p : particles){

    }
}
