//
// Created by apgra on 10/23/2023.
//

#include "HeightMapGenerator.h"
#include "../external/Simplex/SimplexNoise.h"

std::vector<std::vector<float>> HeightMapGenerator::GenerateHeightMap(glm::vec2 position, NoiseConfig& noiseConfig, TerrainConfig& terrainConfig) {
    float minValue = 0xFFF0000, maxValue = -0xFFF00000;

    int resolution = std::pow(2, terrainConfig.resolution);

    //Vertices per line: size + (res - 1) * (size - 1)
    int size = terrainConfig.size + ((resolution - 1) * (terrainConfig.size - 1));

    std::vector<std::vector<float>> map(size, std::vector<float>(size, 0.0f));

    srand(terrainConfig.seed);
    std::vector<glm::vec2> offsets;
    for(int i = 0; i < noiseConfig.octaves; i++) {
        offsets.emplace_back(rand(), rand());
    }



    for(int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++) {
            float xPos = x / (float) (resolution);
            float zPos = z / (float) (resolution);

            float noiseValue = 0;
            float scale = 1;
            float weight = 1;
            for (int i = 0; i < noiseConfig.octaves; i++) {
                glm::vec2 p = offsets[i] + position + glm::vec2(xPos, zPos) * scale;
                noiseValue += 0.5f;//SimplexNoise::noise(p.x, p.y) * weight;

                weight *= noiseConfig.persistence;
                scale *= noiseConfig.lacunarity;
            }


            minValue = fmin(minValue, noiseValue);
            maxValue = fmax(maxValue, noiseValue);





            map[x][z] = noiseValue;
        }
    }

    //Normalize Mapping
    if(maxValue != minValue){
        for(int z = 0; z < map[0].size(); z++){
            for(int x = 0; x < map.size(); x++){
                map[x][z] = (map[x][z] + 1) / 2.0f;
            }
        }
    }

    return map;
}

float fbm(glm::vec2 p, NoiseConfig &config, std::vector<glm::vec2> &offsets) {
    float res = 0.0f;

    float fre = 1.0f;
    float amplitude = 1.0f;

    for (int i = 0; i < config.octaves; i++) {
        glm::vec2 q = offsets[i] + p;
        res += amplitude * SimplexNoise::noise(q.x * fre, q.y * fre);

        fre *= config.lacunarity;
        amplitude *= config.persistence;
    }

    return res;
}

float pattern(glm::vec2 &p, NoiseConfig &config, std::vector<glm::vec2> &offsets) {
    glm::vec2 q(
                fbm(p + glm::vec2(0.0f, 0.0f), config, offsets),
                fbm(p + glm::vec2(5.2f, 1.3f), config, offsets)
            );

    glm::vec2 r(
                fbm(p + (q * glm::vec2(4.0f)) + glm::vec2(1.7f, 9.2f), config, offsets),
                fbm(p + (q * glm::vec2(4.0f)) + glm::vec2(8.3, 2.8f), config, offsets)
            );

    return fbm(p + (r * glm::vec2(4.0f)), config, offsets);
}

std::vector<std::vector<float>> HeightMapGenerator::GenerateHeightMapfBm(glm::vec2 position, NoiseConfig &noiseConfig, TerrainConfig& terrainConfig) {
    float minValue = 0xFFFF0000, maxValue = -0x10000;

    int resolution = std::pow(2, terrainConfig.resolution);

    //Vertices per line: size + (res - 1) * (size - 1)
    int size = terrainConfig.size + ((resolution - 1) * (terrainConfig.size - 1));

    std::vector<std::vector<float>> map(size, std::vector<float>(size, 0.0f));
    std::vector<unsigned int> indices;

    srand(terrainConfig.seed);
    std::vector<glm::vec2> offsets;
    for(int i = 0; i < noiseConfig.octaves; i++) {
        offsets.emplace_back(rand(), rand());
    }


    for(int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            glm::vec2 pos( x / (float) (resolution),
                            y / (float) (resolution));

            float noiseValue = pattern(pos ,noiseConfig, offsets);


            minValue = fmin(minValue, noiseValue);
            maxValue = fmax(maxValue, noiseValue);

            map[x][y] = noiseValue;
        }
    }

    //Apply exponent to normalized mapping
    if(maxValue != minValue){
        for(int z = 0; z < map[0].size(); z++){
            for(int x = 0; x < map.size(); x++){
                //map[x][z] = pow((map[x][z] + 1) / 2.0f, noiseConfig.exp);
                map[x][z] = (map[x][z]-minValue)/(maxValue-minValue);
            }
        }
    }


    return map;
}
