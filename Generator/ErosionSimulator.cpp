//
// Created by apgra on 10/25/2023.
//

#include <random>
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

glm::vec2 calculateGradient(std::vector<std::vector<float>> &map, ErosionSimulator::Grid &pGrid, float u, float v){
    auto tr =  pGrid.TopRight();
    auto tl = pGrid.TopLeft();
    auto br = pGrid.BottomRight();
    auto bl = pGrid.BottomLeft();

    glm::vec2 p1 = {
            (map[br.x][br.y] - map[bl.x][bl.y]) * (1 - u),
            (map[tl.x][tl.y] - map[bl.x][bl.y]) * (1 - v)

    };

    glm::vec2 p2 = {
            (map[tr.x][tr.y] - map[tl.x][tl.y]) * u,
            (map[tr.x][tr.y] - map[br.x][br.y]) * v
    };

    return p1 + p2;
}

float randMToN(float M, float N)
{
    return M + (rand() / ( RAND_MAX / (N-M) ) ) ;
}

void ErosionSimulator::SimulateErosion2D(std::vector<std::vector<float>> &map, ErosionConfig &config, int seed) {
    std::vector<Particle> particles;

    int mapWidth = map.size();
    int mapHeight = map[0].size();

    srand(seed);

    //Initialize particles
    for(int i = 0; i < config.numDroplets; i++){
        particles.emplace_back(glm::ivec2(randMToN(0, mapWidth), randMToN(0, mapHeight)));
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

            Grid pGrid = {
                    glm::floor(p.pos),
                    glm::ceil(p.pos)
            };

            //p.dir = glm::normalize(dir * config.inertia - glm::vec2(calculateGradient(map, pGrid, p.pos.x - p.prevPos.x, p.pos.y - p.prevPos.y) * (1 - config.inertia)));
            p.dir = dir * config.inertia - glm::vec2(calculateGradient(map, pGrid, p.pos.x - p.prevPos.x, p.pos.y - p.prevPos.y) * (1 - config.inertia));

            p.prevPos = p.pos;
            p.pos += p.dir;




            //Positive if drop moved uphill, negative for downhill
            float heightDiff = map[pGrid.BottomLeft().x][pGrid.BottomLeft().y] - map[pGrid.TopRight().x][pGrid.TopRight().y];


            float c = fmax(-heightDiff, config.minSlope) * p.velocity * p.water * config.particleCapacity;

            float deposition = (p.sediment - c) * config.depositionRate;
            float erosion = fmin((c - p.sediment) * config.erosionRate, -heightDiff);

            p.velocity = sqrt(p.velocity * p.velocity + heightDiff * GRAVITY);
            p.water = p.water * (1 - config.evaporationRate);

            map[pGrid.BottomLeft().x][pGrid.BottomLeft().y] -= c;

            p.stepsTaken++;
        }
    }
}
