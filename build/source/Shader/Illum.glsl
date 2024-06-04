#version 430

layout(location = 0) in vec3 VertexColor;
layout(location = 0) out vec4 PixelColor;

void main() {
    PixelColor = vec4(VertexColor, 1.0);
}