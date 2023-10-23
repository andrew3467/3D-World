//
// Created by apgra on 10/23/2023.
//

#ifndef APPLICATION_HEIGHTMAPGENERATOR_H
#define APPLICATION_HEIGHTMAPGENERATOR_H


#include "TerrainChunk.h"

class HeightMapGenerator {
public:
    static std::vector<float> GenerateHeightMap(glm::vec2 position, TerrainConfig& config);
};


#endif //APPLICATION_HEIGHTMAPGENERATOR_H
