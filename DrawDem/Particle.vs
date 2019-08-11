#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
out vec2 TexCoords;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 offset;
uniform vec4 color;

void main()
{
    TexCoords = texCoord;
    gl_Position =  projection * view  * model * vec4(position+offset, 1f);
}