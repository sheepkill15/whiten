#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    vec3 newPos = aPos;
    newPos.x = -newPos.x;
    gl_Position = vec4(newPos, 1.0);
    TexCoord = aTexCoord;
}