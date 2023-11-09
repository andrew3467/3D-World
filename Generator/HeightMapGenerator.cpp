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
                glm::vec2 p = offsets[i] + position + glm::vec2(xPos / size, zPos / size) * scale;
                noiseValue += SimplexNoise::noise(p.x, p.y) * weight;

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

std::vector<std::vector<float>> HeightMapGenerator::GenerateHeightMapfBm(glm::vec2 position, NoiseConfig &noiseConfig, TerrainConfig& terrainConfig) {
    float minValue = 0xFFF0000, maxValue = -0xFFF00000;

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
            float xPos = x / (float) (resolution);
            float zPos = y / (float) (resolution);

            float noiseValue = 0;

            float fre = 1.0f;
            float amplitude = 1.0f;

            for (int i = 0; i < noiseConfig.octaves; i++) {
                glm::vec2 p = offsets[i] + position + glm::vec2(xPos / size, zPos / size);
                noiseValue += amplitude * ((SimplexNoise::noise(p.x * fre, p.y * fre) + 1.0f) / 2.0f);

                fre *= noiseConfig.lacunarity;
                amplitude *= noiseConfig.persistence;
            }


            minValue = fmin(minValue, noiseValue);
            maxValue = fmax(maxValue, noiseValue);

            map[x][y] = pow(noiseValue, noiseConfig.exp);
        }
    }



    return map;
}
