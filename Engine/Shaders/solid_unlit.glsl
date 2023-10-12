#vertex
#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}


#fragment
#version 460 core

out vec4 FragColor;

uniform vec3 color;


void main() {
    FragColor = vec4(color, 1.0);
}
