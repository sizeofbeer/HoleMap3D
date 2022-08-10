#version 410

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

uniform mat4 pmvMatrix;

out vec4 fragColor;

void main() {

  gl_Position = pmvMatrix * vec4(inPosition, 1.0);
  fragColor = inColor;
}