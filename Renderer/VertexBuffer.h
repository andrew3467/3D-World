//
// Created by apgra on 9/18/2023.
//

#ifndef APPLICATION_VERTEXBUFFER_H
#define APPLICATION_VERTEXBUFFER_H


class VertexBuffer {
public:
    VertexBuffer(void* data);
    ~VertexBuffer();

    void bind();
    void unbind();

private:
    void setData(void* data);


private:
    unsigned int m_ID;
};


#endif //APPLICATION_VERTEXBUFFER_H
