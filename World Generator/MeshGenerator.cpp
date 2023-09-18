//
// Created by apgra on 9/18/2023.
//

#include "MeshGenerator.h"

MeshData MeshGenerator::rectangleMesh(unsigned int width, unsigned int height) {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    int vertIndex = 0;
    for(int y = 0; y < width; y++){
        for(int x = 0; x < height; x++){
            vertices.emplace_back(x, y, 0);

            if(x < width && y < height){
                indices.push_back(vertIndex);
                indices.push_back(vertIndex + width);
                indices.push_back(vertIndex + 1);

                indices.push_back(vertIndex + width);
                indices.push_back(vertIndex + width + 1);
                indices.push_back(vertIndex + 1);
            }

            vertIndex++;
        }
    }

    MeshData data = {vertices, indices};
    return data;
}
