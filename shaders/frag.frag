#version 430 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D tex;

struct Rect {
    int x0;
    int y0;
    int x1;
    int y1;
};

layout(std140, binding = 4) uniform WindowSize {
    int width;
    int height;
};

layout(std430, binding = 5) buffer Rects {
    int region;
    int regionCount;
};

layout(std430, binding = 6) buffer Tresholds {
    float tresholds[];
};

int whichRegion(ivec2 coords) {
    int regionX = coords.x / (width / regionCount);
    int regionY = coords.y / (height / regionCount);
    return regionY * regionCount + regionX;
}

void main()
{
    vec4 col = texture(tex, TexCoord);
    const float this_pixel = col.r + col.g + col.b;
    if(this_pixel > tresholds[whichRegion(ivec2(TexCoord.x * width, TexCoord.y * height))]) {
        FragColor = vec4(1);
        return;
    }
    FragColor = col;
} 