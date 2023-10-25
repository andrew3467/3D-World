//
// Created by apgra on 10/25/2023.
//

#include "ErosionSimulator.h"

//TODO: Handle cases where particle is on edge
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
        };ss
ss
        glm::vec3 e0crosse1 = glm::cross(e0, e1);ss
ss
        norm += e0crosse1;ss
    }ss

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

            //Particle has fallen off mesh
            if(p.pos.x < 0 || p.pos.y < 0 || p.pos.x > mapWidth - 1 || p.pos.y > mapHeight - 1){
                p.active = false;
                continue;
            }

            auto norm = surfaceNormal(map, p.pos);

            if(norm.y == 1){
                p.active = false;
                continue;
            }

            float deposit = p.sediment * config.depositionRate * norm.y;
            float erosion = config.erosionRate * (1 - norm.y) * fmin(1, i * config.iterationScale);

            map[p.pos.x][p.pos.y] += deposit - erosion;
            p.sediment += erosion - deposit;

            const float speed = 1.0f;
            p.velocity.x = config.friction * p.velocity.x + norm.x * speed;
            p.velocity.y = config.friction * p.velocity.y + norm.z * speed;

            p.pos += p.velocity;
        }
    }
}
