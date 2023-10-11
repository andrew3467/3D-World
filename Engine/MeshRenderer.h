//
// Created by apgra on 10/11/2023.
//

#ifndef APPLICATION_MESHRENDERER_H
#define APPLICATION_MESHRENDERER_H


#include "Mesh.h"

class MeshRenderer {
public:
    MeshRenderer(Mesh* mesh);
    ~MeshRenderer();

public:
    void draw(glm::mat4 modelMatrix) const;
    void setShader(Shader* shader);


private:
    Mesh* m_Mesh;
    Shader* m_Shader;
};


#endif //APPLICATION_MESHRENDERER_H
