#vertex
#version 460

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 mvp;


void main()
{
    gl_Position =  vec4(aPos, 1.0);
}


#fragment
#version 460 core

out vec4 FragColor;

uniform vec3 color;

void main() {
    FragColor = vec4(color, 1.0);
}
