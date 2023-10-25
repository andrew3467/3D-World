//
// Created by apgra on 10/23/2023.
//

#include "HeightMapGenerator.h"
#include "../external/Simplex/SimplexNoise.h"

std::vector<float> HeightMapGenerator::GenerateHeightMap(glm::vec2 position, TerrainConfig& config) {
    std::vector<float> map;
    std::vector<unsigned int> indices;

    float minValue = 0xFFF0000, maxValue = -0xFFF00000;

    int resolution = std::pow(2, config.resolution);

    //Vertices per line: size + (res - 1) * (size - 1)
    int size = config.size + ((resolution - 1) * (config.size - 1));

    srand(config.seed);
    std::vector<glm::vec2> offsets;
    for(int i = 0; i < config.octaves; i++) {
        offsets.emplace_back(rand(), rand());
    }


    int vertIndex = 0;
    for(int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++) {
            float xPos = x / (float) (resolution);
            float zPos = z / (float) (resolution);

            float noiseValue = 0;
            float scale = 1;
            float weight = 1;
            for (int i = 0; i < config.octaves; i++) {
                glm::vec2 p = offsets[i] + position + glm::vec2(xPos / size, zPos / size) * scale;
                noiseValue += SimplexNoise::noise(p.x, p.y) * weight;

                weight *= config.persistence;
                scale *= config.lacunarity;
            }


            minValue = fmin(minValue, noiseValue);
            maxValue = fmax(maxValue, noiseValue);


            map.push_back(noiseValue);


            if (x < size - 1 && z < size - 1) {
                indices.push_back(vertIndex);
                indices.push_back(vertIndex + size);
                indices.push_back(vertIndex + size + 1);

                indices.push_back(vertIndex + size + 1);
                indices.push_back(vertIndex + 1);
                indices.push_back(vertIndex);
            }

            vertIndex++;
        }
    }

    //Normalize Mapping
    if(maxValue != minValue){
        for(int i = 0; i < map.size(); i++){
            map[i] = (map[i] + 1) / 2.0f;
        }
    }

    return map;
}

void HeightMapGenerator::SimulateErosion(std::vector<float> &map, ErosionConfig &config) {
    
}
