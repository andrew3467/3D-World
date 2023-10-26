//
// Created by apgra on 10/25/2023.
//

#include "ErosionSimulator.h"

#define GRAVITY 9.81

glm::vec3 surfaceNormal(std::vector<std::vector<float>> &map, glm::ivec2 pos){
    const glm::ivec2 neighbors[4] = {
            {-1, 0},
            {0, 1},
            {1, 0},
            {0, -1}
    };

    int mapWidth = map.size();
    int mapHeight = map[0].size();

    glm::vec3 norm(0.0f);

    //(-,+)
    if(pos.x > 0 && pos.y < mapHeight - 1){
        glm::ivec2 n1 = pos + neighbors[0];
        glm::ivec2 n2 = pos + neighbors[1];

        glm::vec3 e0 = {
                n1.x - pos.x,
                map[pos.x][pos.y] - map[n1.x][n1.y],
                n1.y - pos.y
        };
        glm::vec3 e1 = {
                n2.x - pos.x,
                map[pos.x][pos.y] - map[n2.x][n2.y],
                n2.y - pos.y
        };

        glm::vec3 e0crosse1 = glm::cross(e0, e1);

        norm += e0crosse1;
    }

    //(+,-)
    if(pos.y > 0 && pos.x < mapWidth - 1){
        glm::ivec2 n1 = pos + neighbors[2];
        glm::ivec2 n2 = pos + neighbors[3];

        glm::vec3 e0 = {
                n1.x -  pos.x,
                map[pos.x][pos.y] - map[n1.x][n1.y],
                n1.y - pos.y
        };
        glm::vec3 e1 = {
                n2.x - pos.x,
                map[pos.x][pos.y] - map[n2.x][n2.y],
                n2.y - pos.y
        };

        glm::vec3 e0crosse1 = glm::cross(e0, e1);

        norm += e0crosse1;
    }

    return glm::normalize(norm);
}

glm::vec2 calculateGradient(std::vector<std::vector<float>> &map, ErosionSimulator::Particle &p){
    auto& pos = p.pos;
    auto& prevPos = p.prevPos;

    float u = prevPos.x - pos.x;
    float v = prevPos.y - pos.y;

    glm::vec2 p1(
            (map[pos.x+1][pos.y] - map[pos.x][pos.y]) * (1 - v),
            (map[pos.y][pos.y+1] - map[pos.x][pos.y]) * (1 - u)
            );
    glm::vec2 p2(
            (map[pos.x+1][pos.y+1] - map[pos.x][pos.y+1]) * v,
            (map[pos.x+1][pos.y+1] - map[pos.x-1][pos.y]) * u
            );

    return p1 + p2;
}

void ErosionSimulator::SimulateErosion2D(std::vector<std::vector<float>> &map, ErosionConfig &config, int seed) {
    std::vector<Particle> particles;

    int mapWidth = map.size();
    int mapHeight = map[0].size();

    //Initialize particles
    for(int i = 0; i < config.numDroplets; i++){
        particles.emplace_back(glm::ivec2(rand() % mapWidth, rand() % mapHeight));
    }

    for(int i = 0; i < config.numIterations; i++){
        for(auto& p : particles){
            if(!p.active){
                continue;
            }
            if(p.stepsTaken > config.maxSteps){
                p.active = false;
                continue;
            }

            auto prevPos = p.prevPos;

            auto dir = p.dir;

            p.dir = glm::normalize(dir * config.inertia - glm::vec2(GRAVITY * (1 - config.inertia)));

            p.prevPos = p.pos;
            p.pos += p.dir;

            //Positive if drop moved uphill, negative for downhill
            float heightDiff = map[p.pos.x][p.pos.y] - map[prevPos.x][prevPos.y];


            float c = fmax(-heightDiff, config.minSlope) * p.velocity * p.water * config.particleCapacity;

            float deposition = (p.sediment - c) * config.depositionRate;
            float erosion = fmin((c - p.sediment) * config.erosionRate, -heightDiff);

            p.velocity = sqrt(p.velocity * p.velocity + heightDiff * GRAVITY);
            p.water = p.water * (1 - config.evaporationRate);

            map[p.pos.x][p.pos.y] -= c;

        }
    }
}
