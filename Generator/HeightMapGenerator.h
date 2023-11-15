//
// Created by apgra on 10/23/2023.
//

#ifndef APPLICATION_HEIGHTMAPGENERATOR_H
#define APPLICATION_HEIGHTMAPGENERATOR_H


#include "TerrainChunk.h"

class HeightMapGenerator {
public:
    static std::vector<std::vector<float>> GenerateHeightMap(glm::vec2 position, NoiseConfig& noiseConfig, TerrainConfig& terrainConfig);
    static std::vector<std::vector<float>> GenerateHeightMapfBm(glm::vec2 position, NoiseConfig& noiseConfig, TerrainConfig& terrainConfig);
    static std::vector<std::vector<float>> GenerateNoiseless(TerrainConfig &config, ErosionConfig &erosionConfig);
};


#endif //APPLICATION_HEIGHTMAPGENERATOR_H
