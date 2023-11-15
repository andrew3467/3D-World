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
float randMToN(float M, float N)
{
    return M + (rand() / (RAND_MAX / (N-M)));
}


glm::vec2 calcGradient(std::vector<std::vector<float>> &map, glm::ivec2 p){
    //Interpolation parameters, 0.5f is set to center of cell
    float u = 0.5f;
    float v = 0.5f;


    glm::vec2 p1 = {
            (map[p.x + 1][p.y] - map[p.x][p.y]) * (1 - v),
            (map[p.x][p.y + 1] - map[p.x][p.y] * (1 - u))
    };

    glm::vec2 p2 = {
            (map[p.x + 1][p.y + 1] - map[p.x][p.y + 1]) * v,
            (map[p.x + 1][p.y + 1] - map[p.x + 1][p.y] * u)
    };

    return p1 + p2;
}


void ErosionSimulator::SimulateErosion2D(std::vector<std::vector<float>> &map, ErosionConfig &config, int seed) {
    std::queue<Particle> particles;

    int mapWidth = map.size();
    int mapHeight = map[0].size();

    srand(seed);

    //Initialize particles
    for (int i = 0; i < config.numDroplets; i++) {
        particles.push(Particle(glm::vec2(randMToN(0, mapWidth - 1), randMToN(0, mapHeight - 1)), config.particleRadius, glm::vec2(
                randMToN(0.0f, 1.0f), randMToN(0.0f, 1.0f))));
    }

    while(!particles.empty()){
        auto p = particles.front();
        particles.pop();

        if(p.stepsTaken >= config.maxSteps){
            continue;
        }

        //Check bounds [0, size-1]
        if(p.pos.x < 0 || p.pos.y < 0){
            continue;
        }
        if(p.pos.x >= mapWidth - 1 || p.pos.y >= mapHeight - 1){
            continue;
        }



        glm::vec2 g = calcGradient(map, p.prevPos);
        p.dir = p.dir * config.inertia - g * (1- config.inertia);
        p.dir = glm::normalize(p.dir);


        p.prevPos = p.pos;
        p.pos = p.prevPos + p.dir;

        glm::ivec2 iPos = floor(p.pos);
        glm::ivec2 iPrevPos = floor(p.prevPos);

        //Check bounds [0, size-1]
        if(iPos.x < 0 || iPos.y < 0){
            continue;
        }
        if(iPos.x >= mapWidth - 1 || iPos.y >= mapHeight - 1){
            continue;
        }

        float hdif = map[iPrevPos.x][iPrevPos.y] - map[iPos.x][iPos.y];
        //particle carry
        float c = fmax(-hdif, config.minSlope) * p.velocity * p.water * config.particleCapacity;


        float deposition = fmin(0.0f, (p.sediment - c) * config.depositionRate);
        float erosion = fmin(c - p.sediment * config.erosionRate, -hdif);


        //Update Map
        map[iPos.x][iPos.y] += deposition;
        map[iPrevPos.x][iPrevPos.y] += erosion;


        p.velocity = sqrt(p.velocity * p.velocity + hdif * GRAVITY);
        p.water = p.water * (1 - config.evaporationRate);


        p.stepsTaken++;
        particles.push(p);
    }
}
